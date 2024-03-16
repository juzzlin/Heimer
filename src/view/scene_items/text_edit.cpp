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

#include "text_edit.hpp"

#include "../../common/test_mode.hpp"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextOption>

namespace SceneItems {

TextEdit::TextEdit(QGraphicsItem * parentItem)
  : QGraphicsTextItem(parentItem)
  , m_unselectedFormat(textCursor().charFormat())
{
    if (!TestMode::enabled()) {
        setTextInteractionFlags(Qt::TextEditorInteraction);
        setDefaultTextColor({ 0, 0, 0 });
    } else {
        TestMode::logDisabledCode("TextEdit initialization");
    }
}

bool TextEdit::event(QEvent * event)
{
    // We need to add special handling for tab key as it won't trigger our keyPressEvent() by default.
    if (const auto keyEvent = dynamic_cast<QKeyEvent *>(event)) {
        if (keyEvent->key() == Qt::Key_Tab) {
            keyPressEvent(keyEvent);
            return true;
        }
    }

    return QGraphicsTextItem::event(event);
}

void TextEdit::keyPressEvent(QKeyEvent * event)
{
    // Don't mix the global undo and text edit's internal undo
    if (!event->matches(QKeySequence::Undo)) {
        QGraphicsTextItem::keyPressEvent(event);
        const auto oldText = m_text;
        const auto newText = toPlainText();
        if (oldText != newText) {
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

void TextEdit::contextMenuEvent(QGraphicsSceneContextMenuEvent * event)
{
    // Prevents the system context menu from opening.

    event->ignore();
}

QString TextEdit::text() const
{
    return m_text;
}

void TextEdit::setText(const QString & text)
{
    if (m_text != text) {
        m_text = text;
        if (!TestMode::enabled()) {
            setPlainText(text);
        } else {
            TestMode::logDisabledCode("Set TextEdit plain text");
        }
    }
}

void TextEdit::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    // Remove the HasFocus style state, to prevent the dotted line from being drawn.
    auto style = const_cast<QStyleOptionGraphicsItem *>(option);
    style->state &= ~QStyle::State_HasFocus;

    painter->fillRect(option->rect, m_backgroundColor);
    QGraphicsTextItem::paint(painter, style, widget);
}

QColor TextEdit::backgroundColor() const
{
    return m_backgroundColor;
}

void TextEdit::setBackgroundColor(const QColor & backgroundColor)
{
    m_backgroundColor = backgroundColor;

    update();
}

void TextEdit::selectText(const QString & text)
{
    unselectText();
    auto cursor(textCursor());
    cursor.clearSelection();
    if (!text.isEmpty()) {
        if (const auto index = static_cast<int>(this->text().toLower().indexOf(text.toLower())); index >= 0) {
            // Customize the text selection color
            auto format = cursor.charFormat();
            format.setForeground(Qt::white);
            format.setBackground(Qt::blue);
            cursor.setPosition(index);
            cursor.movePosition(QTextCursor::MoveOperation::Right, QTextCursor::MoveMode::KeepAnchor, static_cast<int>(text.length()));
            cursor.setCharFormat(format);
        }
    }
    cursor.clearSelection();
    setTextCursor(cursor);
}

void TextEdit::unselectText()
{
    auto cursor = textCursor();
    cursor.setPosition(0);
    cursor.movePosition(QTextCursor::MoveOperation::Right, QTextCursor::MoveMode::KeepAnchor, static_cast<int>(this->text().length()));
    cursor.setCharFormat(m_unselectedFormat);
}

void TextEdit::setTextSize(int textSize)
{
    m_textSize = textSize;
    if (!TestMode::enabled()) {
        auto && currentFont = font();
        currentFont.setPointSize(textSize);
        setFont(currentFont);
        update();
    } else {
        TestMode::logDisabledCode("TextEdit::setTextSize");
    }
}

TextEdit::~TextEdit() = default;

} // namespace SceneItems
