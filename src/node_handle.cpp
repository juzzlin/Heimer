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

#include "node_handle.hpp"
#include "node.hpp"

#include "constants.hpp"
#include "graphics_factory.hpp"
#include "layers.hpp"

#include <QPainter>
#include <QPen>

NodeHandle::NodeHandle(NodeR parentNode, NodeHandle::Role role, int radius)
  : m_parentNode(parentNode)
  , m_role(role)
  , m_radius(radius)
  , m_sizeAnimation(this, "scale")
  , m_opacityAnimation(this, "opacity")
  , m_size({ m_radius * 2, m_radius * 2 })
{
    setAcceptHoverEvents(true);

    m_sizeAnimation.setDuration(Constants::Node::HANDLE_ANIMATION_DURATION);
    m_opacityAnimation.setDuration(Constants::Node::HANDLE_ANIMATION_DURATION);

    m_visibilityTimer.setSingleShot(true);
    m_visibilityTimer.setInterval(Constants::Node::HANDLE_VISIBILITY_DURATION);

    connect(&m_visibilityTimer, &QTimer::timeout, this, [=] {
        setVisible(false);
    });

    QGraphicsItem::setVisible(false);

    setZValue(static_cast<int>(Layers::NodeHandle));

    setToolTip(getToolTipTextByRole(role));
}

QString NodeHandle::getToolTipTextByRole(NodeHandle::Role role) const
{
    switch (role) {
    case NodeHandle::Role::ConnectOrCreate:
        return tr("Drag to connect or create a child node");
    case NodeHandle::Role::Move:
        return tr("Move the node");
    case NodeHandle::Role::NodeColor:
        return tr("Select node color");
    case NodeHandle::Role::TextColor:
        return tr("Select text color");
    }
    return "";
}

QRectF NodeHandle::boundingRect() const
{
    const int margin = 1;
    return QRectF(-m_size.width() / 2 - margin, -m_size.height() / 2 - margin, m_size.width() + margin * 2, m_size.height() + margin * 2);
}

void NodeHandle::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
    setVisible(true);

    m_visibilityTimer.stop();

    QGraphicsItem::hoverEnterEvent(event);
}

void NodeHandle::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
    m_visibilityTimer.start();

    QGraphicsItem::hoverLeaveEvent(event);
}

void NodeHandle::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{
    m_visibilityTimer.stop();

    QGraphicsItem::hoverMoveEvent(event);
}

void NodeHandle::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)

    switch (role()) {
    case Role::NodeColor:
        drawColorHandle(*painter);
        break;
    case Role::ConnectOrCreate:
    case Role::Move:
        drawPixmapHandle(*painter);
        break;
    case Role::TextColor:
        drawTextColorHandle(*painter);
        break;
    }
}

QColor NodeHandle::calculateBackgroundColor() const
{
    return { (230 + m_parentNode.color().red()) / 2, (230 + m_parentNode.color().green()) / 2, (230 + m_parentNode.color().blue()) / 2 };
}

void NodeHandle::drawColorHandle(QPainter & painter)
{
    const std::vector<QColor> paletteColors = {
        { 255, 0, 255 },
        { 255, 0, 0 },
        { 255, 128, 0 },
        { 0, 0, 128 },
        { 0, 255, 0 },
        { 255, 255, 0 },
        { 255, 255, 255 },
        { 0, 255, 255 },
        { 0, 0, 255 }
    };

    painter.save();
    painter.setBrush(calculateBackgroundColor());
    painter.setPen(Qt::PenStyle::NoPen);
    painter.drawEllipse(-m_size.width() / 2, -m_size.height() / 2, m_size.width(), m_size.height());

    const size_t paletteSize = 3;
    const auto paletteWidth = static_cast<float>(m_size.width()) * 0.7f;
    const auto paletteColorWidth = paletteWidth / paletteSize;
    const auto paletteColorMarginW = paletteColorWidth * 0.1f;
    const auto paletteHeight = paletteWidth;
    const auto paletteColorHeight = paletteColorWidth;
    const auto paletteColorMarginH = paletteColorMarginW;
    for (size_t i = 0; i < paletteSize; i++) {
        for (size_t j = 0; j < paletteSize; j++) {
            const float x = -paletteWidth / 2 + paletteColorWidth * static_cast<float>(i);
            const float y = -paletteHeight / 2 + paletteColorHeight * static_cast<float>(j);
            painter.fillRect(QRectF { static_cast<qreal>(x + paletteColorMarginW),
                                      static_cast<qreal>(y + paletteColorMarginH),
                                      static_cast<qreal>(paletteColorWidth - paletteColorMarginW * 2),
                                      static_cast<qreal>(paletteColorHeight - paletteColorMarginH * 2) },
                             paletteColors[static_cast<size_t>(i + j * paletteSize) % paletteColors.size()]);
        }
    }

    painter.restore();
}

void NodeHandle::drawPixmapHandle(QPainter & painter)
{
    static const std::map<Role, QPixmap> pixmapMap = {
        { Role::ConnectOrCreate, QPixmap(":/add.png") },
        { Role::Move, QPixmap(":/drag.png") }
    };

    if (pixmapMap.count(role())) {
        painter.save();
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.drawPixmap(-m_size.width() / 2, -m_size.height() / 2, m_size.width(), m_size.height(), pixmapMap.at(role()));
        painter.restore();
    }
}

void NodeHandle::drawTextColorHandle(QPainter & painter)
{
    drawColorHandle(painter);

    painter.save();
    painter.setPen(Qt::black);
    painter.setOpacity(0.5);
    QFont font;
    font.setPixelSize(2 * m_size.height() / 3);
    font.setItalic(true);
    font.setBold(true);
    painter.setFont(font);
    painter.drawText(QRectF(0, -m_size.height() / 2, m_size.width() / 2, m_size.height() / 2), Qt::AlignCenter, tr("A"));
    painter.restore();
}

NodeR NodeHandle::parentNode() const
{
    return m_parentNode;
}

int NodeHandle::radius() const
{
    return m_radius;
}

NodeHandle::Role NodeHandle::role() const
{
    return m_role;
}

void NodeHandle::setVisible(bool visible)
{
    if (visible) {
        if (!m_visible && m_parentNode.index() != -1) {
            QGraphicsItem::setVisible(true);

            m_visible = true;
            m_sizeAnimation.setStartValue(0.0);
            m_sizeAnimation.setEndValue(1.0);
            m_sizeAnimation.stop();
            m_sizeAnimation.start();

            m_opacityAnimation.setStartValue(0.0);
            m_opacityAnimation.setEndValue(Constants::Node::HANDLE_OPACITY);
            m_opacityAnimation.stop();
            m_opacityAnimation.start();

            m_visibilityTimer.start();
        }
    } else {
        if (m_visible) {
            m_visible = false;
            m_sizeAnimation.setStartValue(scale());
            m_sizeAnimation.setEndValue(0.0);
            m_sizeAnimation.stop();
            m_sizeAnimation.start();

            m_opacityAnimation.setStartValue(opacity());
            m_opacityAnimation.setEndValue(0.0);
            m_opacityAnimation.stop();
            m_opacityAnimation.start();
        }
    }
}

NodeHandle::~NodeHandle() = default;
