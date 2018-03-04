// This file is part of Dementia.
// Copyright (C) 2018 Jussi Lind <jussi.lind@iki.fi>
//
// Dementia is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dementia is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dementia. If not, see <http://www.gnu.org/licenses/>.

#ifndef NODETEXTEDIT_HPP
#define NODETEXTEDIT_HPP

#include <QGraphicsTextItem>

class Node;
class QKeyEvent;

class NodeTextEdit : public QGraphicsTextItem
{
public:

    NodeTextEdit(Node * parent);

    float maxHeight() const;
    void setMaxHeight(float maxHeight);

    float maxWidth() const;
    void setMaxWidth(float maxWidth);

    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget) override;

protected:

    virtual void keyPressEvent(QKeyEvent * event) override;

private:

    float m_maxHeight = 0;

    float m_maxWidth = 0;
};

#endif // NODETEXTEDIT_HPP
