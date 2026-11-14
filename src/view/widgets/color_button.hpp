// This file is part of Heimer.
// Copyright (C) 2026 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef COLOR_BUTTON_HPP
#define COLOR_BUTTON_HPP

#include <QColor>
#include <QPushButton>

namespace Widgets {

//! A button that shows a color and opens a color dialog when clicked.
class ColorButton : public QPushButton
{
    Q_OBJECT

public:
    //! \param alphaChannel If true, the color dialog allows choosing the transparency.
    explicit ColorButton(QString text, bool alphaChannel = false, QWidget * parent = nullptr);

    QColor color() const;

    void setColor(const QColor & color);

signals:
    void colorChanged(const QColor & color);

private:
    void updateIcon();

    QColor m_color;

    bool m_alphaChannel;
};

} // namespace Widgets

#endif // COLOR_BUTTON_HPP
