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

#include "edge.hpp"
#include "edgedot.hpp"
#include "graphicsfactory.hpp"
#include "layers.hpp"
#include "node.hpp"
#include "edgetextedit.hpp"

#include <QBrush>
#include <QGraphicsEllipseItem>
#include <QPen>
#include <QTimer>

#include <cassert>

Edge::Edge(Node & sourceNode, Node & targetNode)
    : EdgeBase(sourceNode, targetNode)
    , m_sourceDot(new EdgeDot(this))
    , m_targetDot(new EdgeDot(this))
    , m_label(new EdgeTextEdit(this))
    , m_sourceDotSizeAnimation(m_sourceDot, "scale")
    , m_targetDotSizeAnimation(m_targetDot, "scale")
{
    setAcceptHoverEvents(true);

    setGraphicsEffect(GraphicsFactory::createDropShadowEffect());

    setZValue(static_cast<int>(Layers::Edge));

    initDots();

    m_label->setZValue(static_cast<int>(Layers::EdgeLabel));
    m_label->setBackgroundColor(QColor(0xff, 0xee, 0xaa));

    connect(m_label, &TextEdit::textChanged, [=] (const QString & text) {
        updateLabel();
        EdgeBase::setText(text);
    });

    m_labelVisibilityTimer.setSingleShot(true);
    m_labelVisibilityTimer.setInterval(2000);

    connect(&m_labelVisibilityTimer, &QTimer::timeout, [=] () {
        setLabelVisible(false);
    });
}

void Edge::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
    m_labelVisibilityTimer.stop();

    setLabelVisible(true);

    QGraphicsItem::hoverEnterEvent(event);
}

void Edge::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
    m_labelVisibilityTimer.start();

    QGraphicsItem::hoverLeaveEvent(event);
}

void Edge::initDots()
{
    const QColor color(255, 0, 0, 192);
    m_sourceDot->setPen(QPen(color));
    m_sourceDot->setBrush(QBrush(color));

    m_targetDot->setPen(QPen(color));
    m_targetDot->setBrush(QBrush(color));

    const int duration = 2000;

    m_sourceDotSizeAnimation.setDuration(duration);
    m_sourceDotSizeAnimation.setStartValue(1.0f);
    m_sourceDotSizeAnimation.setEndValue(0.0f);

    const QRectF rect(-m_dotRadius, -m_dotRadius, m_dotRadius * 2, m_dotRadius * 2);
    m_sourceDot->setRect(rect);

    m_targetDotSizeAnimation.setDuration(duration);
    m_targetDotSizeAnimation.setStartValue(1.0f);
    m_targetDotSizeAnimation.setEndValue(0.0f);

    m_targetDot->setRect(rect);
}

void Edge::setLabelVisible(bool visible)
{
    m_label->setVisible(visible);
}

void Edge::setText(const QString & text)
{
    EdgeBase::setText(text);
    m_label->setPlainText(text);

    setLabelVisible(!text.isEmpty());
}

Node & Edge::sourceNode() const
{
    auto node = dynamic_cast<Node *>(&sourceNodeBase());
    assert(node);
    return *node;
}

Node & Edge::targetNode() const
{
    auto node = dynamic_cast<Node *>(&targetNodeBase());
    assert(node);
    return *node;
}

void Edge::updateDots(const std::pair<QPointF, QPointF> & nearestPoints)
{
    if (m_sourceDot->pos() != nearestPoints.first)
    {
        m_sourceDot->setPos(nearestPoints.first);

        // Re-parent to source node due to Z-ordering issues
        m_sourceDot->setParentItem(&sourceNode());

        m_sourceDotSizeAnimation.stop();
        m_sourceDotSizeAnimation.start();
    }

    if (m_targetDot->pos() != nearestPoints.second)
    {
        m_targetDot->setPos(nearestPoints.second);

        // Re-parent to target node due to Z-ordering issues
        m_targetDot->setParentItem(&targetNode());

        m_targetDotSizeAnimation.stop();
        m_targetDotSizeAnimation.start();
    }
}

void Edge::updateLabel()
{
    m_label->setPos(line().center() - 0.5f * QPointF(m_label->boundingRect().width(), m_label->boundingRect().height()));
}

void Edge::updateLine()
{
    const auto nearestPoints = Node::getNearestEdgePoints(sourceNode(), targetNode());
    setLine(QLineF(nearestPoints.first + sourceNode().pos(), nearestPoints.second + targetNode().pos()));
    updateDots(nearestPoints);
    updateLabel();
}
