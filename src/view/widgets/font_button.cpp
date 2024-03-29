// This file is part of Heimer.
// Copyright (C) 2023 Jussi Lind <jussi.lind@iki.fi>
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

#include "font_button.hpp"

#include "../../common/constants.hpp"

#include "simple_logger.hpp"

#include <QFont>
#include <QFontDialog>

namespace Widgets {

FontButton::FontButton(QWidget * parent)
  : QPushButton(parent)
{
    setText(tr("Font") + Constants::Misc::threeDots());

    connect(this, &QPushButton::clicked, this, [=] {
        bool ok;
        auto defaultFont = font();
        emit defaultFontSizeRequested();
        defaultFont.setPointSize(m_defaultPointSize);
        const auto font = QFontDialog::getFont(&ok, defaultFont, this);
        if (ok) {
            // Note: Support for multiple families implemented in Qt 5.13 =>
            juzzlin::L().debug() << "Font family selected: '" << font.family().toStdString() << "'";
            juzzlin::L().debug() << "Font weight selected: " << font.weight();
            updateFont(font);
            emit fontSizeChanged(font.pointSize());
            emit fontChanged(font);
        }
    });
}

void FontButton::updateFont(const QFont & font)
{
    QFont newFont(font);
    newFont.setPointSize(this->font().pointSize());
    setFont(newFont);
}

void FontButton::setDefaultPointSize(int defaultPointSize)
{
    m_defaultPointSize = defaultPointSize;
}

} // namespace Widgets
