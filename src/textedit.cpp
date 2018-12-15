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
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QTextDocument>
#include <QTextOption>

TextEdit::TextEdit(QGraphicsItem * parentItem)
    : QGraphicsTextItem(parentItem)
{
#ifndef HEIMER_UNIT_TEST
    setTextInteractionFlags(Qt::TextEditorInteraction);
    setDefaultTextColor({0, 0, 0});
#endif
}

void TextEdit::keyPressEvent(QKeyEvent * event)
{
    const auto prevText = m_text;

    // Don't mix the global undo and text edit's internal undo
    if (!event->matches(QKeySequence::Undo))
    {
        QGraphicsTextItem::keyPressEvent(event);

        const auto newText = toPlainText();
        if (prevText != newText)
        {
            m_text = newText;
            emit textChanged(newText);
        }
    }
}

void TextEdit::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    emit undoPointRequested();

    QGraphicsTextItem::mousePressEvent(event);
}

QString TextEdit::text() const
{
    return m_text;
}

void TextEdit::setText(const QString & text)
{
    m_text = text;
    setPlainText(text);
}

double TextEdit::maxHeight() const
{
    return m_maxHeight;
}

void TextEdit::setMaxHeight(double maxHeight)
{
    m_maxHeight = maxHeight;
}

double TextEdit::maxWidth() const
{
    return m_maxWidth;
}

void TextEdit::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    QStyleOptionGraphicsItem * style = const_cast<QStyleOptionGraphicsItem *>(option);

    // Remove the HasFocus style state, to prevent the dotted line from being drawn.
    style->state &= ~QStyle::State_HasFocus;

    painter->fillRect(option->rect, m_backgroundColor);
    QGraphicsTextItem::paint(painter, style, widget);
}

void TextEdit::focusInEvent(QFocusEvent * event)
{
    QGraphicsTextItem::focusInEvent(event);
}

void TextEdit::setBackgroundColor(const QColor & backgroundColor)
{
    m_backgroundColor = backgroundColor;

    update();
}

void TextEdit::setMaxWidth(double maxWidth)
{
    m_maxWidth = maxWidth;
}

void TextEdit::setTextSize(int textSize)
{
    m_textSize = textSize;
#ifndef HEIMER_UNIT_TEST
    auto && currentFont = font();
    currentFont.setPointSize(textSize);
    setFont(currentFont);
    update();
#endif
}

TextEdit::~TextEdit()
{
}
