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

#ifndef GRAPH_HPP
#define GRAPH_HPP

#include "edge.hpp"
#include "node.hpp"

#include <cstdint>
#include <unordered_map>
#include <vector>

class Node;

class Graph
{
public:
    Graph();

    Graph(const Graph & other) = delete;

    Graph & operator=(const Graph & other) = delete;

    virtual ~Graph();

    using NodeVector = std::vector<NodePtr>;

    using EdgeVector = std::vector<EdgePtr>;

    void clear();

    void addNode(NodePtr node);

    //! "Soft deletes" the given node.
    //! The node gets **really** deleted only when the Graph is deleted.
    //! This is to help integration with Qt that operates only on raw pointers.
    //! \param index Index of the node to be deleted.
    //! \returns The deleted node and connected edges that were also removed.
    std::pair<NodePtr, EdgeVector> deleteNode(int index);

    void addEdge(EdgePtr edge);

    //! "Soft deletes" the given edge.
    //! The edge gets **really** deleted only when the Graph is deleted.
    //! This is to help integration with Qt that operates only on raw pointers.
    //! \param index0 Index of the source node.
    //! \param index1 Index of the target node.
    //! \returns The deleted edge.
    EdgePtr deleteEdge(int index0, int index1);

    bool areDirectlyConnected(NodePtr node0, NodePtr node1);

    bool areDirectlyConnected(int index0, int index1);

    size_t numNodes() const;

    EdgeVector getEdgesFromNode(NodePtr node);

    EdgeVector getEdgesToNode(NodePtr node);

    EdgeVector getEdges() const;

    NodePtr getNode(int index) const;

    NodeVector getNodes() const;

    NodeVector getNodesConnectedToNode(NodePtr node);

private:
    std::unordered_map<int, NodePtr> m_nodes;

    std::unordered_map<int64_t, EdgePtr> m_edges;

    NodeVector m_deletedNodes;

    EdgeVector m_deletedEdges;

    int m_count = 0;
};

#endif // GRAPH_HPP
