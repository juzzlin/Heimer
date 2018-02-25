// This file is part of Dementia.
// Copyright (C) 2018 Jussi Lind <jussi.lind@iki.fi>
//
// Dementia is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dementia is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dementia. If not, see <http://www.gnu.org/licenses/>.

#include "node.hpp"

#include "edge.hpp"
#include "graphicsfactory.hpp"
#include "nodehandle.hpp"

#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QPen>
#include <QVector2D>

#include <cmath>

Node::Node()
{
    setAcceptHoverEvents(true);

    setSize(QSize(200, 150));

    setZValue(10);

    createEdgePoints();

    createHandles();

    setGraphicsEffect(GraphicsFactory::createDropShadowEffect());
}

Node::Node(const Node & other)
    : QGraphicsItem()
    , NodeBase()
{
    setAcceptHoverEvents(true);

    setIndex(other.index());

    setLocation(other.location());

    setSize(other.size());

    setZValue(10);

    createEdgePoints();

    createHandles();
}

void Node::addEdge(EdgePtr edge)
{
    m_edges.push_back(edge);
}

QRectF Node::boundingRect() const
{
    // The bounding rect has to cover also child nodes
    const int margin = m_handleRadius;
    return QRectF(-size().width() / 2 - margin, -size().height() / 2 - margin, size().width() + margin * 2, size().height() + margin * 2);
}

EdgePtr Node::createAndAddEdge(NodePtr targetNode)
{
    auto edge = EdgePtr(new Edge(*this, *targetNode));
    edge->updateLine();
    m_edges.push_back(edge);
    return edge;
}

void Node::createEdgePoints()
{
    const float w2 = size().width() * 0.5f;
    const float h2 = size().height() * 0.5f;
    const float bias = 0.1f;

    m_edgePoints = {
        {-w2, h2},
        {0, h2 + bias},
        {w2, h2},
        {w2 + bias, 0},
        {w2, -h2},
        {0, -h2 - bias},
        {-w2, -h2},
        {-w2 - bias, 0}
    };
}

void Node::createHandles()
{
    const std::vector<QPointF> positions = {
        {0, size().height() * 0.5f}
    };

    for (auto position : positions)
    {
        auto handle = new NodeHandle(m_handleRadius);
        handle->setParentItem(this);
        handle->setPos(position);
        m_handles.push_back(handle);
    }
}

std::pair<QPointF, QPointF> Node::getNearestEdgePoints(Node & node1, Node & node2)
{
    float bestDistance = std::numeric_limits<float>::max();
    std::pair<QPointF, QPointF> bestPair = {QPointF(), QPointF()};

    // This is O(n^2) but fine as there are not many points
    for (auto point1 : node1.m_edgePoints)
    {
        point1 = node1.pos() + point1;
        for (auto point2 : node2.m_edgePoints)
        {
            point2 = node2.pos() + point2;
            const float distance = std::pow(point1.x() - point2.x(), 2) + std::pow(point1.y() - point2.y(), 2);
            if (distance < bestDistance)
            {
                bestDistance = distance;
                bestPair = {point1, point2};
            }
        }
    }

    return bestPair;
}

void Node::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
    setHandlesVisible(true);

    QGraphicsItem::hoverEnterEvent(event);
}

void Node::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
    setHandlesVisible(false);

    QGraphicsItem::hoverLeaveEvent(event);
}

void Node::paint(QPainter * painter,
    const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);

    painter->save();

    // Background
    painter->fillRect(
        -size().width() / 2, -size().height() / 2,
        size().width(), size().height(),
        QBrush(QColor(255, 255, 255, 255)));

    painter->restore();
}

void Node::setHandlesVisible(bool visible)
{
    for (auto handle : m_handles)
    {
        handle->setVisible(visible);
    }
}

void Node::setLocation(QPointF newLocation)
{
    NodeBase::setLocation(newLocation);
    setPos(newLocation);

    for (auto edge : m_edges)
    {
        edge->updateLine();
    }
}
