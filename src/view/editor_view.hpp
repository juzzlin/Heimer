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

#include "../application/state_machine.hpp"
#include "../common/constants.hpp"
#include "../common/types.hpp"
#include "grid.hpp"
#include "menus/main_context_menu.hpp"

#include <QColor>
#include <QGraphicsView>
#include <QMenu>
#include <QTimer>

#include <optional>

class ControlStrategy;
class MindMapTile;
class Object;
class ObjectModelLoaderoader;
class QAction;
class QMouseEvent;
class QPaintEvent;
class QWheelEvent;
class QGraphicsSimpleTextItem;
class QRubberBand;

namespace Menus {
class EdgeContextMenu;
}

namespace SceneItems {
class Edge;
class Node;
class NodeHandle;
} // namespace SceneItems

class EditorView : public QGraphicsView
{
    Q_OBJECT

public:
    EditorView();

    ~EditorView() override;

    const Grid & grid() const;

    void resetDummyDragItems();

    void restoreZoom();

    void saveZoom();

    void showStatusText(QString statusText);

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
    void drawGrid(QPainter & painter, const QRectF & sceneRect);

    void finishRubberBand();

    void handleMousePressEventOnBackground(QMouseEvent & event);

    //! \returns true if accepted.
    bool handleMousePressEventOnEdge(QMouseEvent & event, EdgeR edge);

    void handleMousePressEventOnNode(QMouseEvent & event, NodeR node);

    void handleMousePressEventOnNodeHandle(QMouseEvent & event, SceneItems::NodeHandle & nodeHandle);

    void handlePrimaryButtonClickOnNode(NodeR node);

    void handlePrimaryButtonClickOnNodeHandle(SceneItems::NodeHandle & nodeHandle);

    void handleSecondaryButtonClickOnEdge(EdgeR edge);

    void handleSecondaryButtonClickOnNode(NodeR node);

    void initiateBackgroundDrag();

    void initiateRubberBand();

    bool isModifierPressed() const;

    void openBackgroundContextMenu();

    void openEdgeContextMenu();

    void openMainContextMenu(Menus::MainContextMenu::Mode mode);

    void removeShadowEffectsDuringDrag();

    void showDummyDragEdge(bool show);

    void showDummyDragNode(bool show);

    void updateScale();

    void updateShadowEffectsBasedOnItemVisiblity();

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

    NodeU m_dummyDragNode;

    EdgeU m_dummyDragEdge;

    NodeS m_connectionTargetNode;

    int m_cornerRadius = 0;

    // Arrow size for the dummy drag edge
    double m_arrowSize = Constants::Settings::defaultArrowSize();

    // Width for the dummy drag edge
    double m_edgeWidth = Constants::Settings::defaultEdgeWidth();

    QColor m_edgeColor;

    QRectF m_nodeBoundingRect;

    QRubberBand * m_rubberBand = nullptr;

    Menus::EdgeContextMenu * m_edgeContextMenu;

    Menus::MainContextMenu * m_mainContextMenu;

    bool m_gridVisible = false;

    bool m_shadowEffectsDuringDragRemoved = false;

    QString m_dropFile {};

    struct ZoomParameters
    {
        QRectF sceneRect;

        QPointF viewCenter;

        double scale = 1.0;
    };

    std::optional<ZoomParameters> m_savedZoom;

    ControlStrategyS m_controlStrategy;

    QTimer m_updateShadowEffectsOnZoomTimer;
};

#endif // EDITOR_VIEW_HPP
