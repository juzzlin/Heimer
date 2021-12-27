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
#include "graph.hpp"
#include "graphics_factory.hpp"
#include "layers.hpp"
#include "node.hpp"
#include "settings_proxy.hpp"
#include "test_mode.hpp"

#include "simple_logger.hpp"

#include <QBrush>
#include <QFont>
#include <QGraphicsEllipseItem>
#include <QPen>
#include <QPropertyAnimation>
#include <QTimer>
#include <QVector2D>

#include <QtMath>

#include <cassert>
#include <cmath>

Edge::Edge(NodeP sourceNode, NodeP targetNode, bool enableAnimations, bool enableLabel)
  : m_sourceNode(sourceNode)
  , m_targetNode(targetNode)
  , m_reversed(SettingsProxy::instance().reversedEdgeDirection())
  , m_arrowMode(SettingsProxy::instance().edgeArrowMode())
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
            m_text = text;
        });

        connect(m_label, &TextEdit::undoPointRequested, this, &Edge::undoPointRequested);

        m_labelVisibilityTimer.setSingleShot(true);
        m_labelVisibilityTimer.setInterval(Constants::Edge::LABEL_DURATION);

        connect(&m_labelVisibilityTimer, &QTimer::timeout, [=] {
            setLabelVisible(false);
        });
    }
}

Edge::Edge(NodeS sourceNode, NodeS targetNode, bool enableAnimations, bool enableLabel)
  : Edge(sourceNode.get(), targetNode.get(), enableAnimations, enableLabel)
{
}

Edge::Edge(EdgeCR other, GraphCR graph)
  : Edge(graph.getNode(other.m_sourceNode->index()).get(), graph.getNode(other.m_targetNode->index()).get())
{
    copyData(other);
}

Edge::Edge(EdgeCR other)
  : Edge(nullptr, nullptr)
{
    copyData(other);
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

QPen Edge::buildPen(bool ignoreDashSetting) const
{
    QPen pen { QBrush { QColor { m_color.red(), m_color.green(), m_color.blue() } }, m_width };
    pen.setCapStyle(Qt::PenCapStyle::RoundCap);
    if (!ignoreDashSetting && m_dashedLine) {
        pen.setDashPattern(Constants::Edge::DASH_PATTERN);
    }
    return pen;
}

void Edge::copyData(EdgeCR other)
{
    m_arrowMode = other.m_arrowMode;
    m_dashedLine = other.m_dashedLine;
    m_reversed = other.m_reversed;

    setText(other.m_text); // Update text to the label component
}

void Edge::changeFont(const QFont & font)
{
    if (m_label) {
        // Handle size and family separately to maintain backwards compatibility
        QFont newFont(font);
        if (m_label->font().pointSize() >= 0) {
            newFont.setPointSize(m_label->font().pointSize());
        }
        m_label->setFont(newFont);
    }
}

bool Edge::dashedLine() const
{
    return m_dashedLine;
}

void Edge::initDots()
{
    if (m_enableAnimations) {
        m_sourceDot->setPen(QPen(Constants::Edge::DOT_COLOR));
        m_sourceDot->setBrush(QBrush(Constants::Edge::DOT_COLOR));
        m_sourceDot->setZValue(zValue() + 10);

        m_targetDot->setPen(QPen(Constants::Edge::DOT_COLOR));
        m_targetDot->setBrush(QBrush(Constants::Edge::DOT_COLOR));
        m_targetDot->setZValue(zValue() + 10);

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

void Edge::setLabelVisible(bool visible, EdgeTextEdit::VisibilityChangeReason vcr)
{
    if (m_label) {
        m_label->setVisible(visible, vcr);
    }
}

void Edge::setWidth(double width)
{
    m_width = width;

    updateLine();
}

void Edge::setArrowMode(ArrowMode arrowMode)
{
    m_arrowMode = arrowMode;
    if (!TestMode::enabled()) {
        updateLine();
    } else {
        TestMode::logDisabledCode("Update line after arrow mode change");
    }
}

void Edge::setColor(const QColor & color)
{
    m_color = color;

    updateLine();
}

void Edge::setDashedLine(bool enable)
{
    m_dashedLine = enable;
    if (!TestMode::enabled()) {
        updateLine();
    } else {
        TestMode::logDisabledCode("Set dashed line");
    }
}

void Edge::setText(const QString & text)
{
    m_text = text;
    if (!TestMode::enabled()) {
        if (m_label) {
            m_label->setText(text);
        }
        setLabelVisible(!text.isEmpty());
    } else {
        TestMode::logDisabledCode("Set label text");
    }
}

void Edge::setTextSize(int textSize)
{
    if (textSize <= 0) {
        return;
    }

    if (m_label) {
        m_label->setTextSize(textSize);
    }
}

void Edge::setReversed(bool reversed)
{
    m_reversed = reversed;

    updateArrowhead();
}

void Edge::setSelected(bool selected)
{
    m_selected = selected;
    GraphicsFactory::setSelected(graphicsEffect(), selected);
    update();
}

NodeR Edge::sourceNode() const
{
    return *m_sourceNode;
}

NodeR Edge::targetNode() const
{
    return *m_targetNode;
}

void Edge::updateArrowhead()
{
    setArrowHeadPen(buildPen(true));

    const auto point0 = m_reversed ? this->line().p1() : this->line().p2();
    const auto angle0 = m_reversed ? -this->line().angle() + 180 : -this->line().angle();
    const auto point1 = m_reversed ? this->line().p2() : this->line().p1();
    const auto angle1 = m_reversed ? -this->line().angle() : -this->line().angle() + 180;

    QLineF lineL0;
    QLineF lineR0;
    QLineF lineL1;
    QLineF lineR1;

    switch (m_arrowMode) {
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

void Edge::updateDots()
{
    if (m_enableAnimations) {
        // Trigger new animation if relative connection location has changed
        const auto newRelativeSourcePos = line().p1() - sourceNode().pos();
        if (m_previousRelativeSourcePos != newRelativeSourcePos) {
            m_previousRelativeSourcePos = newRelativeSourcePos;
            m_sourceDotSizeAnimation->stop();
            m_sourceDotSizeAnimation->start();
        }

        // Update location of possibly active animation
        m_sourceDot->setPos(line().p1());

        // Trigger new animation if relative connection location has changed
        const auto newRelativeTargetPos = line().p2() - targetNode().pos();
        if (m_previousRelativeTargetPos != newRelativeTargetPos) {
            m_previousRelativeTargetPos = newRelativeTargetPos;
            m_targetDotSizeAnimation->stop();
            m_targetDotSizeAnimation->start();
        }

        // Update location of possibly active animation
        m_targetDot->setPos(line().p2());
    }
}

void Edge::updateLabel(LabelUpdateReason lur)
{
    if (m_label) {
        m_label->setPos((line().p1() + line().p2()) * 0.5 - QPointF(m_label->boundingRect().width(), m_label->boundingRect().height()) * 0.5);

        // Toggle visibility according to space available if geometry changed
        if (lur == LabelUpdateReason::EdgeGeometryChanged) {
            setLabelVisible(!m_label->sceneBoundingRect().intersects(sourceNode().sceneBoundingRect()) && !m_label->sceneBoundingRect().intersects(targetNode().sceneBoundingRect()), EdgeTextEdit::VisibilityChangeReason::AvailableSpaceChanged);
        }
    }
}

void Edge::setTargetNode(NodeR targetNode)
{
    m_targetNode = &targetNode;
}

void Edge::setSourceNode(NodeR sourceNode)
{
    m_sourceNode = &sourceNode;
}

bool Edge::reversed() const
{
    return m_reversed;
}

Edge::ArrowMode Edge::arrowMode() const
{
    return m_arrowMode;
}

QString Edge::text() const
{
    return m_text;
}

void Edge::updateLine()
{
    setPen(buildPen());

    const auto nearestPoints = Node::getNearestEdgePoints(sourceNode(), targetNode());

    const auto p1 = nearestPoints.first.location + sourceNode().pos();
    QVector2D direction1(sourceNode().pos() - p1);
    direction1.normalize();

    const auto p2 = nearestPoints.second.location + targetNode().pos();
    QVector2D direction2(targetNode().pos() - p2);
    direction2.normalize();

    setLine(QLineF(
      p1 + (nearestPoints.first.isCorner ? Constants::Edge::CORNER_RADIUS_SCALE * (direction1 * static_cast<float>(sourceNode().cornerRadius())).toPointF() : QPointF { 0, 0 }),
      p2 + (nearestPoints.second.isCorner ? Constants::Edge::CORNER_RADIUS_SCALE * (direction2 * static_cast<float>(targetNode().cornerRadius())).toPointF() : QPointF { 0, 0 }) - //
        (direction2 * static_cast<float>(m_width)).toPointF() * Constants::Edge::WIDTH_SCALE));

    updateDots();
    updateLabel(LabelUpdateReason::EdgeGeometryChanged);
    updateArrowhead();
}

Edge::~Edge()
{
    juzzlin::L().debug() << "Deleting edge " << (m_sourceNode ? std::to_string(m_sourceNode->index()) : "nullptr") << " -> " //
                         << (m_targetNode ? std::to_string(m_targetNode->index()) : "nullptr");

    if (!TestMode::enabled()) {
        if (m_enableAnimations) {
            m_sourceDotSizeAnimation->stop();
            m_targetDotSizeAnimation->stop();
        }
        if (m_sourceNode) {
            m_sourceNode->removeGraphicsEdge(*this);
        }
        if (m_targetNode) {
            m_targetNode->removeGraphicsEdge(*this);
        }
    } else {
        TestMode::logDisabledCode("Edge destructor");
    }
}
