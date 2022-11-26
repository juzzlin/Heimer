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
#include <QMimeData>
#include <QMouseEvent>
#include <QRectF>
#include <QRubberBand>
#include <QStatusBar>
#include <QString>
#include <QTransform>

#include "editor_view.hpp"

#include "constants.hpp"
#include "control_strategy.hpp"
#include "edge.hpp"
#include "edge_text_edit.hpp"
#include "graphics_factory.hpp"
#include "item_filter.hpp"
#include "magic_zoom.hpp"
#include "mediator.hpp"
#include "mind_map_data.hpp"
#include "mouse_action.hpp"
#include "node.hpp"
#include "node_action.hpp"
#include "node_handle.hpp"

#include "simple_logger.hpp"

#include "menus/edge_context_menu.hpp"

#include <cassert>
#include <cstdlib>

using juzzlin::L;

EditorView::EditorView(Mediator & mediator)
  : m_mediator(mediator)
  , m_edgeContextMenu(new Menus::EdgeContextMenu(this, m_mediator))
  , m_mainContextMenu(new Menus::MainContextMenu(this, m_mediator, m_grid))
  , m_controlStrategy(std::make_unique<ControlStrategy>())
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    setMouseTracking(true);

    setRenderHint(QPainter::Antialiasing);

    // Forward signals from main context menu
    connect(m_mainContextMenu, &Menus::MainContextMenu::actionTriggered, this, &EditorView::actionTriggered);
    connect(m_mainContextMenu, &Menus::MainContextMenu::newNodeRequested, this, &EditorView::newNodeRequested);
}

const Grid & EditorView::grid() const
{
    return m_grid;
}

void EditorView::finishRubberBand()
{
    if (!m_mediator.setRectagleSelection({ mapToScene(m_rubberBand->geometry().topLeft()), mapToScene(m_rubberBand->geometry().bottomRight()) })) {
        // No nodes were within the rectangle => clear the whole selection group.
        m_mediator.clearSelectionGroup();
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

void EditorView::handleMousePressEventOnEdge(QMouseEvent & event, EdgeR edge)
{
    if (m_controlStrategy->secondaryButtonClicked(event)) {
        handleSecondaryButtonClickOnEdge(edge);
    }
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

void EditorView::handleMousePressEventOnNodeHandle(QMouseEvent & event, NodeHandle & nodeHandle)
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
        m_mediator.toggleNodeInSelectionGroup(node);
    } else {
        // Clear selection group if the node is not in it
        if (m_mediator.selectionGroupSize() && !m_mediator.isInSelectionGroup(node)) {
            m_mediator.clearSelectionGroup();
        }

        // User is initiating a node move drag
        m_mediator.initiateNodeDrag(node);
    }
}

void EditorView::handlePrimaryButtonClickOnNodeHandle(NodeHandle & nodeHandle)
{
    if (!nodeHandle.parentNode().selected()) {
        m_mediator.clearSelectionGroup();
    }

    switch (nodeHandle.role()) {
    case NodeHandle::Role::ConnectOrCreate:
        m_mediator.initiateNewNodeDrag(nodeHandle);
        break;
    case NodeHandle::Role::Move:
        m_mediator.initiateNodeDrag(nodeHandle.parentNode());
        break;
    case NodeHandle::Role::NodeColor:
        m_mediator.addNodeToSelectionGroup(nodeHandle.parentNode());
        emit actionTriggered(StateMachine::Action::NodeColorChangeRequested);
        break;
    case NodeHandle::Role::TextColor:
        m_mediator.addNodeToSelectionGroup(nodeHandle.parentNode());
        emit actionTriggered(StateMachine::Action::TextColorChangeRequested);
        break;
    }
}

void EditorView::handleSecondaryButtonClickOnEdge(EdgeR edge)
{
    m_mediator.setSelectedEdge(&edge);

    openEdgeContextMenu();
}

void EditorView::handleSecondaryButtonClickOnNode(NodeR node)
{
    if (!node.selected()) {
        m_mediator.clearSelectionGroup();
    }

    m_mediator.addNodeToSelectionGroup(node);

    openMainContextMenu(Menus::MainContextMenu::Mode::Node);
}

void EditorView::initiateBackgroundDrag()
{
    juzzlin::L().debug() << "Initiating background drag..";

    m_mediator.setSelectedEdge(nullptr);
    m_mediator.unselectText();
    m_mediator.mouseAction().setSourceNode(nullptr, MouseAction::Action::Scroll);

    setDragMode(ScrollHandDrag);
}

void EditorView::initiateRubberBand()
{
    juzzlin::L().debug() << "Initiating rubber band..";

    m_mediator.mouseAction().setRubberBandOrigin(m_clickedPos);
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
            zoomToFit(MagicZoom::calculateRectangle({ result.node }, false));
        } else if (result.nodeTextEdit) {
            if (const auto node = dynamic_cast<NodeP>(result.nodeTextEdit->parentItem()); node) {
                juzzlin::L().debug() << "Node text edit double-clicked";
                zoomToFit(MagicZoom::calculateRectangle({ node }, false));
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
    m_mediator.mouseAction().setMappedPos(m_mappedPos);

    if (Node::lastHoveredNode()) {
        const auto hhd = Constants::Node::HIDE_HANDLES_DISTANCE;
        if (m_mappedPos.x() > Node::lastHoveredNode()->pos().x() + hhd + Node::lastHoveredNode()->boundingRect().width() / 2 || //
            m_mappedPos.x() < Node::lastHoveredNode()->pos().x() - hhd - Node::lastHoveredNode()->boundingRect().width() / 2 || //
            m_mappedPos.y() > Node::lastHoveredNode()->pos().y() + hhd + Node::lastHoveredNode()->boundingRect().height() / 2 || //
            m_mappedPos.y() < Node::lastHoveredNode()->pos().y() - hhd - Node::lastHoveredNode()->boundingRect().height() / 2) {
            Node::lastHoveredNode()->setHandlesVisible(false);
            Node::lastHoveredNode()->setTextInputActive(false);
        }
    }

    switch (m_mediator.mouseAction().action()) {
    case MouseAction::Action::None:
        break;
    case MouseAction::Action::MoveNode:
        if (const auto node = m_mediator.mouseAction().sourceNode()) {
            if (m_mediator.selectionGroupSize()) {
                m_mediator.moveSelectionGroup(*node, m_grid.snapToGrid(m_mappedPos - m_mediator.mouseAction().sourcePosOnNode()));
            } else {
                node->setLocation(m_grid.snapToGrid(m_mappedPos - m_mediator.mouseAction().sourcePosOnNode()));
            }
        }
        break;
    case MouseAction::Action::CreateOrConnectNode: {
        showDummyDragNode(true);
        showDummyDragEdge(true);
        m_dummyDragNode->setPos(m_grid.snapToGrid(m_mappedPos - m_mediator.mouseAction().sourcePosOnNode()));
        m_dummyDragEdge->updateLine();
        m_mediator.mouseAction().sourceNode()->setHandlesVisible(false);

        // This is needed to clear implicitly "selected" connection candidate nodes when hovering the dummy drag node on other nodes
        m_mediator.clearSelectedNode();
        m_mediator.clearSelectionGroup();

        m_connectionTargetNode = nullptr;

        // TODO: Use items() to pre-filter the nodes
        if (auto && node = m_mediator.getBestOverlapNode(*m_dummyDragNode)) {
            node->setSelected(true);
            m_connectionTargetNode = node;
        }
    } break;
    case MouseAction::Action::RubberBand:
        updateRubberBand();
        break;
    case MouseAction::Action::Scroll:
        break;
    }

    QGraphicsView::mouseMoveEvent(event);
}

void EditorView::mousePressEvent(QMouseEvent * event)
{
    m_clickedPos = event->pos();
    const auto clickedScenePos = mapToScene(m_clickedPos);
    m_mediator.mouseAction().setClickedScenePos(clickedScenePos);

    if (const auto result = ItemFilter::getFirstItemAtPosition(*scene(), clickedScenePos, Constants::View::CLICK_TOLERANCE); result.success) {
        if (result.edge) {
            juzzlin::L().debug() << "Edge pressed";
            handleMousePressEventOnEdge(*event, *result.edge);
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
        switch (m_mediator.mouseAction().action()) {
        case MouseAction::Action::RubberBand:
            finishRubberBand();
            break;
        default:
            break;
        }
    } else if (m_controlStrategy->primaryButtonClicked(*event)) {
        switch (m_mediator.mouseAction().action()) {
        case MouseAction::Action::None:
            // This can happen if the user deletes the drag node while connecting nodes or creating a new node.
            // In this case the action is just aborted.
            if (m_connectionTargetNode) {
                m_connectionTargetNode->setSelected(false);
                m_connectionTargetNode = nullptr;
            }
            break;
        case MouseAction::Action::MoveNode:
            m_mediator.adjustSceneRect();
            break;
        case MouseAction::Action::CreateOrConnectNode:
            if (const auto sourceNode = m_mediator.mouseAction().sourceNode()) {
                if (m_connectionTargetNode) {
                    m_mediator.addEdge(*sourceNode, *m_connectionTargetNode);
                    m_connectionTargetNode->setSelected(false);
                    m_connectionTargetNode = nullptr;
                } else {
                    m_mediator.createAndAddNode(sourceNode->index(), m_grid.snapToGrid(m_mappedPos - m_mediator.mouseAction().sourcePosOnNode()));
                }
                resetDummyDragItems();
            }
            break;
        case MouseAction::Action::RubberBand:
            finishRubberBand();
            break;
        case MouseAction::Action::Scroll:
            setDragMode(NoDrag);
            break;
        }

        QApplication::restoreOverrideCursor();
    }

    m_mediator.mouseAction().clear();

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
    if (const auto sourceNode = m_mediator.mouseAction().sourceNode()) {
        if (!m_dummyDragEdge) {
            L().debug() << "Creating a new dummy drag edge";
            m_dummyDragEdge = std::make_unique<Edge>(sourceNode, m_dummyDragNode.get(), false, false);
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
        m_dummyDragNode = std::make_unique<Node>();
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
}

void EditorView::updateRubberBand()
{
    m_rubberBand->setGeometry(QRect(m_mediator.mouseAction().rubberBandOrigin().toPoint(), m_pos.toPoint()).normalized());
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
    if (scene())
        scene()->update();
}

void EditorView::setGridColor(const QColor & gridColor)
{
    m_mediator.mindMapData()->setGridColor(gridColor);
}

void EditorView::setGridVisible(bool visible)
{
    m_gridVisible = visible;
    if (scene())
        scene()->update();
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

void EditorView::drawBackground(QPainter * painter, const QRectF & rect)
{
    painter->save();
    painter->fillRect(rect, backgroundBrush());

    if (const int gridSize = m_grid.size(); m_gridVisible && gridSize != 0) {
        const qreal left = int(rect.left()) - (int(rect.left()) % gridSize);
        const qreal top = int(rect.top()) - (int(rect.top()) % gridSize);

        QVarLengthArray<QLineF, 100> lines;

        auto x = left;
        while (x < rect.right()) {
            lines.append({ x, rect.top(), x, rect.bottom() });
            x += gridSize;
        }
        auto y = top;
        while (y < rect.bottom()) {
            lines.append({ rect.left(), y, rect.right(), y });
            y += gridSize;
        }

        painter->setPen(m_mediator.mindMapData()->gridColor());
        painter->drawLines(lines.data(), static_cast<int>(lines.size()));
    }

    painter->restore();
}

EditorView::~EditorView() = default;
