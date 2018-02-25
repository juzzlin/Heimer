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

#ifndef EDGE_HPP
#define EDGE_HPP

#include <QGraphicsLineItem>
#include <memory>

class Node;

//! A graphic representation of a graph edge between nodes.
class Edge : public QGraphicsLineItem
{
public:

    Edge(Node & sourceNode, Node & targetNode);

    void setSourceNode(Node & sourceNode);

    void setTargetNode(Node & targetNode);

    void updateLine();

private:

    Node * m_sourceNode;

    Node * m_targetNode;
};

using EdgePtr = std::shared_ptr<Edge>;

#endif // EDGE_HPP
