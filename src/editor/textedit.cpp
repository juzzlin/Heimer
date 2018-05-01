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

#include "textedit.hpp"

#include <QKeyEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QTextDocument>
#include <QTextOption>

TextEdit::TextEdit(QGraphicsItem * parentItem)
    : QGraphicsTextItem(parentItem)
{
    setTextInteractionFlags(Qt::TextEditorInteraction);
}

float TextEdit::maxHeight() const
{
    return m_maxHeight;
}

void TextEdit::setMaxHeight(float maxHeight)
{
    m_maxHeight = maxHeight;
}

float TextEdit::maxWidth() const
{
    return m_maxWidth;
}

void TextEdit::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    QStyleOptionGraphicsItem * style = const_cast<QStyleOptionGraphicsItem *>(option);

    // Remove the HasFocus style state, to prevent the dotted line from being drawn.
    style->state &= ~QStyle::State_HasFocus;

    painter->fillRect(option->rect, QColor(192, 192, 192, 64));
    QGraphicsTextItem::paint(painter, style, widget);
}

void TextEdit::setMaxWidth(float maxWidth)
{
    m_maxWidth = maxWidth;
}

TextEdit::~TextEdit()
{
}
