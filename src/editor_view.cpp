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
#include <QStatusBar>
#include <QString>
#include <QTransform>

#include "editor_view.hpp"

#include "constants.hpp"
#include "mouse_action.hpp"
#include "edge.hpp"
#include "graphics_factory.hpp"
#include "simple_logger.hpp"
#include "mediator.hpp"
#include "mind_map_data.hpp"
#include "node.hpp"
#include "node_handle.hpp"

#include "contrib/SimpleLogger/src/simple_logger.hpp"

#include <cassert>
#include <cstdlib>

using juzzlin::L;

EditorView::EditorView(Mediator & mediator)
    : m_mediator(mediator)
{
    createBackgroundContextMenuActions();
    createEdgeContextMenuActions();
    createNodeContextMenuActions();

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    setMouseTracking(true);

    setRenderHint(QPainter::Antialiasing);
}

void EditorView::createBackgroundContextMenuActions()
{
    auto setBackgroundColorAction = new QAction(tr("Set background color"), &m_backgroundContextMenu);
    QObject::connect(setBackgroundColorAction, &QAction::triggered, [this] () {
        emit actionTriggered(StateMachine::Action::BackgroundColorChangeRequested);
    });

    m_backgroundContextMenu.addAction(setBackgroundColorAction);
    m_backgroundContextMenu.addSeparator();

    auto setEdgeColorAction = new QAction(tr("Set edge color"), &m_backgroundContextMenu);
    QObject::connect(setEdgeColorAction, &QAction::triggered, [this] () {
        emit actionTriggered(StateMachine::Action::EdgeColorChangeRequested);
    });

    m_backgroundContextMenu.addAction(setEdgeColorAction);
    m_backgroundContextMenu.addSeparator();

    auto createNode = new QAction(tr("Create floating node"), &m_backgroundContextMenu);
    QObject::connect(createNode, &QAction::triggered, [this] () {
        emit newNodeRequested(snapToGrid(m_clickedScenePos));
    });

    m_backgroundContextMenu.addAction(createNode);
}

void EditorView::createEdgeContextMenuActions()
{
    m_deleteEdgeAction = new QAction(tr("Delete edge"), &m_edgeContextMenu);
    QObject::connect(m_deleteEdgeAction, &QAction::triggered, [=] {
        assert(m_mediator.selectedEdge());
        m_mediator.saveUndoPoint();
        m_mediator.deleteEdge(*m_mediator.selectedEdge());
    });

    // Populate the menu
    m_edgeContextMenu.addAction(m_deleteEdgeAction);
}

void EditorView::createNodeContextMenuActions()
{
    m_setNodeColorAction = new QAction(tr("Set node color"), &m_nodeContextMenu);
    QObject::connect(m_setNodeColorAction, &QAction::triggered, [this] () {
        assert(m_mediator.selectedNode());
        const auto color = QColorDialog::getColor(Qt::white, this);
        if (color.isValid()) {
            m_mediator.saveUndoPoint();
            m_mediator.selectedNode()->setColor(color);
        }
    });

    m_setNodeTextColorAction = new QAction(tr("Set text color"), &m_nodeContextMenu);
    QObject::connect(m_setNodeTextColorAction, &QAction::triggered, [this] () {
        assert(m_mediator.selectedNode());
        const auto color = QColorDialog::getColor(Qt::white, this);
        if (color.isValid()) {
            m_mediator.saveUndoPoint();
            m_mediator.selectedNode()->setTextColor(color);
        }
    });

    m_deleteNodeAction = new QAction(tr("Delete node"), &m_nodeContextMenu);
    QObject::connect(m_deleteNodeAction, &QAction::triggered, [this] () {
        assert(m_mediator.selectedNode());
        m_mediator.saveUndoPoint();
        m_mediator.deleteNode(*m_mediator.selectedNode());
    });

    // Populate the menu
    m_nodeContextMenu.addAction(m_setNodeColorAction);
    m_nodeContextMenu.addAction(m_setNodeTextColorAction);
    m_nodeContextMenu.addSeparator();
    m_nodeContextMenu.addAction(m_deleteNodeAction);
}

void EditorView::handleMousePressEventOnBackground(QMouseEvent & event)
{
    // Only clear selection group if Ctrl pressed
    if (isControlPressed() && m_mediator.selectionGroupSize())
    {
        m_mediator.clearSelectionGroup();
        return;
    }

    if (event.button() == Qt::LeftButton)
    {
        m_mediator.mouseAction().setSourceNode(nullptr, MouseAction::Action::Scroll);
        setDragMode(ScrollHandDrag);
    }
    else if (event.button() == Qt::RightButton)
    {
        openBackgroundContextMenu();
    }
}

void EditorView::handleMousePressEventOnEdge(QMouseEvent & event, Edge & edge)
{
    if (event.button() == Qt::RightButton)
    {
        handleRightButtonClickOnEdge(edge);
    }
}

void EditorView::handleMousePressEventOnNode(QMouseEvent & event, Node & node)
{
    if (node.index() != -1) // Prevent right-click on the drag node
    {
        if (event.button() == Qt::RightButton)
        {
            handleRightButtonClickOnNode(node);
        }
        else if (event.button() == Qt::LeftButton)
        {
            handleLeftButtonClickOnNode(node);
        }
    }
}

void EditorView::handleMousePressEventOnNodeHandle(QMouseEvent & event, NodeHandle & nodeHandle)
{
    if (isControlPressed())
    {
        return;
    }

    if (event.button() == Qt::LeftButton)
    {
        handleLeftButtonClickOnNodeHandle(nodeHandle);
    }
}

void EditorView::handleLeftButtonClickOnNode(Node & node)
{
    if (isControlPressed())
    {
        // Use is selecting a node

        m_mediator.toggleNodeInSelectionGroup(node);
    }
    else
    {
        // User is initiating a node move drag

        m_mediator.saveUndoPoint();

        node.setZValue(node.zValue() + 1);
        m_mediator.mouseAction().setSourceNode(&node, MouseAction::Action::MoveNode);
        m_mediator.mouseAction().setSourcePos(m_mappedPos);
        m_mediator.mouseAction().setSourcePosOnNode(m_mappedPos - node.pos());

        // Change cursor to the closed hand cursor.
        QApplication::setOverrideCursor(QCursor(Qt::ClosedHandCursor));
    }
}

void EditorView::handleLeftButtonClickOnNodeHandle(NodeHandle & nodeHandle)
{
    switch (nodeHandle.role())
    {
    case NodeHandle::Role::Add:
        initiateNewNodeDrag(nodeHandle);
        break;
    case NodeHandle::Role::Color:
        m_mediator.setSelectedNode(&nodeHandle.parentNode());
        m_setNodeColorAction->trigger();
        break;
    case NodeHandle::Role::TextColor:
        m_mediator.setSelectedNode(&nodeHandle.parentNode());
        m_setNodeTextColorAction->trigger();
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
    m_mediator.setSelectedNode(&node);

    openNodeContextMenu();
}

void EditorView::initiateNewNodeDrag(NodeHandle & nodeHandle)
{
    // User is initiating a new node drag
    m_mediator.saveUndoPoint();
    auto parentNode = dynamic_cast<Node *>(nodeHandle.parentItem());
    assert(parentNode);
    m_mediator.mouseAction().setSourceNode(parentNode, MouseAction::Action::CreateOrConnectNode);
    m_mediator.mouseAction().setSourcePosOnNode(nodeHandle.pos());
    parentNode->hoverLeaveEvent(nullptr);
    // Change cursor to the closed hand cursor.
    QApplication::setOverrideCursor(QCursor(Qt::ClosedHandCursor));
}

bool EditorView::isControlPressed() const
{
    return QGuiApplication::queryKeyboardModifiers().testFlag(Qt::ControlModifier);
}

void EditorView::mouseMoveEvent(QMouseEvent * event)
{
    m_mappedPos = mapToScene(event->pos());

    switch (m_mediator.mouseAction().action())
    {
    case MouseAction::Action::MoveNode:
        if (const auto node = m_mediator.mouseAction().sourceNode())
        {
            if (!m_mediator.isInSelectionGroup(*node))
            {
                m_mediator.clearSelectionGroup();
            }

            if (m_mediator.selectionGroupSize())
            {
                m_mediator.moveSelectionGroup(*node, snapToGrid(m_mappedPos - m_mediator.mouseAction().sourcePosOnNode()));
            }
            else
            {
                node->setLocation(snapToGrid(m_mappedPos - m_mediator.mouseAction().sourcePosOnNode()));
            }
        }
        break;
    case MouseAction::Action::CreateOrConnectNode:
    {
        showDummyDragNode(true);
        showDummyDragEdge(true);
        m_dummyDragNode->setPos(snapToGrid(m_mappedPos - m_mediator.mouseAction().sourcePosOnNode()));
        m_dummyDragEdge->updateLine();
        m_mediator.mouseAction().sourceNode()->setHandlesVisible(false);

        m_mediator.clearSelectedNode();
        m_mediator.clearSelectionGroup();

        m_connectionTargetNode = nullptr;
        // TODO: Use items() to pre-filter the nodes
        if (auto && node = m_mediator.getBestOverlapNode(*m_dummyDragNode))
        {
            node->setSelected(true);
            m_connectionTargetNode = node;
        }
    }
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
    m_clickedScenePos = mapToScene(m_clickedPos);

    const int tolerance = Constants::View::CLICK_TOLERANCE;
    QRectF clickRect(m_clickedScenePos.x() - tolerance, m_clickedScenePos.y() - tolerance, tolerance * 2, tolerance * 2);

    // Fetch all items at the location
    QList<QGraphicsItem *> items = scene()->items(clickRect, Qt::IntersectsItemShape, Qt::DescendingOrder);

    if (items.size())
    {
        auto item = *items.begin();
        if (auto node = dynamic_cast<Node *>(item))
        {
            handleMousePressEventOnNode(*event, *node);
        }
        else if (auto edge = dynamic_cast<Edge *>(item))
        {
            handleMousePressEventOnEdge(*event, *edge);
        }
        else if (auto node = dynamic_cast<NodeHandle *>(item))
        {
            handleMousePressEventOnNodeHandle(*event, *node);
        }
    }
    else
    {
        handleMousePressEventOnBackground(*event);
    }

    QGraphicsView::mousePressEvent(event);
}

void EditorView::mouseReleaseEvent(QMouseEvent * event)
{
    if (event->button() == Qt::LeftButton)
    {
        switch (m_mediator.mouseAction().action())
        {
        case MouseAction::Action::MoveNode:
            m_mediator.mouseAction().clear();
            break;
        case MouseAction::Action::CreateOrConnectNode:
            if (auto sourceNode = m_mediator.mouseAction().sourceNode())
            {
                if (m_connectionTargetNode)
                {
                    m_mediator.addEdge(*sourceNode, *m_connectionTargetNode);
                    m_connectionTargetNode->setSelected(false);
                    m_connectionTargetNode = nullptr;
                }
                else
                {
                    m_mediator.createAndAddNode(sourceNode->index(), snapToGrid(m_mappedPos - m_mediator.mouseAction().sourcePosOnNode()));
                }

                resetDummyDragItems();
                m_mediator.mouseAction().clear();
            }
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

void EditorView::openBackgroundContextMenu()
{
    m_backgroundContextMenu.exec(mapToGlobal(m_clickedPos));
}

void EditorView::openEdgeContextMenu()
{
    m_edgeContextMenu.exec(mapToGlobal(m_clickedPos));
}

void EditorView::openNodeContextMenu()
{
    // Allow deletion of leaf nodes or nodes between exactly two nodes.
    m_deleteNodeAction->setEnabled(true);

    m_nodeContextMenu.exec(mapToGlobal(m_clickedPos));
}

void EditorView::resetDummyDragItems()
{
    // Ensure new dummy nodes and related graphics items are created (again) when needed.
    delete m_dummyDragEdge;
    m_dummyDragEdge = nullptr;
    delete m_dummyDragNode;
    m_dummyDragNode = nullptr;

    L().debug() << "Dummy drag item reset";
}

void EditorView::showDummyDragEdge(bool show)
{
    if (auto sourceNode = m_mediator.mouseAction().sourceNode())
    {
        if (!m_dummyDragEdge)
        {
            L().debug() << "Creating a new dummy drag edge";
            m_dummyDragEdge = new Edge(*sourceNode, *m_dummyDragNode, false, false);
            m_dummyDragEdge->setOpacity(0.5);
            scene()->addItem(m_dummyDragEdge);
        }
        else
        {
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
    if (!m_dummyDragNode)
    {
        L().debug() << "Creating a new dummy drag node";
        m_dummyDragNode = new Node;
        scene()->addItem(m_dummyDragNode);
    }

    m_dummyDragNode->setOpacity(Constants::View::DRAG_NODE_OPACITY);
    m_dummyDragNode->setVisible(show);
}

QPointF EditorView::snapToGrid(QPointF in) const
{
    if (!m_gridSize)
    {
        return in;
    }

    return {
        static_cast<double>(static_cast<int>(in.x() / m_gridSize) * m_gridSize),
        static_cast<double>(static_cast<int>(in.y() / m_gridSize) * m_gridSize)};
}

void EditorView::updateScale(int value)
{
    QTransform transform;
    const double scale = static_cast<double>(value) / 100;
    transform.scale(scale, scale);
    setTransform(transform);
}

void EditorView::setGridSize(int size)
{
    m_gridSize = size;
}

void EditorView::setEdgeColor(const QColor & edgeColor)
{
    m_edgeColor = edgeColor;
}

void EditorView::setEdgeWidth(double edgeWidth)
{
    m_edgeWidth = edgeWidth;
}

void EditorView::wheelEvent(QWheelEvent * event)
{
    zoom(event->delta() > 0 ? Constants::View::ZOOM_SENSITIVITY : -Constants::View::ZOOM_SENSITIVITY);
}

void EditorView::zoom(int amount)
{
    m_scaleValue += amount;
    m_scaleValue = std::min(m_scaleValue, Constants::View::ZOOM_MAX);
    m_scaleValue = std::max(m_scaleValue, Constants::View::ZOOM_MIN);

    updateScale(m_scaleValue);
}

void EditorView::zoomToFit(QRectF nodeBoundingRect)
{
    const double viewAspect = double(rect().height()) / rect().width();
    const double nodeAspect = nodeBoundingRect.height() / nodeBoundingRect.width();

    if (viewAspect < 1.0)
    {
        if (nodeAspect < viewAspect)
        {
            m_scaleValue = rect().width() * 100 / nodeBoundingRect.width();
        }
        else
        {
            m_scaleValue = rect().height() * 100 / nodeBoundingRect.height();
        }
    }
    else
    {
        if (nodeAspect > viewAspect)
        {
            m_scaleValue = rect().height() * 100 / nodeBoundingRect.height();
        }
        else
        {
            m_scaleValue = rect().width() * 100 / nodeBoundingRect.width();
        }
    }

    updateScale(m_scaleValue);

    centerOn(nodeBoundingRect.center());

    m_nodeBoundingRect = nodeBoundingRect;
}

EditorView::~EditorView()
{
}
