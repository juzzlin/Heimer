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

#include "edge_text_edit.hpp"

#include "constants.hpp"
#include "edge.hpp"

EdgeTextEdit::EdgeTextEdit(Edge * parentItem)
  : TextEdit(parentItem)
  , m_sizeAnimation(this, "opacity")
{
    setAcceptHoverEvents(true);

    m_sizeAnimation.setDuration(Constants::Edge::TEXT_EDIT_ANIMATION_DURATION);

    QGraphicsItem::setVisible(false);
    setOpacity(0);

    m_visibilityTimer.setSingleShot(true);
    m_visibilityTimer.setInterval(Constants::Edge::TEXT_EDIT_DURATION);

    connect(&m_visibilityTimer, &QTimer::timeout, [=] {
        setVisible(false);
    });
}

void EdgeTextEdit::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
    m_visibilityTimer.stop();

    setVisible(true);

    TextEdit::hoverEnterEvent(event);
}

void EdgeTextEdit::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
    m_visibilityTimer.start();

    TextEdit::hoverLeaveEvent(event);
}

void EdgeTextEdit::setVisible(bool visible)
{
    if (visible) {
        QGraphicsItem::setVisible(true);
        m_sizeAnimation.setStartValue(opacity());
        m_sizeAnimation.setEndValue(1.0);
        m_sizeAnimation.stop();
        m_sizeAnimation.start();
    } else if (text().isEmpty()) {
        m_sizeAnimation.setStartValue(opacity());
        m_sizeAnimation.setEndValue(0.0);
        m_sizeAnimation.stop();
        m_sizeAnimation.start();
    }
}
