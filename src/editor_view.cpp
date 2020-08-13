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

#include <QDebug> // TODO remove

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
#include "node_handle.hpp"
#include "simple_logger.hpp"

#include "contrib/SimpleLogger/src/simple_logger.hpp"

#include <cassert>
#include <cstdlib>

using juzzlin::L;

EditorView::EditorView(Mediator & mediator)
  : m_mediator(mediator)
  , m_copyPaste(mediator, m_grid)
  , m_edgeContextMenu(new EdgeContextMenu(this, m_mediator))
  , m_mainContextMenu(new MainContextMenu(this, m_mediator, m_grid, m_copyPaste))
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    setMouseTracking(true);

    setRenderHint(QPainter::Antialiasing);

    // Forward signals from main context menu
    connect(m_mainContextMenu, &MainContextMenu::actionTriggered, this, &EditorView::actionTriggered);
    connect(m_mainContextMenu, &MainContextMenu::newNodeRequested, this, &EditorView::newNodeRequested);
    connect(m_mainContextMenu, &MainContextMenu::nodeColorActionTriggered, this, &EditorView::openNodeColorDialog);
    connect(m_mainContextMenu, &MainContextMenu::nodeTextColorActionTriggered, this, &EditorView::openNodeTextColorDialog);
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
    if (m_mediator.selectionGroupSize()) {
        m_mediator.clearSelectionGroup();
    }
    m_mediator.setSelectedEdge(nullptr);
    m_mediator.setSelectedNode(nullptr);

    if (event.button() == Qt::LeftButton) {
        if (isControlPressed()) {
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
    if (isControlPressed()) {
        return;
    }

    if (event.button() == Qt::LeftButton) {
        handleLeftButtonClickOnNodeHandle(nodeHandle);
    }
}

void EditorView::handleLeftButtonClickOnNode(Node & node)
{
    if (isControlPressed()) {
        // User is selecting a node
        m_mediator.toggleNodeInSelectionGroup(node);
    } else {
        // Clear selection group if the node is not in it
        if (m_mediator.selectionGroupSize() && !m_mediator.isInSelectionGroup(node)) {
            m_mediator.clearSelectionGroup();
        }

        // User is initiating a node move drag
        initiateNodeDrag(node);
    }
}

void EditorView::handleLeftButtonClickOnNodeHandle(NodeHandle & nodeHandle)
{
    switch (nodeHandle.role()) {
    case NodeHandle::Role::Add:
        m_mediator.clearSelectionGroup();
        initiateNewNodeDrag(nodeHandle);
        break;
    case NodeHandle::Role::Drag:
        initiateNodeDrag(nodeHandle.parentNode());
        break;
    case NodeHandle::Role::Color:
        m_mediator.clearSelectionGroup();
        m_mediator.setSelectedNode(&nodeHandle.parentNode());
        openNodeColorDialog();
        break;
    case NodeHandle::Role::TextColor:
        m_mediator.clearSelectionGroup();
        m_mediator.setSelectedNode(&nodeHandle.parentNode());
        openNodeTextColorDialog();
        break;
    }

    m_mediator.setSelectedNode(nullptr);
}

void EditorView::handleRightButtonClickOnEdge(Edge & edge)
{
    m_mediator.setSelectedEdge(&edge);

    openEdgeContextMenu();
}

void EditorView::handleRightButtonClickOnNode(Node & node)
{
    m_mediator.clearSelectionGroup();

    m_mediator.setSelectedNode(&node);

    openMainContextMenu(MainContextMenu::Mode::Node);
}

void EditorView::initiateNewNodeDrag(NodeHandle & nodeHandle)
{
    // User is initiating a new node drag
    m_mediator.saveUndoPoint();
    const auto parentNode = dynamic_cast<Node *>(nodeHandle.parentItem());
    assert(parentNode);
    m_mediator.mouseAction().setSourceNode(parentNode, MouseAction::Action::CreateOrConnectNode);
    m_mediator.mouseAction().setSourcePosOnNode(nodeHandle.pos());
    parentNode->hoverLeaveEvent(nullptr);
    // Change cursor to the closed hand cursor.
    QApplication::setOverrideCursor(QCursor(Qt::ClosedHandCursor));
}

void EditorView::initiateNodeDrag(Node & node)
{
    m_mediator.saveUndoPoint();

    node.setZValue(node.zValue() + 1);
    m_mediator.mouseAction().setSourceNode(&node, MouseAction::Action::MoveNode);
    m_mediator.mouseAction().setSourcePos(m_mappedPos);
    m_mediator.mouseAction().setSourcePosOnNode(m_mappedPos - node.pos());

    // Change cursor to the closed hand cursor.
    QApplication::setOverrideCursor(QCursor(Qt::ClosedHandCursor));
}

void EditorView::initiateRubberBand()
{
    m_mediator.mouseAction().setRubberBandOrigin(m_clickedPos);
    if (!m_rubberBand) {
        m_rubberBand = new QRubberBand { QRubberBand::Rectangle, this };
    }
    m_rubberBand->setGeometry(QRect { m_clickedPos, QSize {} });
    m_rubberBand->show();
}

bool EditorView::isControlPressed() const
{
    return QGuiApplication::queryKeyboardModifiers().testFlag(Qt::ControlModifier);
}

void EditorView::mouseMoveEvent(QMouseEvent * event)
{
    m_pos = event->pos();
    m_mappedPos = mapToScene(event->pos());
    m_mediator.mouseAction().setMappedPos(m_mappedPos);

    switch (m_mediator.mouseAction().action()) {
    case MouseAction::Action::MoveNode:
        if (const auto node = m_mediator.mouseAction().sourceNode()) {
            if (!m_mediator.isInSelectionGroup(*node)) {
                m_mediator.clearSelectionGroup();
            }

            if (m_mediator.selectionGroupSize()) {
                m_mediator.moveSelectionGroup(*node, m_grid.snapToGrid(m_mappedPos - m_mediator.mouseAction().sourcePosOnNode()));
            } else {
                auto cur_pos = m_mappedPos - m_mediator.mouseAction().sourcePosOnNode();

                qreal node_half_width = node->size().rwidth() / 2.0;
                qreal node_half_height = node->size().rheight() / 2.0;

                // make point correction to snap top left corner of the node to grid
                cur_pos.setX(cur_pos.x() - node_half_width);
                cur_pos.setY(cur_pos.y() - node_half_height);

                auto calculated_loc = m_grid.snapToGrid(cur_pos);

                // calculate the center of the node
                calculated_loc.setX(calculated_loc.x() + node_half_width);
                calculated_loc.setY(calculated_loc.y() + node_half_height);

                node->setLocation(calculated_loc);
            }
        }
        break;
    case MouseAction::Action::CreateOrConnectNode: {
        showDummyDragNode(true);
        showDummyDragEdge(true);
        m_dummyDragNode->setPos(m_grid.snapToGrid(m_mappedPos - m_mediator.mouseAction().sourcePosOnNode()));
        m_dummyDragEdge->updateLine();
        m_mediator.mouseAction().sourceNode()->setHandlesVisible(false);

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
        if (const auto node = dynamic_cast<Node *>(item)) {
            handleMousePressEventOnNode(*event, *node);
        } else if (const auto edge = dynamic_cast<Edge *>(item)) {
            handleMousePressEventOnEdge(*event, *edge);
        } else if (const auto node = dynamic_cast<NodeHandle *>(item)) {
            handleMousePressEventOnNodeHandle(*event, *node);
        }
        // This hack enables edge context menu even if user clicks on the edge text edit.
        // Must be the last else-if branch.
        else if (const auto edgeTextEdit = dynamic_cast<EdgeTextEdit *>(item)) {
            if (event->button() == Qt::RightButton) {
                const auto edge = dynamic_cast<Edge *>(edgeTextEdit->parentItem());
                if (edge) {
                    handleMousePressEventOnEdge(*event, *edge);
                    return;
                }
            }
        }
    } else {
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

void EditorView::openNodeColorDialog()
{
    const auto node = m_mediator.selectedNode();
    const auto color = QColorDialog::getColor(Qt::white, this);
    if (color.isValid()) {
        m_mediator.saveUndoPoint();
        node->setColor(color);
    }
}

void EditorView::openNodeTextColorDialog()
{
    const auto node = m_mediator.selectedNode();
    const auto color = QColorDialog::getColor(Qt::white, this);
    if (color.isValid()) {
        m_mediator.saveUndoPoint();
        node->setTextColor(color);
    }
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

void EditorView::updateScale(int value)
{
    QTransform transform;
    const double scale = static_cast<double>(value) / 100;
    transform.scale(scale, scale);
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

void EditorView::setGridVisible(int state)
{
    m_grid_visible = state == Qt::Checked;
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

    if (viewAspect < 1.0) {
        if (nodeAspect < viewAspect) {
            m_scaleValue = static_cast<int>(rect().width() * 100 / nodeBoundingRect.width());
        } else {
            m_scaleValue = static_cast<int>(rect().height() * 100 / nodeBoundingRect.height());
        }
    } else {
        if (nodeAspect > viewAspect) {
            m_scaleValue = static_cast<int>(rect().height() * 100 / nodeBoundingRect.height());
        } else {
            m_scaleValue = static_cast<int>(rect().width() * 100 / nodeBoundingRect.width());
        }
    }

    updateScale(m_scaleValue);

    centerOn(nodeBoundingRect.center());

    m_nodeBoundingRect = nodeBoundingRect;
}

void EditorView::drawBackground(QPainter *painter, const QRectF &rect)
{
    painter->save();

    painter->fillRect(rect, this->backgroundBrush());

    if (m_grid_visible)
    {
        const int gridSize = m_grid.size();

        qreal left = int(rect.left()) - (int(rect.left()) % gridSize);
        qreal top = int(rect.top()) - (int(rect.top()) % gridSize);

        QVarLengthArray<QLineF, 100> lines;

        for (qreal x = left; x < rect.right(); x += gridSize)
            lines.append(QLineF(x, rect.top(), x, rect.bottom()));
        for (qreal y = top; y < rect.bottom(); y += gridSize)
            lines.append(QLineF(rect.left(), y, rect.right(), y));

        painter->setPen(m_mediator.mindMapData()->gridColor());
        painter->drawLines(lines.data(), lines.size());
    }

    painter->restore();
}

EditorView::~EditorView() = default;
