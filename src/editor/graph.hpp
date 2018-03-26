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

#ifndef GRAPH_HPP
#define GRAPH_HPP

#include "nodebase.hpp"

#include <map>
#include <set>

class NodeBase;

class Graph
{
public:

    Graph();

    Graph(const Graph & other) = delete;

    Graph & operator= (const Graph & other) = delete;

    virtual ~Graph();

    using NodeVector = std::vector<NodeBasePtr>;

    using NodeMap = std::map<int, NodeBasePtr>;

    using Edge = std::pair<int, int>;

    using EdgeSet = std::set<int>;

    using EdgeVector = std::vector<Edge>;

    void clear();

    void addNode(NodeBasePtr node);

    void addEdge(NodeBasePtr node0, NodeBasePtr node1);

    void addEdge(int node0, int node1);

    int numNodes() const;

    EdgeSet getEdgesFromNode(NodeBasePtr node);

    EdgeVector getEdges() const;

    NodeBasePtr getNode(int index);

    NodeVector getNodes() const;

private:

    std::map<int, NodeBasePtr> m_nodes;

    std::map<int, EdgeSet> m_edges;

    int m_count = 0;
};

#endif // GRAPH_HPP
