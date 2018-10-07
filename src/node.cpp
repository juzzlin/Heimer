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

#include "node.hpp"

#include "edge.hpp"
#include "graphicsfactory.hpp"
#include "layers.hpp"
#include "nodehandle.hpp"
#include "textedit.hpp"

#include "mclogger.hh"

#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QPen>
#include <QVector2D>

#include <algorithm>
#include <cmath>

Node::Node()
    : m_textEdit(new TextEdit(this))
{
    setAcceptHoverEvents(true);

    setSize(QSize(m_minWidth, m_minHeight));

    setZValue(static_cast<int>(Layers::Node));

    createEdgePoints();

    createHandles();

    initTextField();

    setGraphicsEffect(GraphicsFactory::createDropShadowEffect());

    connect(m_textEdit, &TextEdit::textChanged, [=] (const QString & text) {

        setText(text);

        if (isTextUnderflowOrOverflow())
        {
            adjustSize();
        }
    });

    connect(m_textEdit, &TextEdit::undoPointRequested, this, &Node::undoPointRequested);
}

Node::Node(const Node & other)
    : Node()
{
    setColor(other.color());

    setIndex(other.index());

    setLocation(other.location());

    setSize(other.size());

    setText(other.text());

    setTextColor(other.textColor());
}

void Node::addGraphicsEdge(Edge & edge)
{
    m_graphicsEdges.push_back(&edge);
}

void Node::removeGraphicsEdge(Edge & edge)
{
    auto iter = std::find(m_graphicsEdges.begin(), m_graphicsEdges.end(), &edge);
    if (iter != m_graphicsEdges.end())
    {
        m_graphicsEdges.erase(iter);
    }
}

void Node::adjustSize()
{
    prepareGeometryChange();

    setSize(QSize(
        std::max(m_minWidth, static_cast<float>(m_textEdit->boundingRect().width() + m_margin * 2)),
        std::max(m_minHeight, static_cast<float>(m_textEdit->boundingRect().height() + m_margin * 2))));

    initTextField();

    createHandles();

    createEdgePoints();

    updateEdgeLines();

    update();
}

QRectF Node::boundingRect() const
{
    // Take children into account
    QRectF nodeBox{-size().width() / 2, -size().height() / 2, size().width(), size().height()};
    for (auto && handle : m_handles) {
        auto handleBox = handle->boundingRect();
        handleBox.translate(handle->pos());
        nodeBox = nodeBox.united(handleBox);
    }
    return nodeBox;
}

EdgePtr Node::createAndAddGraphicsEdge(NodePtr targetNode)
{
    auto edge = std::make_shared<Edge>(*this, *targetNode);
    edge->updateLine();
    m_graphicsEdges.push_back(edge.get());
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
    // Delete old handles
    for (auto handle : m_handles)
    {
        handle->setParentItem(nullptr);
        delete handle;
    }
    m_handles.clear();

    auto addHandle = new NodeHandle(*this, NodeHandle::Role::Add, m_handleRadius);
    addHandle->setParentItem(this);
    addHandle->setPos({0, size().height() * 0.5f + m_handleRadius * 0.5f});
    m_handles.push_back(addHandle);

    auto colorHandle = new NodeHandle(*this, NodeHandle::Role::Color, m_handleRadius);
    colorHandle->setParentItem(this);
    colorHandle->setPos({size().width() * 0.5f, size().height() * 0.5f});
    m_handles.push_back(colorHandle);

    auto textColorHandle = new NodeHandle(*this, NodeHandle::Role::TextColor, m_handleRadius);
    textColorHandle->setParentItem(this);
    textColorHandle->setPos({size().width() * 0.5f + m_handleRadius * 0.5f, -size().height() * 0.5f});
    m_handles.push_back(textColorHandle);
}

std::pair<QPointF, QPointF> Node::getNearestEdgePoints(const Node & node1, const Node & node2)
{
    float bestDistance = std::numeric_limits<float>::max();
    std::pair<QPointF, QPointF> bestPair = {QPointF(), QPointF()};

    // This is O(n^2) but fine as there are not many points
    for (const auto & point1 : node1.m_edgePoints)
    {
        for (const auto & point2 : node2.m_edgePoints)
        {
            const float distance = std::pow(node1.pos().x() + point1.x() - node2.pos().x() - point2.x(), 2) +
                std::pow(node1.pos().y() + point1.y() - node2.pos().y() - point2.y(), 2);

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

void Node::initTextField()
{
#ifndef HEIMER_UNIT_TEST
    m_textEdit->setTextWidth(size().width() - m_margin * 2);
    m_textEdit->setPos(-m_textEdit->textWidth() * 0.5f, -size().height() * 0.5f + m_margin);
    m_textEdit->setMaxHeight(size().height() - m_margin * 4);
    m_textEdit->setMaxWidth(size().width() - m_margin * 2);
#endif
}

bool Node::isTextUnderflowOrOverflow() const
{
  const float tolerance = 0.001f;
  return m_textEdit->boundingRect().height() > m_textEdit->maxHeight() + tolerance ||
      m_textEdit->boundingRect().width() > m_textEdit->maxWidth() + tolerance ||
      m_textEdit->boundingRect().height() < m_textEdit->maxHeight() - tolerance ||
      m_textEdit->boundingRect().width() < m_textEdit->maxWidth() - tolerance;
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
        QBrush(color()));

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

    updateEdgeLines();
}

void Node::setText(const QString & text)
{
    if (text != this->text())
    {
        NodeBase::setText(text);
        m_textEdit->setPlainText(text);

        if (isTextUnderflowOrOverflow())
        {
            adjustSize();
        }
    }
}

void Node::setTextColor(const QColor & color)
{
    NodeBase::setTextColor(color);
#ifndef HEIMER_UNIT_TEST
    m_textEdit->setDefaultTextColor(color);
    m_textEdit->update();
#endif
}

QString Node::text() const
{
    return m_textEdit->toPlainText();
}

void Node::updateEdgeLines()
{
    for (auto edge : m_graphicsEdges)
    {
        edge->updateLine();
    }
}

Node::~Node()
{
    MCLogger().debug() << "Node deleted: " << index();
}
