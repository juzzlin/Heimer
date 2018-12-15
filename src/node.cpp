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

#include "constants.hpp"
#include "edge.hpp"
#include "graphicsfactory.hpp"
#include "layers.hpp"
#include "nodehandle.hpp"
#include "textedit.hpp"

#include "simple_logger.hpp"

#include <QGraphicsDropShadowEffect>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>
#include <QPen>
#include <QVector2D>

#include <algorithm>
#include <cmath>

Node::Node()
    : m_textEdit(new TextEdit(this))
{
    setAcceptHoverEvents(true);

    setSize(QSize(Constants::Node::MIN_WIDTH, Constants::Node::MIN_HEIGHT));

    setZValue(static_cast<int>(Layers::Node));

    createEdgePoints();

    createHandles();

    initTextField();

    setSelected(false);

    connect(m_textEdit, &TextEdit::textChanged, [=] (const QString & text) {

        setText(text);

        if (isTextUnderflowOrOverflow())
        {
            adjustSize();
        }
    });

    connect(m_textEdit, &TextEdit::undoPointRequested, this, &Node::undoPointRequested);

    m_handleVisibilityTimer.setSingleShot(true);
    m_handleVisibilityTimer.setInterval(2000);

    connect(&m_handleVisibilityTimer, &QTimer::timeout, [=] () {
        setHandlesVisible(false, false);
    });
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

    setTextSize(other.textSize());
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
        std::max(Constants::Node::MIN_WIDTH, static_cast<int>(m_textEdit->boundingRect().width() + Constants::Node::MARGIN * 2)),
        std::max(Constants::Node::MIN_HEIGHT, static_cast<int>(m_textEdit->boundingRect().height() + Constants::Node::MARGIN * 2))));

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
    const double w2 = size().width() * 0.5;
    const double h2 = size().height() * 0.5;
    const double bias = 0.1;

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

    auto addHandle = new NodeHandle(*this, NodeHandle::Role::Add, Constants::Node::HANDLE_RADIUS);
    addHandle->setParentItem(this);
    addHandle->setPos({0, size().height() * 0.5});
    m_handles.push_back(addHandle);

    auto colorHandle = new NodeHandle(*this, NodeHandle::Role::Color, Constants::Node::HANDLE_RADIUS_SMALL);
    colorHandle->setParentItem(this);
    colorHandle->setPos({size().width() * 0.5, size().height() * 0.5 - Constants::Node::HANDLE_RADIUS_SMALL * 0.5});
    m_handles.push_back(colorHandle);

    auto textColorHandle = new NodeHandle(*this, NodeHandle::Role::TextColor, Constants::Node::HANDLE_RADIUS_SMALL);
    textColorHandle->setParentItem(this);
    textColorHandle->setPos({size().width() * 0.5, -size().height() * 0.5 + Constants::Node::HANDLE_RADIUS_SMALL * 0.5});
    m_handles.push_back(textColorHandle);
}

std::pair<QPointF, QPointF> Node::getNearestEdgePoints(const Node & node1, const Node & node2)
{
    double bestDistance = std::numeric_limits<double>::max();
    std::pair<QPointF, QPointF> bestPair = {QPointF(), QPointF()};

    // This is O(n^2) but fine as there are not many points
    for (const auto & point1 : node1.m_edgePoints)
    {
        for (const auto & point2 : node2.m_edgePoints)
        {
            const auto distance = std::pow(node1.pos().x() + point1.x() - node2.pos().x() - point2.x(), 2) +
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
    if (index() != -1) // Prevent right-click on the drag node
    {
        m_currentMousePos = event->pos();
        m_mouseIn = true;

        checkHandleVisibility(event->pos());

        QGraphicsItem::hoverEnterEvent(event);
    }
}

void Node::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
    if (index() != -1) // Prevent right-click on the drag node
    {
        m_mouseIn = false;

        setHandlesVisible(false);

        if (event) // EditorView may call this with a null event
        {
            QGraphicsItem::hoverLeaveEvent(event);
        }
    }
}

void Node::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{
    if (index() != -1) // Prevent right-click on the drag node
    {
        m_currentMousePos = event->pos();

        checkHandleVisibility(event->pos());

        QGraphicsItem::hoverMoveEvent(event);
    }
}

void Node::checkHandleVisibility(QPointF pos)
{
    // Bounding box without children
    const QRectF bbox{-size().width() / 2, -size().height() / 2, size().width(), size().height()};
    if (bbox.contains(pos))
    {
        if (hitsHandle(pos))
        {
            setHandlesVisible(true, false);
        }
        else
        {
            setHandlesVisible(true);
        }

        m_handleVisibilityTimer.start();
    }
}

NodeHandle * Node::hitsHandle(QPointF pos)
{
    for (auto handle : m_handles)
    {
        if (handle->contains(pos))
        {
            return handle;
        }
    }

    return nullptr;
}

void Node::initTextField()
{
#ifndef HEIMER_UNIT_TEST
    m_textEdit->setTextWidth(size().width() - Constants::Node::MARGIN * 2);
    m_textEdit->setPos(-m_textEdit->textWidth() * 0.5, -size().height() * 0.5 + Constants::Node::MARGIN);
    m_textEdit->setMaxHeight(size().height() - Constants::Node::MARGIN * 4);
    m_textEdit->setMaxWidth(size().width() - Constants::Node::MARGIN * 2);
#endif
}

bool Node::isTextUnderflowOrOverflow() const
{
  const double tolerance = 0.001;
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
        int(-size().width() / 2), int(-size().height() / 2), int(size().width()), int(size().height()), QBrush(color()));

    painter->restore();
}

void Node::setHandlesVisible(bool visible, bool all)
{
    if (all)
    {
        for (auto handle : m_handles)
        {
            handle->setVisible(visible);
        }
    }
    else
    {
        for (auto handle : m_handles)
        {
            if (!visible)
            {
                if (!handle->contains(m_currentMousePos))
                {
                    handle->setVisible(visible);
                }
            }
            else
            {
                if (handle->contains(m_currentMousePos))
                {
                    handle->setVisible(visible);
                }
            }
        }
    }
}

void Node::setLocation(QPointF newLocation)
{
    NodeBase::setLocation(newLocation);
    setPos(newLocation);

    updateEdgeLines();

    setHandlesVisible(false);
}

void Node::setSelected(bool selected)
{
    setGraphicsEffect(GraphicsFactory::createDropShadowEffect(selected));
    update();
}

void Node::setText(const QString & text)
{
    if (text != this->text())
    {
        NodeBase::setText(text);
        m_textEdit->setText(text);

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

void Node::setTextSize(int textSize)
{
    m_textEdit->setTextSize(textSize);

    if (isTextUnderflowOrOverflow())
    {
        adjustSize();
    }
}

QString Node::text() const
{
    return m_textEdit->text();
}

void Node::updateEdgeLines()
{
    for (auto edge : m_graphicsEdges)
    {
        edge->updateLine();
    }
}

Node::~Node() = default;
