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

#include "../../common/utils.hpp"
#include "graphics_factory.hpp"
#include "layers.hpp"
#include "node.hpp"

#include <QConicalGradient>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPainter>
#include <QPainterPath>
#include <QPen>

namespace SceneItems {

NodeHandle::NodeHandle(NodeR parentNode, NodeHandle::Role role, int radius)
  : m_parentNode(parentNode)
  , m_role(role)
  , m_radius(radius)
{
    setAcceptHoverEvents(true);

    // Keep the size constant regardless of the zoom level
    setFlag(QGraphicsItem::ItemIgnoresTransformations);

    setAnimationDuration(m_handleAnimationDuration);
    setAnimationEasingCurve(QEasingCurve::OutCubic);

    m_visibilityTimer.setSingleShot(true);
    m_visibilityTimer.setInterval(m_handleVisibilityDuration);

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

//! Room for the shadow
static const double shadowMargin = 3;

QRectF NodeHandle::boundingRect() const
{
    const auto extent = m_radius + shadowMargin;
    return { -extent, -extent, extent * 2, extent * 2 };
}

QPainterPath NodeHandle::shape() const
{
    QPainterPath path;
    path.addEllipse(QPointF {}, m_radius, m_radius);
    return path;
}

void NodeHandle::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
    setVisible(true);

    m_hovered = true;
    update();

    m_visibilityTimer.stop();

    QGraphicsItem::hoverEnterEvent(event);
}

void NodeHandle::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
    m_visibilityTimer.start();

    m_hovered = false;
    update();

    QGraphicsItem::hoverLeaveEvent(event);
}

void NodeHandle::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{
    m_visibilityTimer.stop();

    QGraphicsItem::hoverMoveEvent(event);
}

void NodeHandle::retranslate()
{
    setToolTip(getToolTipTextByRole(m_role));
}

void NodeHandle::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    drawBackground(*painter);

    switch (role()) {
    case Role::NodeColor:
        drawNodeColorIcon(*painter);
        break;
    case Role::ConnectOrCreate:
        drawConnectOrCreateIcon(*painter);
        break;
    case Role::Move:
        drawMoveIcon(*painter);
        break;
    case Role::TextColor:
        drawTextColorIcon(*painter);
        break;
    }

    painter->restore();
}

bool NodeHandle::isOnDarkBackground() const
{
    if (scene() && !scene()->views().isEmpty()) {
        return !Utils::isColorBright(scene()->views().first()->backgroundBrush().color());
    }
    return false;
}

QPen NodeHandle::iconPen() const
{
    QPen pen { isOnDarkBackground() ? QColor { 0xe3, 0xe5, 0xe8 } : QColor { 0x3c, 0x40, 0x48 } };
    pen.setWidthF(1.75);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    return pen;
}

void NodeHandle::drawBackground(QPainter & painter) const
{
    const auto dark = isOnDarkBackground();

    // Soft shadow
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor { 0, 0, 0, dark ? 70 : 18 });
    painter.drawEllipse(QPointF { 0, 1.5 }, m_radius + 1.5, m_radius + 1.5);
    painter.setBrush(QColor { 0, 0, 0, dark ? 90 : 28 });
    painter.drawEllipse(QPointF { 0, 1 }, m_radius + 0.5, m_radius + 0.5);

    QColor surface = dark ? QColor { 0x2b, 0x2d, 0x31 } : QColor { 0xff, 0xff, 0xff };
    if (m_hovered) {
        surface = dark ? QColor { 0x3d, 0x41, 0x48 } : QColor { 0xe8, 0xee, 0xf7 };
    }
    painter.setBrush(surface);
    painter.setPen(GraphicsFactory::createSubtleOutlinePen(surface));
    painter.drawEllipse(QPointF {}, m_radius - 0.5, m_radius - 0.5);
}

void NodeHandle::drawConnectOrCreateIcon(QPainter & painter) const
{
    painter.setPen(iconPen());
    const auto d = m_radius * 0.42;
    painter.drawLine(QPointF { -d, 0 }, QPointF { d, 0 });
    painter.drawLine(QPointF { 0, -d }, QPointF { 0, d });
}

void NodeHandle::drawMoveIcon(QPainter & painter) const
{
    painter.setPen(iconPen());
    const auto d = m_radius * 0.5;
    const auto a = m_radius * 0.17;
    painter.drawLine(QPointF { -d, 0 }, QPointF { d, 0 });
    painter.drawLine(QPointF { 0, -d }, QPointF { 0, d });
    // Arrow heads at all four ends
    for (int i = 0; i < 4; i++) {
        painter.save();
        painter.rotate(90 * i);
        painter.drawPolyline(QPolygonF { { QPointF { d - a, -a }, QPointF { d, 0 }, QPointF { d - a, a } } });
        painter.restore();
    }
}

void NodeHandle::drawNodeColorIcon(QPainter & painter) const
{
    // A rainbow ring around the current node color
    QConicalGradient gradient { QPointF {}, 90 };
    const int hueSteps = 6;
    for (int i = 0; i <= hueSteps; i++) {
        gradient.setColorAt(static_cast<double>(i) / hueSteps, QColor::fromHsv((360 / hueSteps * i) % 360, 200, 240));
    }
    QPen ringPen { QBrush { gradient }, 2.5 };
    painter.setPen(ringPen);
    painter.setBrush(Qt::NoBrush);
    const auto ringRadius = m_radius * 0.56;
    painter.drawEllipse(QPointF {}, ringRadius, ringRadius);

    const auto color = m_parentNode.color();
    painter.setPen(GraphicsFactory::createSubtleOutlinePen(color));
    painter.setBrush(color);
    const auto dotRadius = m_radius * 0.32;
    painter.drawEllipse(QPointF {}, dotRadius, dotRadius);
}

void NodeHandle::drawTextColorIcon(QPainter & painter) const
{
    // The letter "A" and a bar in the current text color
    QFont font;
    font.setPixelSize(static_cast<int>(m_radius * 0.95));
    font.setBold(true);
    painter.setFont(font);
    painter.setPen(iconPen());
    painter.drawText(QRectF { -m_radius * 1.0, -m_radius * 0.82, m_radius * 2.0, m_radius * 1.15 }, Qt::AlignCenter, tr("A"));

    const auto color = m_parentNode.textColor();
    painter.setPen(GraphicsFactory::createSubtleOutlinePen(color));
    painter.setBrush(color);
    painter.drawRoundedRect(QRectF { -m_radius * 0.45, m_radius * 0.36, m_radius * 0.9, m_radius * 0.24 }, 1.5, 1.5);
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

            appearWithAnimation();

            m_visibilityTimer.start();
        }
    } else {
        if (m_visible) {
            m_visible = false;

            disappearWithAnimation();
        }
    }
}

NodeHandle::~NodeHandle() = default;

} // namespace SceneItems
