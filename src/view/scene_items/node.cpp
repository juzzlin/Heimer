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

#include "../../../common/test_mode.hpp"
#include "../../application/application_service.hpp"
#include "../../application/language_service.hpp"
#include "../../application/service_container.hpp"
#include "../../application/settings_proxy.hpp"
#include "../../common/constants.hpp"
#include "../../common/utils.hpp"
#include "../../domain/image.hpp"
#include "../shadow_effect_params.hpp"
#include "edge.hpp"
#include "graphics_factory.hpp"
#include "layers.hpp"
#include "node_model.hpp"
#include "text_edit.hpp"

#include "simple_logger.hpp"

#include <QDebug>
#include <QFont>
#include <QGraphicsEffect>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QImage>
#include <QPainter>
#include <QPen>
#include <QTextCursor>
#include <QVector2D>

#include <algorithm>
#include <cmath>

namespace SceneItems {

static const auto TAG = "Node";

NodeP Node::m_lastHoveredNode = nullptr;

Node::Node()
  : m_settingsProxy(SC::instance().settingsProxy())
  , m_nodeModel(std::make_unique<NodeModel>(m_settingsProxy->nodeColor(), m_settingsProxy->nodeTextColor()))
  , m_textEdit(new TextEdit(this))
{
    setAcceptHoverEvents(true);

    setGraphicsEffect(GraphicsFactory::createDropShadowEffect(m_settingsProxy->shadowEffect(), false));

    m_nodeModel->size = QSize { Constants::Node::minWidth(), Constants::Node::minHeight() };

    setZValue(static_cast<int>(Layers::Node));

    createEdgePoints();

    createHandles();

    updateHandlePositions();

    initTextField();

    setSelected(false);

    connect(m_textEdit, &TextEdit::textChanged, this, [=](const QString & text) {
        setText(text);
        adjustSize();
    });

    connect(m_textEdit, &TextEdit::undoPointRequested, this, &Node::undoPointRequested);

    // Set the background transparent as the TextEdit background will be rendered in Node::paint().
    // The reason for this is that TextEdit's background affects only the area that includes letters
    // and we want to render a larger area.
    m_textEdit->setBackgroundColor({ 0, 0, 0, 0 });

    setTextColor(m_nodeModel->textColor);
}

Node::Node(NodeCR other)
  : Node()
{
    setColor(other.m_nodeModel->color);

    setCornerRadius(other.m_cornerRadius);

    setImageRef(other.m_nodeModel->imageRef);

    setIndex(other.m_nodeModel->index);

    setLocation(other.m_nodeModel->location);

    m_nodeModel->size = other.m_nodeModel->size;

    setText(other.text());

    setTextColor(other.m_nodeModel->textColor);

    setTextSize(other.m_textSize);

    changeFont(other.m_font);
}

void Node::addGraphicsEdge(EdgeR edge)
{
    if (!TestMode::enabled()) {
        m_graphicsEdges.push_back(&edge);
    } else {
        TestMode::logDisabledCode("addGraphicsEdge");
    }
}

void Node::removeGraphicsEdge(EdgeR edge)
{
    if (!TestMode::enabled()) {
        if (const auto iter = std::find(m_graphicsEdges.begin(), m_graphicsEdges.end(), &edge); iter != m_graphicsEdges.end()) {
            m_graphicsEdges.erase(iter);
        }
    } else {
        TestMode::logDisabledCode("removeGraphicsEdge");
    }
}

void Node::removeFromScene()
{
    hide();
    removeHandles();
    if (scene()) {
        scene()->removeItem(this);
    }
}

void Node::raiseBody()
{
    const auto currentSizeRatio = SC::instance().applicationService()->normalizedSizeInView(boundingRect()).width();
    const auto targetSizeRatio = 0.1;
    const auto targetScale = currentSizeRatio < targetSizeRatio ? targetSizeRatio / currentSizeRatio : 1.1;
    raiseWithAnimation(targetScale);
    setZValue(static_cast<int>(Layers::Last) + static_cast<int>(Layers::Node));
}

void Node::raiseHandles()
{
    for (auto && [role, handle] : m_handles) {
        handle->raiseWithAnimation(targetScale());
        handle->setZValue(static_cast<int>(Layers::Last) + static_cast<int>(Layers::NodeHandle));
    }
}

void Node::removeHandles()
{
    for (auto && [role, handle] : m_handles) {
        if (handle->scene()) {
            handle->hide();
            handle->scene()->removeItem(handle);
        }
    }
}

void Node::adjustSize()
{
    prepareGeometryChange();

    const auto newSize = QSize {
        std::max(Constants::Node::minWidth(), static_cast<int>(m_textEdit->boundingRect().width() + m_contentPadding * 2)),
        std::max(Constants::Node::minHeight(), static_cast<int>(m_textEdit->boundingRect().height() + m_contentPadding * 2))
    };

    m_nodeModel->size = newSize;

    createEdgePoints();

    updateEdgeLines();

    updateHandlePositions();

    initTextField();

    update();
}

QRectF Node::boundingRect() const
{
    const auto size = m_nodeModel->size * targetScale();
    return { -size.width() / 2, -size.height() / 2, size.width(), size.height() };
}

void Node::changeFont(const QFont & font)
{
    m_font = font;
    if (!TestMode::enabled()) {
        // Handle size and family separately to maintain backwards compatibility
        QFont newFont(font);
        if (m_textSize >= 0) {
            newFont.setPointSize(m_textSize);
        }
        m_textEdit->setFont(newFont);
        adjustSize();
    } else {
        TestMode::logDisabledCode("set node font");
    }
}

void Node::createEdgePoints()
{
    const double w2 = m_nodeModel->size.width() * 0.5;
    const double h2 = m_nodeModel->size.height() * 0.5;
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
    m_handles[NodeHandle::Role::ConnectOrCreate] = new NodeHandle(*this, NodeHandle::Role::ConnectOrCreate, m_handleRadius);

    m_handles[NodeHandle::Role::NodeColor] = new NodeHandle(*this, NodeHandle::Role::NodeColor, m_handleRadiusSmall);

    m_handles[NodeHandle::Role::TextColor] = new NodeHandle(*this, NodeHandle::Role::TextColor, m_handleRadiusSmall);

    m_handles[NodeHandle::Role::Move] = new NodeHandle(*this, NodeHandle::Role::Move, m_handleRadiusMedium);
}

QRectF Node::expandedTextEditRect() const
{
    auto textEditRect = QRectF {};
    textEditRect.setX(m_textEdit->pos().x());
    textEditRect.setY(m_textEdit->pos().y());
    textEditRect.setWidth(m_nodeModel->size.width() - m_contentPadding * 2);
    textEditRect.setHeight(m_textEdit->boundingRect().height());
    return textEditRect;
}

inline double getDistance(NodeCR node1, const EdgePoint & point1, NodeCR node2, const EdgePoint & point2)
{
    return std::pow(node1.pos().x() + point1.location.x() - node2.pos().x() - point2.location.x(), 2) + //
      std::pow(node1.pos().y() + point1.location.y() - node2.pos().y() - point2.location.y(), 2);
}

std::pair<EdgePoint, EdgePoint> Node::getNearestEdgePoints(NodeCR node1, NodeCR node2)
{
    double bestDistance = std::numeric_limits<double>::max();
    std::pair<EdgePoint, EdgePoint> bestPair = { EdgePoint(), EdgePoint() };

    // This is O(n^2) but fine as there are not many points
    for (auto && point1 : node1.m_edgePoints) {
        for (auto && point2 : node2.m_edgePoints) {
            if (const auto distance = getDistance(node1, point1, node2, point2); distance < bestDistance) {
                bestDistance = distance;
                bestPair = { point1, point2 };
            }
        }
    }

    return bestPair;
}

bool Node::pointBeyondHideHandlesDistance(const QPointF & point) const
{
    static const int hideHandlesDistance = 32;
    const auto scaledHideHandlesDistance = hideHandlesDistance * targetScale();
    return point.x() > pos().x() + scaledHideHandlesDistance + boundingRect().width() / 2 || //
      point.x() < pos().x() - scaledHideHandlesDistance - boundingRect().width() / 2 || //
      point.y() > pos().y() + scaledHideHandlesDistance + boundingRect().height() / 2 || //
      point.y() < pos().y() - scaledHideHandlesDistance - boundingRect().height() / 2;
}

void Node::hideHandlesWithAnimation()
{
    if (m_settingsProxy->raiseNodeOnMouseHover()) {
        lowerWithAnimation();
        setZValue(static_cast<int>(Layers::Node));
    }

    setHandlesVisible(false);
}

void Node::highlightText(const QString & text)
{
    if (!TestMode::enabled()) {
        m_textEdit->selectText(text);
    } else {
        TestMode::logDisabledCode("highlightText");
    }
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

    if (m_settingsProxy->raiseNodeOnMouseHover()) {
        raiseBody();
        raiseHandles();
        updateHandlePositions();
    }

    QGraphicsItem::hoverEnterEvent(event);
}

void Node::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{
    setHandlesVisible(true);

    if (m_settingsProxy->raiseNodeOnMouseHover()) {
        raiseHandles();
        updateHandlePositions();
    }

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
        const auto size = m_nodeModel->size;
        m_textEdit->setPos(-size.width() * 0.5 + m_contentPadding, -size.height() * 0.5 + m_contentPadding);
    } else {
        TestMode::logDisabledCode("initTestField");
    }
}

void Node::addHandlesToScene()
{
    for (auto && handle : m_handles) {
        if (!handle.second->scene())
            scene()->addItem(handle.second);
    }
}

QBrush Node::scaledBackgroundPixmapBrush() const
{
    const auto size = m_nodeModel->size;
    const auto pixmapAspect = static_cast<double>(m_pixmap.width()) / m_pixmap.height();
    if (const auto nodeAspect = size.width() / size.height(); nodeAspect > 1.0) {
        if (pixmapAspect > nodeAspect) {
            return QBrush { m_pixmap.scaledToHeight(static_cast<int>(size.height())) };
        } else {
            return QBrush { m_pixmap.scaledToWidth(static_cast<int>(size.width())) };
        }
    } else {
        if (pixmapAspect < nodeAspect) {
            return QBrush { m_pixmap.scaledToWidth(static_cast<int>(size.width())) };
        } else {
            return QBrush { m_pixmap.scaledToHeight(static_cast<int>(size.height())) };
        }
    }
}

QPixmap Node::createEmptyBackgroundPixmap() const
{
    const auto size = m_nodeModel->size;
    QPixmap backgroundPixmap(static_cast<int>(size.width()), static_cast<int>(size.height()));
    backgroundPixmap.fill(Qt::transparent);
    return backgroundPixmap;
}

void Node::paintPixmapOnEmptyBackgroundPixmap(QPixmap & emptyBackgroundPixmap)
{
    QPainter pixmapPainter(&emptyBackgroundPixmap);
    QPainterPath scaledPath;
    const auto size = m_nodeModel->size;
    const QRectF scaledRect(0, 0, size.width(), size.height());
    scaledPath.addRoundedRect(scaledRect, m_cornerRadius, m_cornerRadius);
    pixmapPainter.fillPath(scaledPath, scaledBackgroundPixmapBrush());
}

void Node::paintBackgroundPixmapOnNode(QPainter & painter, const QPixmap & backgroundPixmap)
{
    const auto size = m_nodeModel->size;
    const QRectF scaledRect(0, 0, size.width(), size.height());
    const QRectF rect(-size.width() / 2, -size.height() / 2, size.width(), size.height());
    painter.drawPixmap(rect, backgroundPixmap, scaledRect);
}

void Node::paintBackgroundWithPixmap(QPainter & painter)
{
    auto && backgroundPixmap = createEmptyBackgroundPixmap();

    paintPixmapOnEmptyBackgroundPixmap(backgroundPixmap);

    paintBackgroundPixmapOnNode(painter, backgroundPixmap);
}

void Node::paintBackgroundWithSolidColor(QPainter & painter)
{
    QPainterPath path;
    const auto size = m_nodeModel->size;
    const QRectF rect(-size.width() / 2, -size.height() / 2, size.width(), size.height());
    path.addRoundedRect(rect, m_cornerRadius, m_cornerRadius);

    const auto color = m_nodeModel->color;
    painter.fillPath(path, QBrush(color));
    painter.strokePath(path, GraphicsFactory::createOutlinePen(color, 0.33));
}

void Node::paintBackground(QPainter & painter)
{
    painter.setRenderHint(QPainter::Antialiasing);

    if (!m_pixmap.isNull()) {
        paintBackgroundWithPixmap(painter);
    } else {
        paintBackgroundWithSolidColor(painter);
    }
}

void Node::paintPatchForTextEdit(QPainter & painter)
{
    painter.fillRect(expandedTextEditRect(),
                     Utils::isColorBright(m_nodeModel->color) ? m_textEditBackgroundColorDark : m_textEditBackgroundColorLight);
}

void Node::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)

    addHandlesToScene();

    painter->save();

    paintBackground(*painter);

    paintPatchForTextEdit(*painter);

    painter->restore();
}

void Node::setColor(const QColor & color)
{
    m_nodeModel->color = color;
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

void Node::enableShadowEffect(bool enable)
{
    GraphicsFactory::updateDropShadowEffect(graphicsEffect(), m_settingsProxy->shadowEffect(), m_selected, !enable);
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
    return m_nodeModel->location;
}

void Node::setLocation(QPointF newLocation)
{
    m_nodeModel->location = newLocation;

    setPos(newLocation);

    updateEdgeLines();

    updateHandlePositions();
}

QRectF Node::placementBoundingRect() const
{
    const auto size = m_nodeModel->size;
    return { -size.width() / 2, -size.height() / 2, size.width(), size.height() };
}

bool Node::selected() const
{
    return m_selected;
}

void Node::setSelected(bool selected)
{
    m_selected = selected;
    GraphicsFactory::updateDropShadowEffect(graphicsEffect(), m_settingsProxy->shadowEffect(), m_selected);
    update();
}

void Node::setShadowEffect(const ShadowEffectParams & params)
{
    GraphicsFactory::updateDropShadowEffect(graphicsEffect(), params, m_selected);
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
        return m_nodeModel->text;
    }
}

void Node::setText(const QString & text)
{
    if (text != m_nodeModel->text) {
        m_nodeModel->text = text;
        m_textEdit->setText(text);
        adjustSize();
    }
}

QColor Node::textColor() const
{
    return m_nodeModel->textColor;
}

void Node::setTextColor(const QColor & color)
{
    m_nodeModel->textColor = color;
    if (!TestMode::enabled()) {
        m_textEdit->setDefaultTextColor(color);
        m_textEdit->update();
    } else {
        TestMode::logDisabledCode("set widget color");
    }
}

void Node::setTextSize(int textSize)
{
    if (textSize <= 0) {
        return;
    }

    m_textSize = textSize;
    if (!TestMode::enabled()) {
        m_textEdit->setTextSize(textSize);
        adjustSize();
    } else {
        TestMode::logDisabledCode("set node text size");
    }
}

void Node::setImageRef(size_t imageRef)
{
    if (imageRef) {
        m_nodeModel->imageRef = imageRef;
        emit imageRequested(imageRef, *this);
    } else if (m_nodeModel->imageRef) {
        m_nodeModel->imageRef = imageRef;
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
    const auto height = m_nodeModel->size.height();

    const auto width = m_nodeModel->size.width();

    m_handles[NodeHandle::Role::ConnectOrCreate]->setPos(pos() + QPointF { 0, height * 0.5 } * targetScale());

    m_handles[NodeHandle::Role::NodeColor]->setPos(pos() + QPointF { width * 0.5, height * 0.5 - m_handleRadiusSmall * 0.5 } * targetScale());

    m_handles[NodeHandle::Role::TextColor]->setPos(pos() + QPointF { width * 0.5, -height * 0.5 + m_handleRadiusSmall * 0.5 } * targetScale());

    m_handles[NodeHandle::Role::Move]->setPos(pos() + QPointF { -width * 0.5 - m_handleRadiusSmall * 0.15, -height * 0.5 - m_handleRadiusSmall * 0.15 } * targetScale());
}

NodeP Node::lastHoveredNode()
{
    return m_lastHoveredNode;
}

QSizeF Node::size() const
{
    return m_nodeModel->size;
}

void Node::setSize(const QSizeF & size)
{
    m_nodeModel->size = size;
}

size_t Node::imageRef() const
{
    return m_nodeModel->imageRef;
}

QColor Node::color() const
{
    return m_nodeModel->color;
}

bool Node::containsText(const QString & text) const
{
    return m_nodeModel->text.contains(text, Qt::CaseInsensitive);
}

int Node::index() const
{
    return m_nodeModel->index;
}

void Node::setIndex(int index)
{
    m_nodeModel->index = index;
}

void Node::unselectText()
{
    m_textEdit->unselectText();
}

Node::~Node()
{
    if (Node::m_lastHoveredNode == this) {
        Node::m_lastHoveredNode = nullptr;
    }

    juzzlin::L(TAG).trace() << "Deleting handles of node id=" << index();

    for (auto && [role, handle] : m_handles) {
        delete handle;
    }

    juzzlin::L(TAG).trace() << "Deleting Node id=" << index();
}

} // namespace SceneItems
