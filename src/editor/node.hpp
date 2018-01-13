// This file is part of Dementia.
// Copyright (C) 2018 Jussi Lind <jussi.lind@iki.fi>
//
// Dementia is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dementia is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dementia. If not, see <http://www.gnu.org/licenses/>.

#ifndef NODE_HPP
#define NODE_HPP

#include <QGraphicsItem>
#include <vector>

#include "nodebase.hpp"
#include "edge.hpp"

class NodeHandle;

//! Freely placeable target node.
class Node : public QGraphicsItem, public NodeBase
{
public:

    //! Constructor.
    Node();

    //! Copy constructor.
    Node(const Node & other);

    virtual void addEdge(EdgePtr edge);

    virtual QRectF boundingRect () const override;

    using NodePtr = std::shared_ptr<Node>;
    virtual EdgePtr createAndAddEdge(NodePtr targetNode);

    virtual void paint(QPainter * painter,
        const QStyleOptionGraphicsItem * option, QWidget * widget = nullptr) override;

    //! Sets the Node and QGraphicsItem locations.
    virtual void setLocation(QPointF newLocation);

    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event) override;

    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event) override;

private:

    void createHandles();

    void setHandlesVisible(bool visible);

    std::vector<NodeHandle *> m_handles;

    std::vector<EdgePtr> m_edges;

    const int m_handleRadius = 32;
};

using NodePtr = std::shared_ptr<Node>;

#endif // NODE_HPP
