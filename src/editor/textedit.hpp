// This file is part of Heimer.
// Copyright (C) 2018 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef TEXTEDIT_HPP
#define TEXTEDIT_HPP

#include <QGraphicsTextItem>

class TextEdit : public QGraphicsTextItem
{
public:

    explicit TextEdit(QGraphicsItem * parentItem);

    virtual float maxHeight() const;

    virtual void setMaxHeight(float maxHeight);

    virtual float maxWidth() const;

    virtual void setMaxWidth(float maxWidth);

    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget) override;

    virtual ~TextEdit();

private:

    float m_maxHeight = 0;

    float m_maxWidth = 0;
};

#endif // TEXTEDIT_HPP
