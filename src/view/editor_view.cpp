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

#include <QApplication>
#include <QGraphicsItem>
#include <QGraphicsSimpleTextItem>
#include <QLabel>
#include <QMimeData>
#include <QMouseEvent>
#include <QRectF>
#include <QRubberBand>
#include <QStatusBar>
#include <QString>
#include <QTransform>

#include "editor_view.hpp"

#include "../application/application_service.hpp"
#include "../application/control_strategy.hpp"
#include "../application/service_container.hpp"
#include "../application/settings_proxy.hpp"
#include "../common/constants.hpp"
#include "../common/utils.hpp"
#include "../domain/mind_map_data.hpp"
#include "item_filter.hpp"
#include "magic_zoom.hpp"
#include "menus/edge_context_menu.hpp"
#include "mouse_action.hpp"
#include "scene_items/edge.hpp"
#include "scene_items/edge_text_edit.hpp"
#include "scene_items/node.hpp"
#include "scene_items/node_handle.hpp"
#include "scene_items/scene_item_base.hpp"
#include "widgets/status_label.hpp"

#include "simple_logger.hpp"

#include <unordered_set>

using juzzlin::L;

EditorView::EditorView()
  : m_edgeContextMenu(new Menus::EdgeContextMenu(this))
  , m_mainContextMenu(new Menus::MainContextMenu(this, m_grid))
  , m_controlStrategy(SC::instance().controlStrategy())
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    setMouseTracking(true);

    setRenderHint(QPainter::Antialiasing);

    // Forward signals from main context menu
    connect(m_mainContextMenu, &Menus::MainContextMenu::actionTriggered, this, &EditorView::actionTriggered);
    connect(m_mainContextMenu, &Menus::MainContextMenu::newNodeRequested, this, &EditorView::newNodeRequested);

    m_updateShadowEffectsOnZoomTimer.setInterval(Constants::View::SHADOW_EFFECT_OPTIMIZATION_UPDATE_DELAY_MS);
    m_updateShadowEffectsOnZoomTimer.setSingleShot(true);
    connect(&m_updateShadowEffectsOnZoomTimer, &QTimer::timeout, this, &EditorView::updateShadowEffectsBasedOnItemVisiblity);
}

const Grid & EditorView::grid() const
{
    return m_grid;
}

void EditorView::finishRubberBand()
{
    const auto selectionRectangle = QRectF { mapToScene(m_rubberBand->geometry().topLeft()), mapToScene(m_rubberBand->geometry().bottomRight()) };
    if (!SC::instance().applicationService()->setNodeRectangleSelection(selectionRectangle)) {
        // No nodes were within the rectangle => clear the whole selection group.
        SC::instance().applicationService()->clearNodeSelectionGroup();
    }
    if (!SC::instance().applicationService()->setEdgeRectangleSelection(selectionRectangle)) {
        // No edges were within the rectangle => clear the whole selection group.
        SC::instance().applicationService()->clearEdgeSelectionGroup();
    }
    m_rubberBand->hide();
}

void EditorView::handleMousePressEventOnBackground(QMouseEvent & event)
{
    if (m_controlStrategy->rubberBandInitiated(event)) {
        initiateRubberBand();
    } else if (m_controlStrategy->backgroundDragInitiated(event)) {
        initiateBackgroundDrag();
    } else if (m_controlStrategy->secondaryButtonClicked(event)) {
        openMainContextMenu(Menus::MainContextMenu::Mode::Background);
    }
}

bool EditorView::handleMousePressEventOnEdge(QMouseEvent & event, EdgeR edge)
{
    if (m_controlStrategy->secondaryButtonClicked(event)) {
        handleSecondaryButtonClickOnEdge(edge);
        return true;
    }
    return false;
}

void EditorView::handleMousePressEventOnNode(QMouseEvent & event, NodeR node)
{
    if (node.index() != -1) // Prevent right-click on the drag node
    {
        if (m_controlStrategy->secondaryButtonClicked(event)) {
            handleSecondaryButtonClickOnNode(node);
        } else if (m_controlStrategy->primaryButtonClicked(event)) {
            handlePrimaryButtonClickOnNode(node);
        }
    }
}

void EditorView::handleMousePressEventOnNodeHandle(QMouseEvent & event, SceneItems::NodeHandle & nodeHandle)
{
    if (isModifierPressed()) {
        return;
    }

    if (m_controlStrategy->primaryButtonClicked(event)) {
        handlePrimaryButtonClickOnNodeHandle(nodeHandle);
    }
}

void EditorView::handlePrimaryButtonClickOnNode(NodeR node)
{
    if (isModifierPressed()) {
        // User is selecting a node
        SC::instance().applicationService()->toggleNodeInSelectionGroup(node);
    } else {
        // Clear selection group if the node is not in it
        if (SC::instance().applicationService()->nodeSelectionGroupSize() && !SC::instance().applicationService()->isInSelectionGroup(node)) {
            SC::instance().applicationService()->clearNodeSelectionGroup();
        }

        // User is initiating a node move drag
        SC::instance().applicationService()->initiateNodeDrag(node);
    }
}

void EditorView::handlePrimaryButtonClickOnNodeHandle(SceneItems::NodeHandle & nodeHandle)
{
    if (!nodeHandle.parentNode().selected()) {
        SC::instance().applicationService()->clearNodeSelectionGroup();
    }

    switch (nodeHandle.role()) {
    case SceneItems::NodeHandle::Role::ConnectOrCreate:
        SC::instance().applicationService()->initiateNewNodeDrag(nodeHandle);
        break;
    case SceneItems::NodeHandle::Role::Move:
        SC::instance().applicationService()->initiateNodeDrag(nodeHandle.parentNode());
        break;
    case SceneItems::NodeHandle::Role::NodeColor:
        SC::instance().applicationService()->addNodeToSelectionGroup(nodeHandle.parentNode(), true);
        emit actionTriggered(StateMachine::Action::NodeColorChangeRequested);
        break;
    case SceneItems::NodeHandle::Role::TextColor:
        SC::instance().applicationService()->addNodeToSelectionGroup(nodeHandle.parentNode(), true);
        emit actionTriggered(StateMachine::Action::TextColorChangeRequested);
        break;
    }
}

void EditorView::handleSecondaryButtonClickOnEdge(EdgeR edge)
{
    if (!edge.selected()) {
        SC::instance().applicationService()->clearEdgeSelectionGroup();
    }

    SC::instance().applicationService()->addEdgeToSelectionGroup(edge, true);

    openEdgeContextMenu();

    SC::instance().applicationService()->unselectImplicitlySelectedEdges();
}

void EditorView::handleSecondaryButtonClickOnNode(NodeR node)
{
    if (!node.selected()) {
        SC::instance().applicationService()->clearNodeSelectionGroup();
    }

    SC::instance().applicationService()->addNodeToSelectionGroup(node, true);

    openMainContextMenu(Menus::MainContextMenu::Mode::Node);

    SC::instance().applicationService()->unselectImplicitlySelectedNodes();
}

void EditorView::initiateBackgroundDrag()
{
    juzzlin::L().debug() << "Initiating background drag";

    SC::instance().applicationService()->mouseAction().setSourceNode(nullptr, MouseAction::Action::Scroll);

    setDragMode(ScrollHandDrag);
}

void EditorView::initiateRubberBand()
{
    juzzlin::L().debug() << "Initiating rubber band";

    SC::instance().applicationService()->mouseAction().setRubberBandOrigin(m_clickedPos);
    if (!m_rubberBand) {
        m_rubberBand = new QRubberBand { QRubberBand::Rectangle, this };
    }
    m_rubberBand->setGeometry(QRect { m_clickedPos, QSize {} });
    m_rubberBand->show();
}

bool EditorView::isModifierPressed() const
{
    return QGuiApplication::queryKeyboardModifiers().testFlag(Qt::ControlModifier) || QGuiApplication::queryKeyboardModifiers().testFlag(Qt::ShiftModifier);
}

void EditorView::mouseDoubleClickEvent(QMouseEvent * event)
{
    const auto clickedScenePos = mapToScene(event->pos());
    if (const auto result = ItemFilter::getFirstItemAtPosition(*scene(), clickedScenePos, Constants::View::CLICK_TOLERANCE); result.success) {
        if (result.node) {
            juzzlin::L().debug() << "Node double-clicked";
            zoomToFit(MagicZoom::calculateRectangleByItems({ result.node }, false));
        } else if (result.nodeTextEdit) {
            if (const auto node = dynamic_cast<NodeP>(result.nodeTextEdit->parentItem()); node) {
                juzzlin::L().debug() << "Node text edit double-clicked";
                zoomToFit(MagicZoom::calculateRectangleByItems({ node }, false));
            }
        }
    } else {
        emit newNodeRequested(clickedScenePos);
    }
}

void EditorView::mouseMoveEvent(QMouseEvent * event)
{
    m_pos = event->pos();
    m_mappedPos = mapToScene(event->pos());
    SC::instance().applicationService()->mouseAction().setMappedPos(m_mappedPos);

    using SceneItems::Node;

    if (Node::lastHoveredNode()) {
        const auto hhd = Constants::Node::HIDE_HANDLES_DISTANCE;
        if (m_mappedPos.x() > Node::lastHoveredNode()->pos().x() + hhd + Node::lastHoveredNode()->boundingRect().width() / 2 || //
            m_mappedPos.x() < Node::lastHoveredNode()->pos().x() - hhd - Node::lastHoveredNode()->boundingRect().width() / 2 || //
            m_mappedPos.y() > Node::lastHoveredNode()->pos().y() + hhd + Node::lastHoveredNode()->boundingRect().height() / 2 || //
            m_mappedPos.y() < Node::lastHoveredNode()->pos().y() - hhd - Node::lastHoveredNode()->boundingRect().height() / 2) {
            Node::lastHoveredNode()->setHandlesVisible(false);
        }
    }

    switch (SC::instance().applicationService()->mouseAction().action()) {
    case MouseAction::Action::None:
        break;
    case MouseAction::Action::MoveNode:
        if (const auto node = SC::instance().applicationService()->mouseAction().sourceNode()) {
            if (SC::instance().applicationService()->nodeSelectionGroupSize()) {
                SC::instance().applicationService()->moveSelectionGroup(*node, m_grid.snapToGrid(m_mappedPos - SC::instance().applicationService()->mouseAction().sourcePosOnNode()));
            } else {
                node->setLocation(m_grid.snapToGrid(m_mappedPos - SC::instance().applicationService()->mouseAction().sourcePosOnNode()));
            }
        }
        break;
    case MouseAction::Action::CreateOrConnectNode: {
        showDummyDragNode(true);
        showDummyDragEdge(true);
        m_dummyDragNode->setPos(m_grid.snapToGrid(m_mappedPos - SC::instance().applicationService()->mouseAction().sourcePosOnNode()));
        m_dummyDragEdge->updateLine();
        SC::instance().applicationService()->mouseAction().sourceNode()->setHandlesVisible(false);

        // This is needed to clear implicitly "selected" connection candidate nodes when hovering the dummy drag node on other nodes
        SC::instance().applicationService()->unselectSelectedNode();
        SC::instance().applicationService()->clearNodeSelectionGroup();

        m_connectionTargetNode = nullptr;

        // TODO: Use items() to pre-filter the nodes
        if (auto && node = SC::instance().applicationService()->getBestOverlapNode(*m_dummyDragNode)) {
            node->setSelected(true);
            m_connectionTargetNode = node;
        }
    } break;
    case MouseAction::Action::RubberBand:
        updateRubberBand();
        break;
    case MouseAction::Action::Scroll:
        removeShadowEffectsDuringDrag();
        break;
    }

    QGraphicsView::mouseMoveEvent(event);
}

void EditorView::mousePressEvent(QMouseEvent * event)
{
    m_clickedPos = event->pos();
    const auto clickedScenePos = mapToScene(m_clickedPos);
    SC::instance().applicationService()->mouseAction().setClickedScenePos(clickedScenePos);

    if (const auto result = ItemFilter::getFirstItemAtPosition(*scene(), clickedScenePos, Constants::View::CLICK_TOLERANCE); result.success) {
        if (result.edge) {
            juzzlin::L().debug() << "Edge pressed";
            if (!handleMousePressEventOnEdge(*event, *result.edge)) {
                juzzlin::L().debug() << "Background pressed via edge";
                handleMousePressEventOnBackground(*event);
            }
        } else if (result.nodeHandle) {
            juzzlin::L().debug() << "Node handle pressed";
            handleMousePressEventOnNodeHandle(*event, *result.nodeHandle);
        } else if (result.node) {
            juzzlin::L().debug() << "Node pressed";
            handleMousePressEventOnNode(*event, *result.node);
            if (isModifierPressed()) { // User was just selecting
                result.node->setTextInputActive(false);
                return;
            }
            // This hack enables edge context menu even if user clicks on the edge text edit.
        } else if (result.edgeTextEdit) {
            if (m_controlStrategy->secondaryButtonClicked(*event)) {
                if (const auto edge = result.edgeTextEdit->edge(); edge) {
                    juzzlin::L().debug() << "Edge text edit pressed";
                    handleMousePressEventOnEdge(*event, *edge);
                    return;
                }
            }
            // This hack enables node context menu even if user clicks on the node text edit.
        } else if (result.nodeTextEdit) {
            if (m_controlStrategy->secondaryButtonClicked(*event) || (m_controlStrategy->primaryButtonClicked(*event) && isModifierPressed())) {
                if (const auto node = dynamic_cast<NodeP>(result.nodeTextEdit->parentItem()); node) {
                    juzzlin::L().debug() << "Node text edit pressed";
                    handleMousePressEventOnNode(*event, *node);
                    node->setTextInputActive(false);
                    return;
                }
            }
        }
    } else {
        juzzlin::L().debug() << "Background pressed";
        handleMousePressEventOnBackground(*event);
    }

    QGraphicsView::mousePressEvent(event);
}

void EditorView::mouseReleaseEvent(QMouseEvent * event)
{
    if (event->button() == Qt::MiddleButton) {
        switch (SC::instance().applicationService()->mouseAction().action()) {
        case MouseAction::Action::RubberBand:
            finishRubberBand();
            break;
        default:
            break;
        }
    } else if (m_controlStrategy->primaryButtonClicked(*event)) {
        switch (SC::instance().applicationService()->mouseAction().action()) {
        case MouseAction::Action::None:
            // This can happen if the user deletes the drag node while connecting nodes or creating a new node.
            // In this case the action is just aborted.
            if (m_connectionTargetNode) {
                m_connectionTargetNode->setSelected(false);
                m_connectionTargetNode = nullptr;
            }
            break;
        case MouseAction::Action::MoveNode:
            SC::instance().applicationService()->adjustSceneRect();
            break;
        case MouseAction::Action::CreateOrConnectNode:
            if (const auto sourceNode = SC::instance().applicationService()->mouseAction().sourceNode()) {
                if (m_connectionTargetNode) {
                    SC::instance().applicationService()->addEdge(*sourceNode, *m_connectionTargetNode);
                    m_connectionTargetNode->setSelected(false);
                    m_connectionTargetNode = nullptr;
                } else {
                    SC::instance().applicationService()->createAndAddNode(sourceNode->index(), m_grid.snapToGrid(m_mappedPos - SC::instance().applicationService()->mouseAction().sourcePosOnNode()));
                }
                resetDummyDragItems();
            }
            break;
        case MouseAction::Action::RubberBand:
            finishRubberBand();
            break;
        case MouseAction::Action::Scroll:
            setDragMode(NoDrag);
            updateShadowEffectsBasedOnItemVisiblity();
            break;
        }

        QApplication::restoreOverrideCursor();
    }

    SC::instance().applicationService()->mouseAction().clear();

    QGraphicsView::mouseReleaseEvent(event);
}

void EditorView::openEdgeContextMenu()
{
    m_edgeContextMenu->exec(mapToGlobal(m_clickedPos));
}

void EditorView::openMainContextMenu(Menus::MainContextMenu::Mode mode)
{
    m_mainContextMenu->setMode(mode);
    m_mainContextMenu->exec(mapToGlobal(m_clickedPos));
}

void EditorView::resetDummyDragItems()
{
    // Ensure new dummy nodes and related graphics items are created (again) when needed.
    m_dummyDragEdge.reset();
    m_dummyDragNode.reset();

    L().debug() << "Dummy drag item reset";
}

void EditorView::showDummyDragEdge(bool show)
{
    if (const auto sourceNode = SC::instance().applicationService()->mouseAction().sourceNode()) {
        if (!m_dummyDragEdge) {
            L().debug() << "Creating a new dummy drag edge";
            m_dummyDragEdge = std::make_unique<SceneItems::Edge>(sourceNode, m_dummyDragNode.get(), false, false);
            m_dummyDragEdge->setOpacity(0.5);
            scene()->addItem(m_dummyDragEdge.get());
        } else {
            m_dummyDragEdge->setSourceNode(*sourceNode);
            m_dummyDragEdge->setTargetNode(*m_dummyDragNode);
        }
    }

    m_dummyDragEdge->setArrowSize(m_arrowSize);
    m_dummyDragEdge->setColor(m_edgeColor);
    m_dummyDragEdge->setEdgeWidth(m_edgeWidth);
    m_dummyDragEdge->setVisible(show);
}

void EditorView::showDummyDragNode(bool show)
{
    if (!m_dummyDragNode) {
        L().debug() << "Creating a new dummy drag node";
        m_dummyDragNode = std::make_unique<SceneItems::Node>();
        scene()->addItem(m_dummyDragNode.get());
    }

    m_dummyDragNode->setCornerRadius(m_cornerRadius);
    m_dummyDragNode->setOpacity(Constants::View::DRAG_NODE_OPACITY);
    m_dummyDragNode->setVisible(show);
}

void EditorView::updateScale()
{
    QTransform transform;
    transform.scale(m_scale, m_scale);
    setTransform(transform);

    m_updateShadowEffectsOnZoomTimer.stop();
    m_updateShadowEffectsOnZoomTimer.start();
}

void EditorView::removeShadowEffectsDuringDrag()
{
    if (SC::instance().settingsProxy()->optimizeShadowEffects()) {

        if (!m_shadowEffectsDuringDragRemoved) {
            m_shadowEffectsDuringDragRemoved = true;

            // Remove shadow effects from edges that are not completely visible while dragging.
            // We can assume that the effects of off-screen items have already been removed in
            // updateShadowEffectsBasedOnItemVisiblity().
            const auto mappedViewRect = mapToScene(rect());
            for (auto && item : scene()->items(mappedViewRect, Qt::IntersectsItemShape)) {
                if (const auto edge = dynamic_cast<SceneItems::Edge *>(item); edge) {
                    if (!mappedViewRect.boundingRect().contains(edge->sceneBoundingRect().toRect())) {
                        edge->enableShadowEffect(false);
                    }
                }
            }
        }
    }
}

void EditorView::updateShadowEffectsBasedOnItemVisiblity()
{
    std::unordered_set<SceneItems::SceneItemBase *> enabledItems;

    const int glitchMargin = rect().width() / Constants::View::SHADOW_EFFECT_OPTIMIZATION_MARGIN_FRACTION;
    for (auto && item : scene()->items(mapToScene(rect().adjusted(-glitchMargin, -glitchMargin, glitchMargin, glitchMargin)))) {
        if (const auto sceneItem = dynamic_cast<SceneItems::SceneItemBase *>(item); sceneItem) {
            sceneItem->enableShadowEffect(true);
            enabledItems.insert(sceneItem);
        }
    }

    if (SC::instance().settingsProxy()->optimizeShadowEffects()) {
        for (auto && item : scene()->items()) {
            if (const auto sceneItem = dynamic_cast<SceneItems::SceneItemBase *>(item); sceneItem) {
                if (!enabledItems.count(sceneItem)) {
                    sceneItem->enableShadowEffect(false);
                }
            }
        }
    }

    m_shadowEffectsDuringDragRemoved = false;
}

void EditorView::updateRubberBand()
{
    m_rubberBand->setGeometry(QRect(SC::instance().applicationService()->mouseAction().rubberBandOrigin().toPoint(), m_pos.toPoint()).normalized());
}

void EditorView::restoreZoom()
{
    if (m_savedZoom.has_value()) {
        juzzlin::L().debug() << "Restoring zoom";
        scene()->setSceneRect(m_savedZoom->sceneRect);
        m_scale = m_savedZoom->scale;
        updateScale();
        centerOn(m_savedZoom->viewCenter);
        m_savedZoom.reset();
    }
}

void EditorView::saveZoom()
{
    juzzlin::L().debug() << "Saving zoom";
    m_savedZoom = { scene()->sceneRect(), mapToScene(viewport()->rect().center()), m_scale };
}

void EditorView::setArrowSize(double arrowSize)
{
    m_arrowSize = arrowSize;
}

void EditorView::setCornerRadius(int cornerRadius)
{
    m_cornerRadius = cornerRadius;
}

void EditorView::setEdgeColor(const QColor & edgeColor)
{
    m_edgeColor = edgeColor;
}

void EditorView::setEdgeWidth(double edgeWidth)
{
    m_edgeWidth = edgeWidth;
}

void EditorView::setGridSize(int size)
{
    m_grid.setSize(size);
    if (scene()) {
        scene()->update();
    }
}

void EditorView::setGridColor(const QColor & gridColor)
{
    SC::instance().applicationService()->mindMapData()->setGridColor(gridColor);
}

void EditorView::setGridVisible(bool visible)
{
    m_gridVisible = visible;
    if (scene()) {
        scene()->update();
    }
}

void EditorView::showStatusText(QString statusText)
{
    const auto label = new Widgets::StatusLabel(*this, statusText);
    label->show();
    label->updateGeometryKakkaPissa();
}

void EditorView::wheelEvent(QWheelEvent * event)
{
    zoom(event->angleDelta().y() > 0 ? Constants::View::ZOOM_SENSITIVITY : 1.0 / Constants::View::ZOOM_SENSITIVITY);
}

void EditorView::dropEvent(QDropEvent * event)
{
    const auto urls = event->mimeData()->urls();
    if (urls.isEmpty()) {
        return;
    }
    if (const auto fileName = urls.first().toLocalFile(); !fileName.isEmpty()) {
        m_dropFile = fileName;
        emit actionTriggered(StateMachine::Action::DropFileSelected);
    }
}

void EditorView::dragMoveEvent(QDragMoveEvent * event)
{
    event->setDropAction(Qt::CopyAction);
    event->accept();
}

void EditorView::zoom(double amount)
{
    const auto mappedSceneRect = QRectF(
      mapFromScene(scene()->sceneRect().topLeft()),
      mapFromScene(scene()->sceneRect().bottomRight()));
    juzzlin::L().debug() << "Current scale: " << m_scale;
    juzzlin::L().debug() << "Mapped scene rectangle width: " << mappedSceneRect.width();
    juzzlin::L().debug() << "View rectangle width: " << rect().width();
    const auto testScale = amount * amount;
    if (amount > 1.0 || (mappedSceneRect.width() * testScale > rect().width() && mappedSceneRect.height() * testScale > rect().height())) {
        m_scale *= amount;
        m_scale = std::min(m_scale, Constants::View::ZOOM_MAX);
        m_scale = std::max(m_scale, Constants::View::ZOOM_MIN);
        updateScale();
    } else {
        juzzlin::L().debug() << "Zoom end";
    }
}

void EditorView::zoomToFit(QRectF nodeBoundingRect)
{
    const double nodeAspect = nodeBoundingRect.height() / nodeBoundingRect.width();
    if (const double viewAspect = static_cast<double>(rect().height()) / rect().width(); viewAspect < 1.0) {
        if (nodeAspect < viewAspect) {
            m_scale = static_cast<double>(rect().width()) / nodeBoundingRect.width();
        } else {
            m_scale = static_cast<double>(rect().height()) / nodeBoundingRect.height();
        }
    } else {
        if (nodeAspect > viewAspect) {
            m_scale = static_cast<double>(rect().height()) / nodeBoundingRect.height();
        } else {
            m_scale = static_cast<double>(rect().width()) / nodeBoundingRect.width();
        }
    }

    updateScale();

    centerOn(nodeBoundingRect.center());

    m_nodeBoundingRect = nodeBoundingRect;
}

QString EditorView::dropFile() const
{
    return this->m_dropFile;
}

void EditorView::drawBackground(QPainter * painter, const QRectF & sceneRect)
{
    painter->save();
    painter->fillRect(sceneRect, backgroundBrush());
    drawGrid(*painter, sceneRect);
    painter->restore();
}

void EditorView::drawGrid(QPainter & painter, const QRectF & sceneRect)
{
    // Draw grid only if we have enough details visible versus the grid size. Otherwise fake it with a colored rectangle.
    // The idea is to prevent situations where we have zoomed out so that there are ridiculous amount of grid lines
    // visible - even multiple lines per pixel.
    if (m_gridVisible && m_grid.size()) {
        const int virtualLineWidth = 5;
        if (const auto detailRect = mapFromScene(QRectF { 0, 0, static_cast<double>(m_grid.size()), 1 }).boundingRect(); detailRect.width() > virtualLineWidth) {
            if (auto && lines = m_grid.calculateLines(sceneRect); !lines.empty()) {
                painter.setPen(SC::instance().applicationService()->mindMapData()->gridColor());
                painter.drawLines(m_grid.calculateLines(sceneRect).data(), static_cast<int>(lines.size()));
            }
        } else {
            const double balance = static_cast<double>(detailRect.width()) / virtualLineWidth;
            painter.fillRect(sceneRect, Utils::mixedColor(backgroundBrush().color(), SC::instance().applicationService()->mindMapData()->gridColor(), balance));
        }
    }
}

EditorView::~EditorView() = default;
