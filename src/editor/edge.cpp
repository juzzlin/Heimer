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

#include <QBrush>
#include <QGraphicsEllipseItem>
#include <QPen>

Edge::Edge(Node & sourceNode, Node & targetNode)
    : m_sourceNode(&sourceNode)
    , m_targetNode(&targetNode)
    , m_sourceDot(new EdgeDot(this))
    , m_targetDot(new EdgeDot(this))
    , m_sourceDotSizeAnimation(m_sourceDot, "scale")
    , m_targetDotSizeAnimation(m_targetDot, "scale")
{
    setGraphicsEffect(GraphicsFactory::createDropShadowEffect());

    setZValue(static_cast<int>(Layers::Edge));

    initDots();
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

void Edge::setSourceNode(Node & sourceNode)
{
    m_sourceNode = &sourceNode;
}

void Edge::setTargetNode(Node & targetNode)
{
    m_targetNode = &targetNode;
}

void Edge::updateDots(const std::pair<QPointF, QPointF> & nearestPoints)
{
    if (m_sourceDot->pos() != nearestPoints.first)
    {
        m_sourceDot->setPos(nearestPoints.first);

        // Re-parent to source node due to Z-ordering issues
        m_sourceDot->setParentItem(m_sourceNode);

        m_sourceDotSizeAnimation.stop();
        m_sourceDotSizeAnimation.start();
    }

    if (m_targetDot->pos() != nearestPoints.second)
    {
        m_targetDot->setPos(nearestPoints.second);

        // Re-parent to target node due to Z-ordering issues
        m_targetDot->setParentItem(m_targetNode);

        m_targetDotSizeAnimation.stop();
        m_targetDotSizeAnimation.start();
    }
}

Node & Edge::targetNode() const
{
    return *m_targetNode;
}

Node & Edge::sourceNode() const
{
    return *m_sourceNode;
}

void Edge::updateLine()
{
    const auto nearestPoints = Node::getNearestEdgePoints(*m_sourceNode, *m_targetNode);
    setLine(QLineF(nearestPoints.first + m_sourceNode->pos(), nearestPoints.second + m_targetNode->pos()));
    updateDots(nearestPoints);
}
