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

#ifndef SPECIAL_CONTENT_BUTTON_HPP
#define SPECIAL_CONTENT_BUTTON_HPP

#include <QGraphicsItem>

namespace SpecialContent {

class SpecialContentButton : public QGraphicsItem
{
public:
    enum class Role
    {
        Image,
        WebLink
    };

    SpecialContentButton(Role role);

    QRectF
    boundingRect() const override;

    const QSizeF & size() const;

    void setSize(const QSizeF & newSize);

protected:
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = nullptr) override;

private:
    Role m_role;

    QSizeF m_size;
};

} // namespace SpecialContent

#endif // SPECIAL_CONTENT_BUTTON_HPP
