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

#include <QGraphicsView>
#include <QMenu>

class Edge;
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

class EditorView : public QGraphicsView
{
    Q_OBJECT

public:

    explicit EditorView(Mediator & mediator);

    ~EditorView();

    void showHelloText(bool show);

    void resetDummyDragItems();

    void zoomToFit(QRectF nodeBoundingRect);

protected:

    void mouseMoveEvent(QMouseEvent * event) override;

    void mousePressEvent(QMouseEvent * event) override;

    void mouseReleaseEvent(QMouseEvent * event) override;

    void wheelEvent(QWheelEvent * event) override;

signals:

    void backgroundColorChanged(QColor color);

private:

    void createBackgroundContextMenuActions();

    void createNodeContextMenuActions();

    void handleMousePressEventOnBackground(QMouseEvent & event);

    void handleMousePressEventOnNode(QMouseEvent & event, Node & node);

    void handleMousePressEventOnNodeHandle(QMouseEvent & event, NodeHandle & nodeHandle);

    void handleLeftButtonClickOnNode(Node & node);

    void handleLeftButtonClickOnNodeHandle(NodeHandle & nodeHandle);

    void handleRightButtonClickOnNode(Node & node);

    void openBackgroundContextMenu();

    void openNodeContextMenu();

    void showDummyDragEdge(bool show);

    void showDummyDragNode(bool show);

    void updateScale(int value);

    QMenu m_backgroundContextMenu;

    QMenu m_nodeContextMenu;

    QPoint m_clickedPos;

    QPointF m_clickedScenePos;

    QAction * m_clearComputerHint = nullptr;

    QAction * m_setComputerHintBrakeHard = nullptr;

    QAction * m_setComputerHintBrake = nullptr;

    QAction * m_deleteRow = nullptr;

    QAction * m_deleteCol = nullptr;

    QAction * m_excludeFromMinimap = nullptr;

    QAction * m_forceStationaryAction = nullptr;

    QGraphicsSimpleTextItem * m_helloText = nullptr;

    QPointF m_mappedPos;

    int m_scaleValue = 100;

    Mediator & m_mediator;

    Node * m_dummyDragNode = nullptr;

    Edge * m_dummyDragEdge = nullptr;
};

#endif // EDITORVIEW_HPP
