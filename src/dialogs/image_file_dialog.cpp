// This file is part of Heimer.
// Copyright (C) 2022 Jussi Lind <jussi.lind@iki.fi>
//
// Heimer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Heimer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Heimer. If not, see <http://www.gnu.org/licenses/>.

#include "image_file_dialog.hpp"

#include "../settings.hpp"

#include <QFileInfo>
#include <QMessageBox>

namespace Dialogs {

ImageFileDialog::ImageFileDialog(QWidget * parent)
  : QFileDialog(parent)
{
    setNameFilter("(*.jpg *.jpeg *.JPG *.JPEG *.png *.PNG)");
    setViewMode(QFileDialog::Detail);
}

std::optional<Image> ImageFileDialog::loadImage()
{
    const auto path = Settings::V1::loadRecentImagePath();
    setHistory({ path });
    setDirectory(QFileInfo(path).path());

    if (exec()) {
        if (auto fileNames = selectedFiles(); !fileNames.isEmpty()) {
            const auto fileName = fileNames.at(0);
            if (QImage image; image.load(fileName)) {
                Settings::V1::saveRecentImagePath(fileName);
                return { { image, fileName.toStdString() } };
            } else if (fileName != "") {
                QMessageBox::critical(this, tr("Load image"), tr("Failed to load image '") + fileName + "'");
            }
        }
    }

    return {};
}

} // namespace Dialogs
