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
#include <QColorDialog>
#include <QGraphicsItem>
#include <QGraphicsSimpleTextItem>
#include <QMouseEvent>
#include <QRectF>
#include <QRubberBand>
#include <QStatusBar>
#include <QString>
#include <QTransform>

#include "editor_view.hpp"

#include "constants.hpp"
#include "edge.hpp"
#include "edge_context_menu.hpp"
#include "edge_text_edit.hpp"
#include "graphics_factory.hpp"
#include "mediator.hpp"
#include "mind_map_data.hpp"
#include "mouse_action.hpp"
#include "node.hpp"
#include "node_action.hpp"
#include "node_handle.hpp"
#include "simple_logger.hpp"

#include "contrib/SimpleLogger/src/simple_logger.hpp"

#include <cassert>
#include <cstdlib>

using juzzlin::L;

EditorView::EditorView(Mediator & mediator)
  : m_mediator(mediator)
  , m_edgeContextMenu(new EdgeContextMenu(this, m_mediator))
  , m_mainContextMenu(new MainContextMenu(this, m_mediator, m_grid))
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    setMouseTracking(true);

    setRenderHint(QPainter::Antialiasing);

    // Forward signals from main context menu
    connect(m_mainContextMenu, &MainContextMenu::actionTriggered, this, &EditorView::actionTriggered);
    connect(m_mainContextMenu, &MainContextMenu::newNodeRequested, this, &EditorView::newNodeRequested);
}

const Grid & EditorView::grid() const
{
    return m_grid;
}

void EditorView::finishRubberBand()
{
    m_mediator.setRectagleSelection({ mapToScene(m_rubberBand->geometry().topLeft()), mapToScene(m_rubberBand->geometry().bottomRight()) });

    m_rubberBand->hide();
}

void EditorView::handleMousePressEventOnBackground(QMouseEvent & event)
{
    if (!isModifierPressed()) {
        m_mediator.clearSelectionGroup();
        m_mediator.setSelectedEdge(nullptr);
    }

    if (event.button() == Qt::LeftButton) {
        if (isModifierPressed()) {
            initiateRubberBand();
        } else {
            m_mediator.mouseAction().setSourceNode(nullptr, MouseAction::Action::Scroll);
            setDragMode(ScrollHandDrag);
        }
    } else if (event.button() == Qt::RightButton) {
        openMainContextMenu(MainContextMenu::Mode::Background);
    }
}

void EditorView::handleMousePressEventOnEdge(QMouseEvent & event, Edge & edge)
{
    if (event.button() == Qt::RightButton) {
        handleRightButtonClickOnEdge(edge);
    }
}

void EditorView::handleMousePressEventOnNode(QMouseEvent & event, Node & node)
{
    if (node.index() != -1) // Prevent right-click on the drag node
    {
        if (event.button() == Qt::RightButton) {
            handleRightButtonClickOnNode(node);
        } else if (event.button() == Qt::LeftButton) {
            handleLeftButtonClickOnNode(node);
        }
    }
}

void EditorView::handleMousePressEventOnNodeHandle(QMouseEvent & event, NodeHandle & nodeHandle)
{
    if (isModifierPressed()) {
        return;
    }

    if (event.button() == Qt::LeftButton) {
        handleLeftButtonClickOnNodeHandle(nodeHandle);
    }
}

void EditorView::handleLeftButtonClickOnNode(Node & node)
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

void EditorView::handleLeftButtonClickOnNodeHandle(NodeHandle & nodeHandle)
{
    switch (nodeHandle.role()) {
    case NodeHandle::Role::Add:
        m_mediator.initiateNewNodeDrag(nodeHandle);
        break;
    case NodeHandle::Role::Drag:
        m_mediator.initiateNodeDrag(nodeHandle.parentNode());
        break;
    case NodeHandle::Role::Color:
        m_mediator.addSelectedNode(nodeHandle.parentNode());
        emit actionTriggered(StateMachine::Action::NodeColorChangeRequested);
        break;
    case NodeHandle::Role::TextColor:
        m_mediator.addSelectedNode(nodeHandle.parentNode());
        emit actionTriggered(StateMachine::Action::TextColorChangeRequested);
        break;
    }
}

void EditorView::handleRightButtonClickOnEdge(Edge & edge)
{
    m_mediator.setSelectedEdge(&edge);

    openEdgeContextMenu();
}

void EditorView::handleRightButtonClickOnNode(Node & node)
{
    if (!node.selected()) {
        m_mediator.clearSelectionGroup();
    }

    m_mediator.addSelectedNode(node);

    openMainContextMenu(MainContextMenu::Mode::Node);
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

void EditorView::mouseMoveEvent(QMouseEvent * event)
{
    m_pos = event->pos();
    m_mappedPos = mapToScene(event->pos());
    m_mediator.mouseAction().setMappedPos(m_mappedPos);

    switch (m_mediator.mouseAction().action()) {
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
    default:
        break;
    }

    QGraphicsView::mouseMoveEvent(event);
}

void EditorView::mousePressEvent(QMouseEvent * event)
{
    m_clickedPos = event->pos();
    const auto clickedScenePos = mapToScene(m_clickedPos);
    m_mediator.mouseAction().setClickedScenePos(clickedScenePos);

    const int tolerance = Constants::View::CLICK_TOLERANCE;
    const QRectF clickRect(clickedScenePos.x() - tolerance, clickedScenePos.y() - tolerance, tolerance * 2, tolerance * 2);

    // Fetch all items at the location
    const auto items = scene()->items(clickRect, Qt::IntersectsItemShape, Qt::DescendingOrder);
    if (items.size()) {
        const auto item = *items.begin();
        if (const auto edge = dynamic_cast<Edge *>(item)) {
            juzzlin::L().debug() << "Edge pressed";
            handleMousePressEventOnEdge(*event, *edge);
        } else if (const auto node = dynamic_cast<NodeHandle *>(item)) {
            juzzlin::L().debug() << "Node handle pressed";
            handleMousePressEventOnNodeHandle(*event, *node);
        } else if (const auto node = dynamic_cast<Node *>(item)) {
            juzzlin::L().debug() << "Node pressed";
            handleMousePressEventOnNode(*event, *node);
            if (isModifierPressed()) { // User was just selecting
                node->setTextInputActive(false);
                return;
            }
        }
        // This hack enables edge context menu even if user clicks on the edge text edit.
        else if (const auto edgeTextEdit = dynamic_cast<EdgeTextEdit *>(item)) {
            if (event->button() == Qt::RightButton) {
                const auto edge = dynamic_cast<Edge *>(edgeTextEdit->parentItem());
                if (edge) {
                    juzzlin::L().debug() << "Edge text edit pressed";
                    handleMousePressEventOnEdge(*event, *edge);
                    return;
                }
            }
        }
        // This hack enables node context menu even if user clicks on the node text edit.
        else if (const auto nodeTextEdit = dynamic_cast<TextEdit *>(item)) {
            if (event->button() == Qt::RightButton || (event->button() == Qt::LeftButton && isModifierPressed())) {
                const auto node = dynamic_cast<Node *>(nodeTextEdit->parentItem());
                if (node) {
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
    if (event->button() == Qt::LeftButton) {
        switch (m_mediator.mouseAction().action()) {
        case MouseAction::Action::MoveNode:
            m_mediator.mouseAction().clear();
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
                m_mediator.mouseAction().clear();
            }
            break;
        case MouseAction::Action::RubberBand:
            finishRubberBand();
            m_mediator.mouseAction().clear();
            break;
        case MouseAction::Action::Scroll:
            setDragMode(NoDrag);
            break;
        default:
            break;
        }

        QApplication::restoreOverrideCursor();
    }

    QGraphicsView::mouseReleaseEvent(event);
}

void EditorView::openEdgeContextMenu()
{
    m_edgeContextMenu->exec(mapToGlobal(m_clickedPos));
}

void EditorView::openMainContextMenu(MainContextMenu::Mode mode)
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
            m_dummyDragEdge = std::make_unique<Edge>(*sourceNode, *m_dummyDragNode, false, false);
            m_dummyDragEdge->setOpacity(0.5);
            scene()->addItem(m_dummyDragEdge.get());
        } else {
            m_dummyDragEdge->setSourceNode(*sourceNode);
            m_dummyDragEdge->setTargetNode(*m_dummyDragNode);
        }
    }

    m_dummyDragEdge->setColor(m_edgeColor);
    m_dummyDragEdge->setWidth(m_edgeWidth);
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

void EditorView::setCornerRadius(int cornerRadius)
{
    m_cornerRadius = cornerRadius;
}

void EditorView::setGridSize(int size)
{
    m_grid.setSize(size);
    if (scene())
        scene()->update();
}

void EditorView::setGridVisible(bool visible)
{
    m_gridVisible = visible;
    if (scene())
        scene()->update();
}

void EditorView::setEdgeColor(const QColor & edgeColor)
{
    m_edgeColor = edgeColor;
}

void EditorView::setGridColor(const QColor & gridColor)
{
    m_mediator.mindMapData()->setGridColor(gridColor);
}

void EditorView::setEdgeWidth(double edgeWidth)
{
    m_edgeWidth = edgeWidth;
}

void EditorView::wheelEvent(QWheelEvent * event)
{
    zoom(event->delta() > 0 ? Constants::View::ZOOM_SENSITIVITY : 1.0 / Constants::View::ZOOM_SENSITIVITY);
}

void EditorView::zoom(double amount)
{
    m_scale *= amount;
    m_scale = std::min(m_scale, Constants::View::ZOOM_MAX);
    m_scale = std::max(m_scale, Constants::View::ZOOM_MIN);

    updateScale();
}

void EditorView::zoomToFit(QRectF nodeBoundingRect)
{
    const double viewAspect = static_cast<double>(rect().height()) / rect().width();
    const double nodeAspect = nodeBoundingRect.height() / nodeBoundingRect.width();

    if (viewAspect < 1.0) {
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

void EditorView::drawBackground(QPainter *painter, const QRectF &rect)
{
    painter->save();

    painter->fillRect(rect, this->backgroundBrush());

    const int gridSize = m_grid.size();

    if (m_gridVisible && gridSize != 0)
    {
        const qreal left = int(rect.left()) - (int(rect.left()) % gridSize);
        const qreal top = int(rect.top()) - (int(rect.top()) % gridSize);

        QVarLengthArray<QLineF, 100> lines;

        for (qreal x = left; x < rect.right(); x += gridSize) {
            lines.append(QLineF(x, rect.top(), x, rect.bottom()));
        }
        for (qreal y = top; y < rect.bottom(); y += gridSize) {
            lines.append(QLineF(rect.left(), y, rect.right(), y));
        }

        painter->setPen(m_mediator.mindMapData()->gridColor());
        painter->drawLines(lines.data(), lines.size());
    }

    painter->restore();
}

EditorView::~EditorView() = default;
