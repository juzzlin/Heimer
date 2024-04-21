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

#include "edge.hpp"
#include "graphics_factory.hpp"

#include <QPainter>
#include <QStyleOptionGraphicsItem>

namespace SceneItems {

EdgeTextEdit::EdgeTextEdit(EdgeP parentItem)
  : TextEdit(parentItem)
  , m_edge(parentItem)
  , m_opacityAnimation(this, "opacity")
{
    setAcceptHoverEvents(true);

    initializeVisibilityTimer();

    m_opacityAnimation.setDuration(150);

    QGraphicsItem::setVisible(false);

    setOpacity(0);
}

void EdgeTextEdit::initializeVisibilityTimer()
{
    m_visibilityTimer.setSingleShot(true);
    m_visibilityTimer.setInterval(1000);

    connect(&m_visibilityTimer, &QTimer::timeout, this, &EdgeTextEdit::visibilityTimeout);
}

EdgeP EdgeTextEdit::edge() const
{
    return m_edge;
}

void EdgeTextEdit::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
    m_visibilityTimer.stop();

    TextEdit::hoverEnterEvent(event);

    emit hoverEntered();
}

void EdgeTextEdit::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
    m_visibilityTimer.start();

    TextEdit::hoverLeaveEvent(event);
}

void EdgeTextEdit::paintOutline(QPainter * painter, const QStyleOptionGraphicsItem * option)
{
    QPainterPath path;
    path.addRect(option->rect);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->strokePath(path, GraphicsFactory::createOutlinePen(backgroundColor()));
}

void EdgeTextEdit::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    TextEdit::paint(painter, option, widget);

    paintOutline(painter, option);
}

void EdgeTextEdit::hideAnimated()
{
    m_opacityAnimation.setStartValue(opacity());
    m_opacityAnimation.setEndValue(0.0);
    m_opacityAnimation.stop();
    m_opacityAnimation.start();
}

void EdgeTextEdit::showAnimated()
{
    QGraphicsItem::setVisible(true);

    m_opacityAnimation.setStartValue(opacity());
    m_opacityAnimation.setEndValue(1.0);
    m_opacityAnimation.stop();
    m_opacityAnimation.start();
}

void EdgeTextEdit::setVisible(bool visible)
{
    if (visible) {
        showAnimated();
    } else {
        hideAnimated();
    }
}

void EdgeTextEdit::updateDueToLostFocus()
{
    m_visibilityTimer.start();
}

} // namespace SceneItems
