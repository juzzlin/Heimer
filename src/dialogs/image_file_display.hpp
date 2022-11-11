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

#ifndef IMAGE_FILE_DISPLAY_HPP
#define IMAGE_FILE_DISPLAY_HPP

#include <QWidget>

#include <optional>

#include "../image.hpp"

namespace Dialogs {

class ImageFileDisplay : public QWidget
{
    Q_OBJECT

public:
    explicit ImageFileDisplay(QWidget * parent = nullptr);

    bool event(QEvent * event) override;

    QSize sizeHint() const override;

    QSize minimumSizeHint() const override;

    void mousePressEvent(QMouseEvent * event) override;

    void setImage(const Image & image);

protected:
    void paintEvent(QPaintEvent * event) override;

    std::optional<Image> m_image;
};

} // namespace Dialogs

#endif // IMAGE_FILE_DISPLAY_HPP
