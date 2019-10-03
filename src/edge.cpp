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

#include "constants.hpp"
#include "edge_dot.hpp"
#include "edge_text_edit.hpp"
#include "graphics_factory.hpp"
#include "layers.hpp"
#include "node.hpp"

#include "simple_logger.hpp"

#include <QBrush>
#include <QGraphicsEllipseItem>
#include <QPen>
#include <QPropertyAnimation>
#include <QTimer>
#include <QVector2D>

#include <QtMath>

#include <cassert>
#include <cmath>

Edge::Edge(Node & sourceNode, Node & targetNode, bool enableAnimations, bool enableLabel)
  : EdgeBase(sourceNode, targetNode)
  , m_enableAnimations(enableAnimations)
  , m_enableLabel(enableLabel)
  , m_sourceDot(enableAnimations ? new EdgeDot(this) : nullptr)
  , m_targetDot(enableAnimations ? new EdgeDot(this) : nullptr)
  , m_label(enableLabel ? new EdgeTextEdit(this) : nullptr)
  , m_arrowheadL0(new QGraphicsLineItem(this))
  , m_arrowheadR0(new QGraphicsLineItem(this))
  , m_arrowheadL1(new QGraphicsLineItem(this))
  , m_arrowheadR1(new QGraphicsLineItem(this))
  , m_sourceDotSizeAnimation(enableAnimations ? new QPropertyAnimation(m_sourceDot, "scale", this) : nullptr)
  , m_targetDotSizeAnimation(enableAnimations ? new QPropertyAnimation(m_targetDot, "scale", this) : nullptr)
{
    setAcceptHoverEvents(true && enableAnimations);

    setGraphicsEffect(GraphicsFactory::createDropShadowEffect());

    setZValue(static_cast<int>(Layers::Edge));

    initDots();

    if (m_enableLabel) {
        m_label->setZValue(static_cast<int>(Layers::EdgeLabel));
        m_label->setBackgroundColor(Constants::Edge::LABEL_COLOR);

        connect(m_label, &TextEdit::textChanged, [=](const QString & text) {
            updateLabel();
            EdgeBase::setText(text);
        });

        connect(m_label, &TextEdit::undoPointRequested, this, &Edge::undoPointRequested);

        m_labelVisibilityTimer.setSingleShot(true);
        m_labelVisibilityTimer.setInterval(Constants::Edge::LABEL_DURATION);

        connect(&m_labelVisibilityTimer, &QTimer::timeout, [=]() {
            setLabelVisible(false);
        });
    }
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

QPen Edge::getPen() const
{
    return QPen { QBrush { QColor { color().red(), color().green(), color().blue(), 200 } }, width() };
}

void Edge::initDots()
{
    if (m_enableAnimations) {
        m_sourceDot->setPen(QPen(Constants::Edge::DOT_COLOR));
        m_sourceDot->setBrush(QBrush(Constants::Edge::DOT_COLOR));

        m_targetDot->setPen(QPen(Constants::Edge::DOT_COLOR));
        m_targetDot->setBrush(QBrush(Constants::Edge::DOT_COLOR));

        m_sourceDotSizeAnimation->setDuration(Constants::Edge::DOT_DURATION);
        m_sourceDotSizeAnimation->setStartValue(1.0);
        m_sourceDotSizeAnimation->setEndValue(0.0);

        const QRectF rect(-Constants::Edge::DOT_RADIUS, -Constants::Edge::DOT_RADIUS, Constants::Edge::DOT_RADIUS * 2, Constants::Edge::DOT_RADIUS * 2);
        m_sourceDot->setRect(rect);

        m_targetDotSizeAnimation->setDuration(Constants::Edge::DOT_DURATION);
        m_targetDotSizeAnimation->setStartValue(1.0);
        m_targetDotSizeAnimation->setEndValue(0.0);

        m_targetDot->setRect(rect);
    }
}

void Edge::setArrowHeadPen(const QPen & pen)
{
    m_arrowheadL0->setPen(pen);
    m_arrowheadL0->update();
    m_arrowheadR0->setPen(pen);
    m_arrowheadR0->update();
    m_arrowheadL1->setPen(pen);
    m_arrowheadL1->update();
    m_arrowheadR1->setPen(pen);
    m_arrowheadR1->update();
}

void Edge::setLabelVisible(bool visible)
{
    m_label->setVisible(visible);
}

void Edge::setWidth(double width)
{
    EdgeBase::setWidth(width);

    setPen(getPen());
    setArrowHeadPen(pen());
    updateLine();
}

void Edge::setArrowMode(ArrowMode arrowMode)
{
    EdgeBase::setArrowMode(arrowMode);
#ifndef HEIMER_UNIT_TEST
    updateLine();
#endif
}

void Edge::setColor(const QColor & color)
{
    EdgeBase::setColor(color);

    setPen(getPen());
    setArrowHeadPen(pen());
    updateLine();
}

void Edge::setText(const QString & text)
{
    EdgeBase::setText(text);
#ifndef HEIMER_UNIT_TEST
    if (m_label) {
        m_label->setText(text);
    }
    setLabelVisible(!text.isEmpty());
#endif
}

void Edge::setTextSize(int textSize)
{
    if (m_label) {
        m_label->setTextSize(textSize);
    }
}

void Edge::setReversed(bool reversed)
{
    EdgeBase::setReversed(reversed);

    updateArrowhead();
}

void Edge::setSelected(bool selected)
{
    EdgeBase::setSelected(selected);
    setGraphicsEffect(GraphicsFactory::createDropShadowEffect(selected));
    update();
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

void Edge::updateArrowhead()
{
    const auto point0 = reversed() ? this->line().p1() : this->line().p2();
    const auto angle0 = reversed() ? -this->line().angle() + 180 : -this->line().angle();
    const auto point1 = reversed() ? this->line().p2() : this->line().p1();
    const auto angle1 = reversed() ? -this->line().angle() : -this->line().angle() + 180;

    QLineF lineL0;
    QLineF lineR0;
    QLineF lineL1;
    QLineF lineR1;

    switch (arrowMode()) {
    case ArrowMode::Single: {
        lineL0.setP1(point0);
        const auto angleL = qDegreesToRadians(angle0 + Constants::Edge::ARROW_OPENING);
        lineL0.setP2(point0 + QPointF(std::cos(angleL), std::sin(angleL)) * Constants::Edge::ARROW_LENGTH);
        lineR0.setP1(point0);
        const auto angleR = qDegreesToRadians(angle0 - Constants::Edge::ARROW_OPENING);
        lineR0.setP2(point0 + QPointF(std::cos(angleR), std::sin(angleR)) * Constants::Edge::ARROW_LENGTH);
        m_arrowheadL0->setLine(lineL0);
        m_arrowheadR0->setLine(lineR0);
        m_arrowheadL0->show();
        m_arrowheadR0->show();
        m_arrowheadL1->hide();
        m_arrowheadR1->hide();
        break;
    }
    case ArrowMode::Double: {
        lineL0.setP1(point0);
        const auto angleL0 = qDegreesToRadians(angle0 + Constants::Edge::ARROW_OPENING);
        lineL0.setP2(point0 + QPointF(std::cos(angleL0), std::sin(angleL0)) * Constants::Edge::ARROW_LENGTH);
        lineR0.setP1(point0);
        const auto angleR0 = qDegreesToRadians(angle0 - Constants::Edge::ARROW_OPENING);
        lineR0.setP2(point0 + QPointF(std::cos(angleR0), std::sin(angleR0)) * Constants::Edge::ARROW_LENGTH);
        lineL1.setP1(point1);
        m_arrowheadL0->setLine(lineL0);
        m_arrowheadR0->setLine(lineR0);
        m_arrowheadL0->show();
        m_arrowheadR0->show();
        const auto angleL1 = qDegreesToRadians(angle1 + Constants::Edge::ARROW_OPENING);
        lineL1.setP2(point1 + QPointF(std::cos(angleL1), std::sin(angleL1)) * Constants::Edge::ARROW_LENGTH);
        lineR1.setP1(point1);
        const auto angleR1 = qDegreesToRadians(angle1 - Constants::Edge::ARROW_OPENING);
        lineR1.setP2(point1 + QPointF(std::cos(angleR1), std::sin(angleR1)) * Constants::Edge::ARROW_LENGTH);
        m_arrowheadL1->setLine(lineL1);
        m_arrowheadR1->setLine(lineR1);
        m_arrowheadL1->show();
        m_arrowheadR1->show();
        break;
    }
    case ArrowMode::Hidden:
        m_arrowheadL0->hide();
        m_arrowheadR0->hide();
        m_arrowheadL1->hide();
        m_arrowheadR1->hide();
        break;
    }
}

void Edge::updateDots(const std::pair<EdgePoint, EdgePoint> & nearestPoints)
{
    if (m_enableAnimations) {
        if (m_sourceDot->pos() != nearestPoints.first.location) {
            m_sourceDot->setPos(nearestPoints.first.location);

            // Re-parent to source node due to Z-ordering issues
            m_sourceDot->setParentItem(&sourceNode());

            m_sourceDotSizeAnimation->stop();
            m_sourceDotSizeAnimation->start();
        }

        if (m_targetDot->pos() != nearestPoints.second.location) {
            m_targetDot->setPos(nearestPoints.second.location);

            // Re-parent to target node due to Z-ordering issues
            m_targetDot->setParentItem(&targetNode());

            m_targetDotSizeAnimation->stop();
            m_targetDotSizeAnimation->start();
        }
    }
}

void Edge::updateLabel()
{
    if (m_label) {
        m_label->setPos((line().p1() + line().p2()) * 0.5 - QPointF(m_label->boundingRect().width(), m_label->boundingRect().height()) * 0.5);
    }
}

void Edge::updateLine()
{
    const auto nearestPoints = Node::getNearestEdgePoints(sourceNode(), targetNode());
    const auto p1 = nearestPoints.first.location + sourceNode().pos();
    const auto p2 = nearestPoints.second.location + targetNode().pos();

    QVector2D direction(p2 - p1);
    direction.normalize();

    setLine(QLineF(
      p1 - (nearestPoints.first.isCorner ? Constants::Edge::CORNER_RADIUS_SCALE * (direction * sourceNode().cornerRadius()).toPointF() : QPointF { 0, 0 }),
      p2 + (nearestPoints.second.isCorner ? Constants::Edge::CORNER_RADIUS_SCALE * (direction * targetNode().cornerRadius()).toPointF() : QPointF { 0, 0 }) - (direction * static_cast<float>(width())).toPointF() * Constants::Edge::WIDTH_SCALE));

    updateDots(nearestPoints);
    updateLabel();
    updateArrowhead();
}

Edge::~Edge()
{
    if (m_enableAnimations) {
        m_sourceDotSizeAnimation->stop();
        m_targetDotSizeAnimation->stop();
        delete m_sourceDot;
        delete m_targetDot;
    }

#ifndef HEIMER_UNIT_TEST
    sourceNode().removeGraphicsEdge(*this);
    targetNode().removeGraphicsEdge(*this);

    // Needed to remove glitches from possibly running dot animations
    sourceNode().update();
    targetNode().update();
#endif
}
