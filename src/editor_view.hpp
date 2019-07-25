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

#ifndef EDITORVIEW_HPP
#define EDITORVIEW_HPP

#include "state_machine.hpp"

#include <QColor>
#include <QGraphicsView>
#include <QMenu>

#include <set>

class Edge;
class EdgeContextMenu;
class Node;
class NodeHandle;
class Mediator;
class MindMapTile;
class Object;
class ObjectModelLoaderoader;
class QAction;
class QMouseEvent;
class QPaintEvent;
class QWheelEvent;
class QGraphicsSimpleTextItem;
class QRubberBand;

class EditorView : public QGraphicsView
{
    Q_OBJECT

public:

    explicit EditorView(Mediator & mediator);

    ~EditorView() override;

    void resetDummyDragItems();

    void zoom(int amount);

    void zoomToFit(QRectF nodeBoundingRect);

public slots:

    void setCornerRadius(int cornerRadius);

    void setEdgeColor(const QColor & edgeColor);

    void setEdgeWidth(double edgeWidth);

    void setGridSize(int size);

protected:

    void mouseMoveEvent(QMouseEvent * event) override;

    void mousePressEvent(QMouseEvent * event) override;

    void mouseReleaseEvent(QMouseEvent * event) override;

    void wheelEvent(QWheelEvent * event) override;

signals:

    void actionTriggered(StateMachine::Action action);

    void newNodeRequested(QPointF position);

private:

    enum class MainContextMenuMode
    {
        Background,
        Node
    };

    void createMainContextMenuActions();

    void finishRubberBand();

    void handleMousePressEventOnBackground(QMouseEvent & event);

    void handleMousePressEventOnEdge(QMouseEvent & event, Edge & edge);

    void handleMousePressEventOnNode(QMouseEvent & event, Node & node);

    void handleMousePressEventOnNodeHandle(QMouseEvent & event, NodeHandle & nodeHandle);

    void handleLeftButtonClickOnNode(Node & node);

    void handleLeftButtonClickOnNodeHandle(NodeHandle & nodeHandle);

    void handleRightButtonClickOnEdge(Edge & edge);

    void handleRightButtonClickOnNode(Node & node);

    void initiateNewNodeDrag(NodeHandle & nodeHandle);

    void initiateRubberBand();

    bool isControlPressed() const;

    void openBackgroundContextMenu();

    void openEdgeContextMenu();

    void openMainContextMenu(MainContextMenuMode mode);

    void removeNodeFromSelectionGroup(Node & node);

    void showDummyDragEdge(bool show);

    void showDummyDragNode(bool show);

    QPointF snapToGrid(QPointF in) const;

    void updateScale(int value);

    void updateRubberBand();

    QMenu m_mainContextMenu;

    QPoint m_clickedPos;

    QPointF m_clickedScenePos;

    QAction * m_doubleArrowAction = nullptr;

    QAction * m_singleArrowAction = nullptr;

    QAction * m_setNodeColorAction = nullptr;

    QAction * m_setNodeTextColorAction = nullptr;

    QAction * m_deleteNodeAction = nullptr;

    QAction * m_changeEdgeDirectionAction = nullptr;

    QAction * m_hideEdgeArrowAction = nullptr;

    QAction * m_deleteEdgeAction = nullptr;

    std::map<MainContextMenuMode, std::vector<QAction *> > m_mainContextMenuActions;

    QPointF m_pos;

    QPointF m_mappedPos;

    int m_scaleValue = 100;

    Mediator & m_mediator;

    Node * m_dummyDragNode = nullptr;

    Edge * m_dummyDragEdge = nullptr;

    Node * m_selectedNode = nullptr;

    std::shared_ptr<Node> m_connectionTargetNode;

    int m_cornerRadius = 0;

    int m_gridSize = 0;

    // Width for the dummy drag edge
    double m_edgeWidth = 1.5;

    QColor m_edgeColor;

    QRectF m_nodeBoundingRect;

    QRubberBand * m_rubberBand = nullptr;

    EdgeContextMenu * m_edgeContextMenu;
};

#endif // EDITORVIEW_HPP
