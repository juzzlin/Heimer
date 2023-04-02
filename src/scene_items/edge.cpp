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

#include "edge_dot.hpp"
#include "edge_text_edit.hpp"
#include "graphics_factory.hpp"
#include "layers.hpp"
#include "node.hpp"

#include "../constants.hpp"

#include "../core/graph.hpp"
#include "../core/settings_proxy.hpp"
#include "../core/shadow_effect_params.hpp"
#include "../core/single_instance_container.hpp"
#include "../core/test_mode.hpp"

#include "simple_logger.hpp"

#include <QBrush>
#include <QFont>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QPen>
#include <QPropertyAnimation>
#include <QTimer>
#include <QVector2D>

#include <QtMath>

#include <cassert>
#include <cmath>

using Core::TestMode;

namespace SceneItems {

Edge::Edge(NodeP sourceNode, NodeP targetNode, bool enableAnimations, bool enableLabel)
  : m_settingsProxy(Core::SingleInstanceContainer::instance().settingsProxy())
  , m_edgeModel(std::make_unique<EdgeModel>(m_settingsProxy.reversedEdgeDirection(),
                                            EdgeModel::Style { m_settingsProxy.edgeArrowMode() }))
  , m_sourceNode(sourceNode)
  , m_targetNode(targetNode)
  , m_enableAnimations(enableAnimations)
  , m_enableLabel(enableLabel)
  , m_sourceDot(enableAnimations ? new EdgeDot(this) : nullptr)
  , m_targetDot(enableAnimations ? new EdgeDot(this) : nullptr)
  , m_label(enableLabel ? new EdgeTextEdit(this) : nullptr)
  , m_dummyLabel(enableLabel ? new EdgeTextEdit(this) : nullptr)
  , m_line(new QGraphicsLineItem(this))
  , m_arrowheadL0(new QGraphicsLineItem(this))
  , m_arrowheadR0(new QGraphicsLineItem(this))
  , m_arrowheadL1(new QGraphicsLineItem(this))
  , m_arrowheadR1(new QGraphicsLineItem(this))
  , m_sourceDotSizeAnimation(enableAnimations ? new QPropertyAnimation(m_sourceDot, "scale", this) : nullptr)
  , m_targetDotSizeAnimation(enableAnimations ? new QPropertyAnimation(m_targetDot, "scale", this) : nullptr)
{
    setAcceptHoverEvents(true && enableAnimations);

    setGraphicsEffect(GraphicsFactory::createDropShadowEffect(m_settingsProxy.shadowEffect(), false));

    setZValue(static_cast<int>(Layers::Edge));

    initDots();

    if (m_enableLabel) {
        m_label->setZValue(static_cast<int>(Layers::EdgeLabel));
        m_label->setBackgroundColor(Constants::Edge::LABEL_COLOR);
        connect(m_label, &TextEdit::textChanged, this, [=](const QString & text) {
            updateLabel();
            m_edgeModel->text = text;
        });

        connect(m_label, &TextEdit::undoPointRequested, this, &Edge::undoPointRequested);

        m_dummyLabel->setZValue(static_cast<int>(Layers::EdgeDummyLabel));
        m_dummyLabel->setAcceptHoverEvents(false);
        m_dummyLabel->setBackgroundColor(Constants::Edge::LABEL_COLOR);
        m_dummyLabel->setText(tr("..."));
        m_dummyLabel->setEnabled(false);

        connect(m_label, &EdgeTextEdit::hoverEntered, this, [=] {
            setLabelVisible(true, EdgeTextEdit::VisibilityChangeReason::Focused);
        });

        connect(m_label, &EdgeTextEdit::visibilityTimeout, this, [=] {
            setLabelVisible(false);
        });

        m_labelVisibilityTimer.setSingleShot(true);
        m_labelVisibilityTimer.setInterval(Constants::Edge::LABEL_DURATION);

        connect(&m_labelVisibilityTimer, &QTimer::timeout, this, [=] {
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

    setLabelVisible(true, EdgeTextEdit::VisibilityChangeReason::Focused);

    QGraphicsItem::hoverEnterEvent(event);
}

void Edge::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
    m_labelVisibilityTimer.start();

    QGraphicsItem::hoverLeaveEvent(event);
}

QPen Edge::buildPen(bool ignoreDashSetting) const
{
    QPen pen { QBrush { QColor { m_color.red(), m_color.green(), m_color.blue() } }, m_edgeModel->style.edgeWidth };
    pen.setCapStyle(Qt::PenCapStyle::RoundCap);
    if (!ignoreDashSetting && m_edgeModel->style.dashedLine) {
        pen.setDashPattern(Constants::Edge::DASH_PATTERN);
    }
    return pen;
}

void Edge::copyData(EdgeCR other)
{
    *m_edgeModel = *other.m_edgeModel;

    setText(other.m_edgeModel->text); // Update text to the label component
}

void Edge::changeFont(const QFont & font)
{
    if (m_enableLabel) {
        // Handle size and family separately to maintain backwards compatibility
        QFont newFont(font);
        if (m_label->font().pointSize() >= 0) {
            newFont.setPointSize(m_label->font().pointSize());
        }
        m_label->setFont(newFont);
        m_dummyLabel->setFont(newFont);
    }
}

bool Edge::dashedLine() const
{
    return m_edgeModel->style.dashedLine;
}

void Edge::enableShadowEffect(bool enable)
{
    GraphicsFactory::updateDropShadowEffect(graphicsEffect(), m_settingsProxy.shadowEffect(), m_selected, !enable);
    update();
}

double Edge::length() const
{
    return m_line->line().length();
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
    if (m_enableLabel) {
        // Note: We need to test for scene(), because sceneBoundingRect() will crash if the item has not been added to any scene (yet).
        const bool isEnoughSpaceForLabel = m_label->scene() && !m_label->sceneBoundingRect().intersects(sourceNode().sceneBoundingRect()) && //
          !m_label->sceneBoundingRect().intersects(targetNode().sceneBoundingRect());
        const bool dummyLabelTextIsShoterThanLabelText = m_dummyLabel->text().length() < m_label->text().length();
        const bool isEnoughSpaceForDummyLabel = m_dummyLabel->scene() && !m_dummyLabel->sceneBoundingRect().intersects(sourceNode().sceneBoundingRect()) && //
          !m_dummyLabel->sceneBoundingRect().intersects(targetNode().sceneBoundingRect());
        switch (vcr) {
        case EdgeTextEdit::VisibilityChangeReason::AvailableSpaceChanged: {
            // Toggle visibility according to space available if geometry changed
            const bool isLabelVisible = isEnoughSpaceForLabel && !m_label->text().isEmpty();
            m_label->setVisible(isLabelVisible);
            m_dummyLabel->setVisible(!isLabelVisible && isEnoughSpaceForDummyLabel && dummyLabelTextIsShoterThanLabelText);
        } break;
        case EdgeTextEdit::VisibilityChangeReason::Explicit: {
            m_label->setVisible(visible);
            m_dummyLabel->setVisible(visible);
        } break;
        case EdgeTextEdit::VisibilityChangeReason::Focused: {
            if (visible) {
                m_label->setVisible(true);
                m_label->setParentItem(nullptr);
                m_label->setGraphicsEffect(GraphicsFactory::createDropShadowEffect(m_settingsProxy.shadowEffect(), false));
                m_dummyLabel->setVisible(false);
            }
        } break;
        case EdgeTextEdit::VisibilityChangeReason::Timeout: {
            if (!visible) {
                if ((m_label->text().isEmpty() || (!m_label->text().isEmpty() && !isEnoughSpaceForLabel)) && !m_label->hasFocus()) {
                    m_label->setVisible(false);
                    m_dummyLabel->setVisible(isEnoughSpaceForDummyLabel && dummyLabelTextIsShoterThanLabelText);
                }
            }
        } break;
        }
    }
}

void Edge::setEdgeWidth(double edgeWidth)
{
    m_edgeModel->style.edgeWidth = edgeWidth;

    updateLine();
}

void Edge::setArrowMode(EdgeModel::ArrowMode arrowMode)
{
    m_edgeModel->style.arrowMode = arrowMode;
    if (!TestMode::enabled()) {
        updateLine();
    } else {
        TestMode::logDisabledCode("Update line after arrow mode change");
    }
}

void Edge::setArrowSize(double arrowSize)
{
    m_edgeModel->style.arrowSize = arrowSize;

    updateLine();
}

void Edge::setColor(const QColor & color)
{
    m_color = color;

    updateLine();
}

void Edge::setDashedLine(bool enable)
{
    m_edgeModel->style.dashedLine = enable;
    if (!TestMode::enabled()) {
        updateLine();
    } else {
        TestMode::logDisabledCode("Set dashed line");
    }
}

void Edge::setText(const QString & text)
{
    m_edgeModel->text = text;
    if (m_enableLabel) {
        m_label->setText(text);
        setLabelVisible(!text.isEmpty());
    }
}

void Edge::setTextSize(int textSize)
{
    if (m_enableLabel && textSize > 0) {
        m_label->setTextSize(textSize);
        m_dummyLabel->setTextSize(textSize);
    }
}

void Edge::setReversed(bool reversed)
{
    m_edgeModel->reversed = reversed;

    updateArrowhead();
}

void Edge::setSelected(bool selected)
{
    m_selected = selected;
    GraphicsFactory::updateDropShadowEffect(graphicsEffect(), m_settingsProxy.shadowEffect(), selected);
    if (m_label && m_label->parentItem() != this) {
        GraphicsFactory::updateDropShadowEffect(m_label->graphicsEffect(), m_settingsProxy.shadowEffect(), selected);
    }
    update();
}

void Edge::setShadowEffect(const ShadowEffectParams & params)
{
    GraphicsFactory::updateDropShadowEffect(graphicsEffect(), params, m_selected);
    if (m_label && m_label->parentItem() != this) {
        GraphicsFactory::updateDropShadowEffect(m_label->graphicsEffect(), m_settingsProxy.shadowEffect(), m_selected);
    }
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

    const auto reversed = m_edgeModel->reversed;
    const auto point0 = reversed ? m_line->line().p1() : m_line->line().p2();
    const auto angle0 = reversed ? -m_line->line().angle() + 180 : -m_line->line().angle();
    const auto point1 = reversed ? m_line->line().p2() : m_line->line().p1();
    const auto angle1 = reversed ? -m_line->line().angle() : -m_line->line().angle() + 180;

    QLineF lineL0;
    QLineF lineR0;
    QLineF lineL1;
    QLineF lineR1;

    switch (m_edgeModel->style.arrowMode) {
    case EdgeModel::ArrowMode::Single: {
        lineL0.setP1(point0);
        const auto angleL = qDegreesToRadians(angle0 + Constants::Edge::ARROW_OPENING);
        lineL0.setP2(point0 + QPointF(std::cos(angleL), std::sin(angleL)) * m_edgeModel->style.arrowSize);
        lineR0.setP1(point0);
        const auto angleR = qDegreesToRadians(angle0 - Constants::Edge::ARROW_OPENING);
        lineR0.setP2(point0 + QPointF(std::cos(angleR), std::sin(angleR)) * m_edgeModel->style.arrowSize);
        m_arrowheadL0->setLine(lineL0);
        m_arrowheadR0->setLine(lineR0);
        m_arrowheadL0->show();
        m_arrowheadR0->show();
        m_arrowheadL1->hide();
        m_arrowheadR1->hide();
        break;
    }
    case EdgeModel::ArrowMode::Double: {
        lineL0.setP1(point0);
        const auto angleL0 = qDegreesToRadians(angle0 + Constants::Edge::ARROW_OPENING);
        lineL0.setP2(point0 + QPointF(std::cos(angleL0), std::sin(angleL0)) * m_edgeModel->style.arrowSize);
        lineR0.setP1(point0);
        const auto angleR0 = qDegreesToRadians(angle0 - Constants::Edge::ARROW_OPENING);
        lineR0.setP2(point0 + QPointF(std::cos(angleR0), std::sin(angleR0)) * m_edgeModel->style.arrowSize);
        lineL1.setP1(point1);
        m_arrowheadL0->setLine(lineL0);
        m_arrowheadR0->setLine(lineR0);
        m_arrowheadL0->show();
        m_arrowheadR0->show();
        const auto angleL1 = qDegreesToRadians(angle1 + Constants::Edge::ARROW_OPENING);
        lineL1.setP2(point1 + QPointF(std::cos(angleL1), std::sin(angleL1)) * m_edgeModel->style.arrowSize);
        lineR1.setP1(point1);
        const auto angleR1 = qDegreesToRadians(angle1 - Constants::Edge::ARROW_OPENING);
        lineR1.setP2(point1 + QPointF(std::cos(angleR1), std::sin(angleR1)) * m_edgeModel->style.arrowSize);
        m_arrowheadL1->setLine(lineL1);
        m_arrowheadR1->setLine(lineR1);
        m_arrowheadL1->show();
        m_arrowheadR1->show();
        break;
    }
    case EdgeModel::ArrowMode::Hidden:
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
        const auto newRelativeSourcePos = m_line->line().p1() - sourceNode().pos();
        if (m_previousRelativeSourcePos != newRelativeSourcePos) {
            m_previousRelativeSourcePos = newRelativeSourcePos;
            m_sourceDotSizeAnimation->stop();
            m_sourceDotSizeAnimation->start();
        }

        // Update location of possibly active animation
        m_sourceDot->setPos(m_line->line().p1());

        // Trigger new animation if relative connection location has changed
        const auto newRelativeTargetPos = m_line->line().p2() - targetNode().pos();
        if (m_previousRelativeTargetPos != newRelativeTargetPos) {
            m_previousRelativeTargetPos = newRelativeTargetPos;
            m_targetDotSizeAnimation->stop();
            m_targetDotSizeAnimation->start();
        }

        // Update location of possibly active animation
        m_targetDot->setPos(m_line->line().p2());
    }
}

void Edge::updateLabel(LabelUpdateReason lur)
{
    if (m_enableLabel) {
        m_label->setPos((m_line->line().p1() + m_line->line().p2()) * 0.5 - QPointF(m_label->boundingRect().width(), m_label->boundingRect().height()) * 0.5);
        m_dummyLabel->setPos((m_line->line().p1() + m_line->line().p2()) * 0.5 - QPointF(m_dummyLabel->boundingRect().width(), m_dummyLabel->boundingRect().height()) * 0.5);
        // Toggle visibility according to space available if geometry changed
        if (lur == LabelUpdateReason::EdgeGeometryChanged) {
            setLabelVisible(m_label->isVisible(), EdgeTextEdit::VisibilityChangeReason::AvailableSpaceChanged);
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

void Edge::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    // Nothing to do as Edge is just a composite object
    Q_UNUSED(painter)
    Q_UNUSED(option)
    Q_UNUSED(widget)
}

void Edge::removeFromScene()
{
    restoreLabelParent();
    hide();
    if (scene()) {
        scene()->removeItem(this);
    }
}

bool Edge::reversed() const
{
    return m_edgeModel->reversed;
}

void Edge::restoreLabelParent()
{
    if (m_label) {
        m_label->setParentItem(this);
    }
}

EdgeModel::ArrowMode Edge::arrowMode() const
{
    return m_edgeModel->style.arrowMode;
}

QRectF Edge::boundingRect() const
{
    return m_line->boundingRect();
}

QString Edge::text() const
{
    return m_edgeModel->text;
}

void Edge::updateLine()
{
    m_line->setPen(buildPen());

    const auto nearestPoints = Node::getNearestEdgePoints(sourceNode(), targetNode());

    const auto p1 = nearestPoints.first.location + sourceNode().pos();
    QVector2D direction1(sourceNode().pos() - p1);
    direction1.normalize();

    const auto p2 = nearestPoints.second.location + targetNode().pos();
    QVector2D direction2(targetNode().pos() - p2);
    direction2.normalize();

    m_line->setLine(QLineF(
      p1 + (nearestPoints.first.isCorner ? Constants::Edge::CORNER_RADIUS_SCALE * (direction1 * static_cast<float>(sourceNode().cornerRadius())).toPointF() : QPointF { 0, 0 }),
      p2 + (nearestPoints.second.isCorner ? Constants::Edge::CORNER_RADIUS_SCALE * (direction2 * static_cast<float>(targetNode().cornerRadius())).toPointF() : QPointF { 0, 0 }) - //
        (direction2 * static_cast<float>(m_edgeModel->style.edgeWidth)).toPointF() * Constants::Edge::WIDTH_SCALE));

    updateDots();
    updateLabel(LabelUpdateReason::EdgeGeometryChanged);
    updateArrowhead();

    // Set correct origin for scale animations
    setTransformOriginPoint(m_line->line().center());
}

Edge::~Edge()
{
    juzzlin::L().trace() << "Deleting edge (" << (m_sourceNode ? std::to_string(m_sourceNode->index()) : "(none)") << ", " //
                         << (m_targetNode ? std::to_string(m_targetNode->index()) : "(none)") << ")";

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

} // namespace SceneItems
