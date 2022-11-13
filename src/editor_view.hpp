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

#ifndef EDITOR_VIEW_HPP
#define EDITOR_VIEW_HPP

#include "constants.hpp"
#include "grid.hpp"
#include "main_context_menu.hpp"
#include "state_machine.hpp"
#include "types.hpp"

#include <QColor>
#include <QGraphicsView>
#include <QMenu>

#include <memory>
#include <optional>
#include <set>

class ControlStrategy;
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

    const Grid & grid() const;

    void resetDummyDragItems();

    void restoreZoom();

    void saveZoom();

    void zoom(double amount);

    void zoomToFit(QRectF nodeBoundingRect);

    QString dropFile() const;

public slots:
    void setArrowSize(double arrowSize);

    void setCornerRadius(int cornerRadius);

    void setEdgeColor(const QColor & edgeColor);

    void setEdgeWidth(double edgeWidth);

    void setGridColor(const QColor & edgeColor);

    void setGridSize(int size);

    void setGridVisible(bool visible);

protected:
    void mouseDoubleClickEvent(QMouseEvent * event) override;

    void mouseMoveEvent(QMouseEvent * event) override;

    void mousePressEvent(QMouseEvent * event) override;

    void mouseReleaseEvent(QMouseEvent * event) override;

    void wheelEvent(QWheelEvent * event) override;

    void dropEvent(QDropEvent * event) override;

    void dragMoveEvent(QDragMoveEvent * event) override;
signals:

    void actionTriggered(StateMachine::Action action);

    void newNodeRequested(QPointF position);

private:
    void finishRubberBand();

    void handleMousePressEventOnBackground(QMouseEvent & event);

    void handleMousePressEventOnEdge(QMouseEvent & event, EdgeR edge);

    void handleMousePressEventOnNode(QMouseEvent & event, NodeR node);

    void handleMousePressEventOnNodeHandle(QMouseEvent & event, NodeHandle & nodeHandle);

    void handlePrimaryButtonClickOnNode(NodeR node);

    void handlePrimaryButtonClickOnNodeHandle(NodeHandle & nodeHandle);

    void handleSecondaryButtonClickOnEdge(EdgeR edge);

    void handleSecondaryButtonClickOnNode(NodeR node);

    void initiateBackgroundDrag();

    void initiateRubberBand();

    bool isModifierPressed() const;

    void openBackgroundContextMenu();

    void openEdgeContextMenu();

    void openMainContextMenu(MainContextMenu::Mode mode);

    void showDummyDragEdge(bool show);

    void showDummyDragNode(bool show);

    void updateScale();

    void updateRubberBand();

    void drawBackground(QPainter * painter, const QRectF & rect) override;

    Grid m_grid;

    QPoint m_clickedPos;

    QAction * m_doubleArrowAction = nullptr;

    QAction * m_singleArrowAction = nullptr;

    QAction * m_changeEdgeDirectionAction = nullptr;

    QAction * m_hideEdgeArrowAction = nullptr;

    QAction * m_deleteEdgeAction = nullptr;

    QPointF m_pos;

    QPointF m_mappedPos;

    double m_scale = 1.0;

    Mediator & m_mediator;

    NodeU m_dummyDragNode;

    EdgeU m_dummyDragEdge;

    NodeS m_connectionTargetNode;

    int m_cornerRadius = 0;

    // Arrow size for the dummy drag edge
    double m_arrowSize = Constants::Edge::Defaults::ARROW_SIZE;

    // Width for the dummy drag edge
    double m_edgeWidth = Constants::MindMap::Defaults::EDGE_WIDTH;

    QColor m_edgeColor;

    QRectF m_nodeBoundingRect;

    QRubberBand * m_rubberBand = nullptr;

    EdgeContextMenu * m_edgeContextMenu;

    MainContextMenu * m_mainContextMenu;

    bool m_gridVisible = false;

    QString m_dropFile {};

    struct ZoomParameters
    {
        QRectF sceneRect;

        QPointF viewCenter;

        double scale = 1.0;
    };

    std::optional<ZoomParameters> m_savedZoom;

    std::unique_ptr<ControlStrategy> m_controlStrategy;
};

#endif // EDITOR_VIEW_HPP
