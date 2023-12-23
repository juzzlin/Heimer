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

#ifndef FONT_BUTTON_HPP
#define FONT_BUTTON_HPP

#include <QPushButton>

class QFont;

namespace Widgets {

class FontButton : public QPushButton
{
    Q_OBJECT

public:
    explicit FontButton(QWidget * parent = nullptr);

    void setDefaultPointSize(int defaultPointSize);

signals:
    void defaultFontSizeRequested();

    void fontChanged(const QFont & font);

    void fontSizeChanged(int pointSize);

private:
    void updateFont(const QFont & font);

    int m_defaultPointSize = 0;
};

} // namespace Widgets

#endif // FONT_BUTTON_HPP
