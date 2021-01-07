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

#include "graph.hpp"
#include "node.hpp"
#include "test_mode.hpp"

#include "simple_logger.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include <string>

namespace {
int64_t getKey(int c0, int c1)
{
    return (int64_t(c0) << 32) + c1;
}
} // namespace

Graph::Graph()
{
}

void Graph::clear()
{
    m_edges.clear();
    m_nodes.clear();
}

void Graph::addNode(NodePtr node)
{
    if (node->index() == -1) {
        node->setIndex(m_count++);
    } else {
        if (node->index() >= m_count) {
            m_count = node->index() + 1;
        }
    }

    m_nodes.push_back(node);
}

EdgePtr Graph::deleteEdge(int index0, int index1)
{
    EdgePtr deletedEdge;
    const auto edgeIter = m_edges.find(getKey(index0, index1));
    if (edgeIter != m_edges.end()) {
        deletedEdge = (*edgeIter).second;
        m_deletedEdges.push_back(deletedEdge);
        m_edges.erase(edgeIter);
    }
    return deletedEdge;
}

std::pair<NodePtr, Graph::EdgeVector> Graph::deleteNode(int index)
{
    NodePtr deletedNode;
    Graph::EdgeVector deletedEdges;
    const auto iter = std::find_if(m_nodes.begin(), m_nodes.end(), [=](const NodePtr & node) {
        return node->index() == index;
    });

    if (iter != m_nodes.end()) {
        auto edgeIter = m_edges.begin();
        while (edgeIter != m_edges.end()) {
            const auto edgePair = *edgeIter;
            if (edgePair.second->sourceNode().index() == index || edgePair.second->targetNode().index() == index) {
                edgeIter = m_edges.erase(edgeIter);
                deletedEdges.push_back(edgePair.second);
            } else {
                edgeIter++;
            }
        }
        deletedNode = *iter;
        m_deletedNodes.push_back(deletedNode);
        m_nodes.erase(iter);
    }

    return { deletedNode, deletedEdges };
}

void Graph::addEdge(EdgePtr newEdge)
{
    // Add if such edge doesn't already exist
    const auto c0 = newEdge->sourceNode().index();
    const auto c1 = newEdge->targetNode().index();
    if (!m_edges.count(getKey(c0, c1))) {
        m_edges.insert({ getKey(c0, c1), newEdge });
    }
}

bool Graph::areDirectlyConnected(NodePtr node0, NodePtr node1)
{
    return areDirectlyConnected(node0->index(), node1->index());
}

bool Graph::areDirectlyConnected(int index0, int index1)
{
    return m_edges.count(getKey(index0, index1)) || m_edges.count(getKey(index1, index0));
}

size_t Graph::numNodes() const
{
    return m_nodes.size();
}

Graph::EdgeVector Graph::getEdges() const
{
    EdgeVector edges;
    edges.reserve(m_edges.size());
    for (auto && edge : m_edges) {
        edges.push_back(edge.second);
    }
    return edges;
}

Graph::EdgeVector Graph::getEdgesFromNode(NodePtr node)
{
    EdgeVector edges;
    for (auto && edge : m_edges) {
        if (edge.second->sourceNode().index() == node->index()) {
            edges.push_back(edge.second);
        }
    }
    return edges;
}

Graph::EdgeVector Graph::getEdgesToNode(NodePtr node)
{
    Graph::EdgeVector edges;
    for (auto && edge : m_edges) {
        if (edge.second->targetNode().index() == node->index()) {
            edges.push_back(edge.second);
        }
    }
    return edges;
}

NodePtr Graph::getNode(int index)
{
    auto iter = std::find_if(m_nodes.begin(), m_nodes.end(), [=](const NodePtr & node) {
        return node->index() == index;
    });
    if (iter != m_nodes.end()) {
        return *iter;
    }
    throw std::runtime_error("Invalid node index: " + std::to_string(index));
}

const Graph::NodeVector & Graph::getNodes() const
{
    return m_nodes;
}

Graph::NodeVector Graph::getNodesConnectedToNode(NodePtr node)
{
    NodeVector result;

    auto && from = getEdgesFromNode(node);
    auto && to = getEdgesToNode(node);

    for (auto && edge : to) {
        result.push_back(getNode(edge->sourceNode().index()));
    }

    for (auto && edge : from) {
        result.push_back(getNode(edge->targetNode().index()));
    }

    return result;
}

Graph::~Graph()
{
    // Ensure that edges are always deleted before nodes
    clear();

    juzzlin::L().debug() << "Graph deleted";
}
