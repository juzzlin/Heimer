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

#ifndef NODE_HPP
#define NODE_HPP

#include <QObject>
#include <QGraphicsItem>
#include <QTimer>

#include <vector>
#include <map>

#include "nodebase.hpp"
#include "edge.hpp"

class NodeHandle;
class QGraphicsTextItem;
class TextEdit;

//! Freely placeable target node.
class Node : public QObject, public QGraphicsItem, public NodeBase
{
    Q_OBJECT

    Q_INTERFACES(QGraphicsItem)

public:

    //! Constructor.
    Node();

    //! Copy constructor.
    Node(const Node & other);

    virtual ~Node();

    virtual void addGraphicsEdge(Edge & edge);

    virtual void removeGraphicsEdge(Edge & edge);

    void adjustSize();

    virtual QRectF boundingRect() const override;

    using NodePtr = std::shared_ptr<Node>;
    virtual EdgePtr createAndAddGraphicsEdge(NodePtr targetNode);

    virtual void paint(QPainter * painter,
        const QStyleOptionGraphicsItem * option, QWidget * widget = nullptr) override;

    //! Sets the Node and QGraphicsItem locations.
    virtual void setLocation(QPointF newLocation);

    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event) override;

    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event) override;

    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent * event) override;

    static std::pair<QPointF, QPointF> getNearestEdgePoints(const Node & node1, const Node & node2);

    void setHandlesVisible(bool visible, bool all = true);

    QString text() const override;

    void setText(const QString & text) override;

    void setTextColor(const QColor & color) override;

signals:

    void undoPointRequested();

private:

    void checkHandleVisibility(QPointF pos);

    void createEdgePoints();

    void createHandles();

    NodeHandle * hitsHandle(QPointF pos);

    void initTextField();

    bool isTextUnderflowOrOverflow() const;

    void updateEdgeLines();

    std::vector<NodeHandle *> m_handles;

    std::vector<Edge *> m_graphicsEdges;

    std::vector<QPointF> m_edgePoints;

    const int m_handleRadius = 28;

    const int m_handleRadiusSmall = 20;

    const int m_margin = 10;

    const float m_minHeight = 75;

    const float m_minWidth = 200;

    TextEdit * m_textEdit;

    QTimer m_handleVisibilityTimer;

    QPointF m_currentMousePos;

    bool m_mouseIn = false;
};

using NodePtr = std::shared_ptr<Node>;

#endif // NODE_HPP
