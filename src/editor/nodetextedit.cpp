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

#include "nodetextedit.hpp"
#include "node.hpp"

#include <QKeyEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QTextDocument>
#include <QTextOption>

NodeTextEdit::NodeTextEdit(Node * parentItem)
    : QGraphicsTextItem(parentItem)
{
    setTextInteractionFlags(Qt::TextEditorInteraction);
}

void NodeTextEdit::keyPressEvent(QKeyEvent * event)
{
    QGraphicsTextItem::keyPressEvent(event);

    const float tolerance = 0.001f;
    if (boundingRect().height() > m_maxHeight + tolerance || boundingRect().width() > m_maxWidth + tolerance||
        boundingRect().height() < m_maxHeight - tolerance || boundingRect().width() < m_maxWidth - tolerance)
    {
        auto node = dynamic_cast<Node *>(parentItem());
        node->adjustSize();
    }
}

float NodeTextEdit::maxHeight() const
{
    return m_maxHeight;
}

void NodeTextEdit::setMaxHeight(float maxHeight)
{
    m_maxHeight = maxHeight;
}

float NodeTextEdit::maxWidth() const
{
    return m_maxWidth;
}

void NodeTextEdit::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    QStyleOptionGraphicsItem * style = const_cast<QStyleOptionGraphicsItem *>(option);

    // Remove the HasFocus style state, to prevent the dotted line from being drawn.
    style->state &= ~QStyle::State_HasFocus;

    painter->fillRect(option->rect, QColor(192, 192, 192, 64));
    QGraphicsTextItem::paint(painter, style, widget);
}

void NodeTextEdit::setMaxWidth(float maxWidth)
{
    m_maxWidth = maxWidth;
}
