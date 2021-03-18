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
#include "graphics_factory.hpp"
#include "image.hpp"
#include "layers.hpp"
#include "test_mode.hpp"
#include "text_edit.hpp"

#include "simple_logger.hpp"

#include <QGraphicsEffect>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QImage>
#include <QPainter>
#include <QPen>
#include <QVector2D>

#include <algorithm>
#include <cmath>

Node * Node::m_lastHoveredNode = nullptr;

Node::Node()
  : m_textEdit(new TextEdit(this))
{
    setAcceptHoverEvents(true);

    setGraphicsEffect(GraphicsFactory::createDropShadowEffect());

    m_size = QSize(Constants::Node::MIN_WIDTH, Constants::Node::MIN_HEIGHT);

    setZValue(static_cast<int>(Layers::Node));

    createEdgePoints();

    createHandles();

    initTextField();

    setSelected(false);

    connect(m_textEdit, &TextEdit::textChanged, [=](const QString & text) {
        setText(text);
        adjustSize();
    });

    connect(m_textEdit, &TextEdit::undoPointRequested, this, &Node::undoPointRequested);

    // Set the background transparent as the TextEdit background will be rendered in Node::paint().
    // The reason for this is that TextEdit's background affects only the area that includes letters
    // and we want to render a larger area.
    m_textEdit->setBackgroundColor({ 0, 0, 0, 0 });
}

Node::Node(const Node & other)
  : Node()
{
    setColor(other.m_color);

    setCornerRadius(other.m_cornerRadius);

    setImageRef(other.m_imageRef);

    setIndex(other.m_index);

    setLocation(other.m_location);

    m_size = other.m_size;

    setText(other.text());

    setTextColor(other.m_textColor);

    setTextSize(other.m_textSize);
}

void Node::addGraphicsEdge(Edge & edge)
{
    if (!TestMode::enabled()) {
        m_graphicsEdges.push_back(&edge);
    } else {
        TestMode::logDisabledCode("addGraphicsEdge");
    }
}

void Node::removeGraphicsEdge(Edge & edge)
{
    if (!TestMode::enabled()) {
        const auto iter = std::find(m_graphicsEdges.begin(), m_graphicsEdges.end(), &edge);
        if (iter != m_graphicsEdges.end()) {
            m_graphicsEdges.erase(iter);
        }
    } else {
        TestMode::logDisabledCode("removeGraphicsEdge");
    }
}

void Node::adjustSize()
{
    prepareGeometryChange();

    const auto margin = Constants::Node::MARGIN * 2;
    const auto newSize = QSize {
        std::max(Constants::Node::MIN_WIDTH, static_cast<int>(m_textEdit->boundingRect().width() + margin)),
        std::max(Constants::Node::MIN_HEIGHT, static_cast<int>(m_textEdit->boundingRect().height() + margin))
    };

    m_size = newSize;

    createEdgePoints();

    updateEdgeLines();

    updateHandlePositions();

    initTextField();

    update();
}

QRectF Node::boundingRect() const
{
    return { -m_size.width() / 2, -m_size.height() / 2, m_size.width(), m_size.height() };
}

EdgePtr Node::createAndAddGraphicsEdge(NodePtr targetNode)
{
    const auto edge = std::make_shared<Edge>(*this, *targetNode);
    edge->updateLine();
    m_graphicsEdges.push_back(edge.get());
    return edge;
}

void Node::createEdgePoints()
{
    const double w2 = m_size.width() * 0.5;
    const double h2 = m_size.height() * 0.5;
    const double bias = 0.1;

    m_edgePoints = {
        { { -w2, h2 }, true },
        { { 0, h2 + bias }, false },
        { { w2, h2 }, true },
        { { w2 + bias, 0 }, false },
        { { w2, -h2 }, true },
        { { 0, -h2 - bias }, false },
        { { -w2, -h2 }, true },
        { { -w2 - bias, 0 }, false }
    };
}

void Node::createHandles()
{
    m_handles[NodeHandle::Role::Add] = new NodeHandle(*this, NodeHandle::Role::Add, Constants::Node::HANDLE_RADIUS);

    m_handles[NodeHandle::Role::Color] = new NodeHandle(*this, NodeHandle::Role::Color, Constants::Node::HANDLE_RADIUS_SMALL);

    m_handles[NodeHandle::Role::TextColor] = new NodeHandle(*this, NodeHandle::Role::TextColor, Constants::Node::HANDLE_RADIUS_SMALL);

    m_handles[NodeHandle::Role::Drag] = new NodeHandle(*this, NodeHandle::Role::Drag, Constants::Node::HANDLE_RADIUS_MEDIUM);

    updateHandlePositions();
}

QRectF Node::expandedTextEditRect() const
{
    auto textEditRect = QRectF {};
    textEditRect.setX(m_textEdit->pos().x());
    textEditRect.setY(m_textEdit->pos().y());
    textEditRect.setWidth(m_size.width() - Constants::Node::MARGIN * 2);
    textEditRect.setHeight(m_textEdit->boundingRect().height());
    return textEditRect;
}

std::pair<EdgePoint, EdgePoint> Node::getNearestEdgePoints(const Node & node1, const Node & node2)
{
    double bestDistance = std::numeric_limits<double>::max();
    std::pair<EdgePoint, EdgePoint> bestPair = { EdgePoint(), EdgePoint() };

    // This is O(n^2) but fine as there are not many points
    for (auto && point1 : node1.m_edgePoints) {
        for (auto && point2 : node2.m_edgePoints) {
            const auto distance = std::pow(node1.pos().x() + point1.location.x() - node2.pos().x() - point2.location.x(), 2) + std::pow(node1.pos().y() + point1.location.y() - node2.pos().y() - point2.location.y(), 2);
            if (distance < bestDistance) {
                bestDistance = distance;
                bestPair = { point1, point2 };
            }
        }
    }

    return bestPair;
}

void Node::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
    // This is to more quickly hide the handles of the previous node when
    // hovering on another node.
    if (Node::m_lastHoveredNode && Node::m_lastHoveredNode != this) {
        Node::m_lastHoveredNode->setHandlesVisible(false);
    }
    Node::m_lastHoveredNode = this;

    setHandlesVisible(true);

    QGraphicsItem::hoverEnterEvent(event);
}

void Node::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{
    setHandlesVisible(true);

    QGraphicsItem::hoverMoveEvent(event);
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    // Prevent left-click on the drag node
    if (event && index() != -1) {
        if (expandedTextEditRect().contains(event->pos())) {
            m_textEdit->setFocus();
        }
        QGraphicsItem::mousePressEvent(event);
    }
}

void Node::initTextField()
{
    if (!TestMode::enabled()) {
        m_textEdit->setTextWidth(-1);
        m_textEdit->setPos(-m_size.width() * 0.5 + Constants::Node::MARGIN, -m_size.height() * 0.5 + Constants::Node::MARGIN);
    } else {
        TestMode::logDisabledCode("initTestField");
    }
}

void Node::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)

    for (auto && handle : m_handles) {
        if (!handle.second->scene())
            scene()->addItem(handle.second);
    }

    painter->save();

    // Background

    QPainterPath path;
    const QRectF rect(-m_size.width() / 2, -m_size.height() / 2, m_size.width(), m_size.height());
    path.addRoundedRect(rect, m_cornerRadius, m_cornerRadius);
    painter->setRenderHint(QPainter::Antialiasing);

    if (!m_pixmap.isNull()) {
        QPixmap scaledPixmap(static_cast<int>(m_size.width()), static_cast<int>(m_size.height()));
        scaledPixmap.fill(Qt::transparent);
        QPainter pixmapPainter(&scaledPixmap);
        QPainterPath scaledPath;
        const QRectF scaledRect(0, 0, m_size.width(), m_size.height());
        scaledPath.addRoundedRect(scaledRect, m_cornerRadius, m_cornerRadius);

        const auto pixmapAspect = static_cast<double>(m_pixmap.width()) / m_pixmap.height();
        const auto nodeAspect = m_size.width() / m_size.height();
        if (nodeAspect > 1.0) {
            if (pixmapAspect > nodeAspect) {
                pixmapPainter.fillPath(scaledPath, QBrush(m_pixmap.scaledToHeight(static_cast<int>(m_size.height()))));
            } else {
                pixmapPainter.fillPath(scaledPath, QBrush(m_pixmap.scaledToWidth(static_cast<int>(m_size.width()))));
            }
        } else {
            if (pixmapAspect < nodeAspect) {
                pixmapPainter.fillPath(scaledPath, QBrush(m_pixmap.scaledToWidth(static_cast<int>(m_size.width()))));
            } else {
                pixmapPainter.fillPath(scaledPath, QBrush(m_pixmap.scaledToHeight(static_cast<int>(m_size.height()))));
            }
        }

        painter->drawPixmap(rect, scaledPixmap, scaledRect);
    } else {
        const QPen pen(QColor { 2 * m_color.red() / 3, 2 * m_color.green() / 3, 2 * m_color.blue() / 3 }, 1);
        painter->fillPath(path, QBrush(m_color));
        painter->strokePath(path, pen);
    }

    // Patch for TextEdit

    painter->fillRect(expandedTextEditRect(), Constants::Node::TEXT_EDIT_BACKGROUND_COLOR);

    painter->restore();
}

void Node::setColor(const QColor & color)
{
    m_color = color;
    if (!TestMode::enabled()) {
        update();
    } else {
        TestMode::logDisabledCode("update() on setColor");
    }
}

int Node::cornerRadius() const
{
    return m_cornerRadius;
}

void Node::setCornerRadius(int value)
{
    m_cornerRadius = value;

    updateEdgeLines();

    // Needed to redraw immediately on e.g. a new design. Otherwise updates start to work only after
    // the first mouse over, which is a bit weird.
    prepareGeometryChange();

    update();
}

void Node::setHandlesVisible(bool visible)
{
    for (auto && handle : m_handles) {
        handle.second->setVisible(visible && index() != -1);
    }
}

QPointF Node::location() const
{
    return m_location;
}

void Node::setLocation(QPointF newLocation)
{
    m_location = newLocation;
    setPos(newLocation);

    updateEdgeLines();

    updateHandlePositions();
}

QRectF Node::placementBoundingRect() const
{
    return { -m_size.width() / 2, -m_size.height() / 2, m_size.width(), m_size.height() };
}

bool Node::selected() const
{
    return m_selected;
}

void Node::setSelected(bool selected)
{
    m_selected = selected;
    GraphicsFactory::setSelected(graphicsEffect(), selected);
    update();
}

void Node::setTextInputActive(bool active)
{
    m_textEdit->setActive(active);
    if (active) {
        m_textEdit->setFocus();
    } else {
        m_textEdit->clearFocus();
    }
}

QString Node::text() const
{
    if (!TestMode::enabled()) {
        return m_textEdit->text();
    } else {
        TestMode::logDisabledCode("return widget text");
        return m_text;
    }
}

void Node::setText(const QString & text)
{
    if (text != m_text) {
        m_text = text;
        m_textEdit->setText(text);
        adjustSize();
    }
}

QColor Node::textColor() const
{
    return m_textColor;
}

void Node::setTextColor(const QColor & color)
{
    m_textColor = color;
    if (!TestMode::enabled()) {
        m_textEdit->setDefaultTextColor(color);
        m_textEdit->update();
    } else {
        TestMode::logDisabledCode("set widget color");
    }
}

void Node::setTextSize(int textSize)
{
    m_textSize = textSize;
    if (!TestMode::enabled()) {
        m_textEdit->setTextSize(textSize);
        adjustSize();
    } else {
        TestMode::logDisabledCode("set widget text size");
    }
}

void Node::setImageRef(size_t imageRef)
{
    if (imageRef) {
        m_imageRef = imageRef;
        emit imageRequested(imageRef, *this);
    } else if (m_imageRef) {
        m_imageRef = imageRef;
        applyImage({});
    }
}

void Node::applyImage(const Image & image)
{
    m_pixmap = QPixmap::fromImage(image.image());

    update();
}

void Node::updateEdgeLines()
{
    for (auto && edge : m_graphicsEdges) {
        edge->updateLine();
    }
}

void Node::updateHandlePositions()
{
    m_handles[NodeHandle::Role::Add]->setPos(pos() + QPointF { 0, m_size.height() * 0.5 });

    m_handles[NodeHandle::Role::Color]->setPos(pos() + QPointF { m_size.width() * 0.5, m_size.height() * 0.5 - Constants::Node::HANDLE_RADIUS_SMALL * 0.5 });

    m_handles[NodeHandle::Role::TextColor]->setPos(pos() + QPointF { m_size.width() * 0.5, -m_size.height() * 0.5 + Constants::Node::HANDLE_RADIUS_SMALL * 0.5 });

    m_handles[NodeHandle::Role::Drag]->setPos(pos() + QPointF { -m_size.width() * 0.5 - Constants::Node::HANDLE_RADIUS_SMALL * 0.15, -m_size.height() * 0.5 - Constants::Node::HANDLE_RADIUS_SMALL * 0.15 });
}

Node * Node::lastHoveredNode()
{
    return m_lastHoveredNode;
}

QSizeF Node::size() const
{
    return m_size;
}

void Node::setSize(const QSizeF & size)
{
    m_size = size;
}

size_t Node::imageRef() const
{
    return m_imageRef;
}

QColor Node::color() const
{
    return m_color;
}

int Node::index() const
{
    return m_index;
}

void Node::setIndex(int index)
{
    m_index = index;
}

Node::~Node()
{
    if (Node::m_lastHoveredNode == this) {
        Node::m_lastHoveredNode = nullptr;
    }

    juzzlin::L().debug() << "Deleting Node " << index();
}
