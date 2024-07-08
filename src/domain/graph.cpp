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

#include "simple_logger.hpp"

#include <algorithm>
#include <stdexcept>
#include <string>

static const auto TAG = "Graph";

Graph::Graph() = default;

void Graph::clear()
{
    m_edges.clear();
    m_deletedEdges.clear();
    m_nodes.clear();
    m_deletedNodes.clear();
}

void Graph::addNode(NodeS node)
{
    if (node->index() == -1) {
        node->setIndex(m_count++);
    } else {
        if (node->index() >= m_count) {
            m_count = node->index() + 1;
        }
    }

    m_nodes[node->index()] = node;
}

EdgeS Graph::deleteEdge(int index0, int index1)
{
    EdgeS deletedEdge;
    if (const auto edgeIter = m_edges.find(buildKeyFromIndices(index0, index1)); edgeIter != m_edges.end()) {
        deletedEdge = (*edgeIter).second;
        m_deletedEdges.push_back(deletedEdge);
        m_edges.erase(edgeIter);
    }
    return deletedEdge;
}

std::pair<NodeS, Graph::EdgeVector> Graph::deleteNode(int index)
{
    NodeS deletedNode;
    Graph::EdgeVector deletedEdges;
    if (const auto iter = m_nodes.find(index); iter != m_nodes.end()) {
        auto edgeIter = m_edges.begin();
        while (edgeIter != m_edges.end()) {
            if (const auto edgePair = *edgeIter; edgePair.second->sourceNode().index() == index || edgePair.second->targetNode().index() == index) {
                deletedEdges.push_back(edgePair.second);
                m_deletedEdges.push_back(edgePair.second);
                edgeIter = m_edges.erase(edgeIter);
            } else {
                edgeIter++;
            }
        }
        deletedNode = iter->second;
        m_deletedNodes.push_back(deletedNode);
        m_nodes.erase(iter);
    }

    return { deletedNode, deletedEdges };
}

void Graph::addEdge(EdgeS newEdge)
{
    // Add if such edge doesn't already exist
    const auto c0 = newEdge->sourceNode().index();
    const auto c1 = newEdge->targetNode().index();
    if (!m_edges.count(buildKeyFromIndices(c0, c1))) {
        m_edges.insert({ buildKeyFromIndices(c0, c1), newEdge });
    }
}

size_t Graph::edgeCount() const
{
    return m_edges.size();
}

bool Graph::areDirectlyConnected(NodeS node0, NodeS node1) const
{
    return areDirectlyConnected(node0->index(), node1->index());
}

bool Graph::areDirectlyConnected(int index0, int index1) const
{
    return m_edges.count(buildKeyFromIndices(index0, index1)) || m_edges.count(buildKeyFromIndices(index1, index0));
}

size_t Graph::nodeCount() const
{
    return m_nodes.size();
}

EdgeS Graph::getEdge(int index0, int index1) const
{
    const auto iter = m_edges.find(buildKeyFromIndices(index0, index1));
    return iter != m_edges.end() ? iter->second : nullptr;
}

Graph::EdgeVector Graph::getEdges() const
{
    EdgeVector edges(m_edges.size());
    std::transform(std::begin(m_edges), std::end(m_edges), std::begin(edges), [](auto && edge) { return edge.second; });
    return edges;
}

Graph::EdgeVector Graph::getEdgesFromNode(NodeS node) const
{
    EdgeVector edges;
    for (auto && edge : m_edges) {
        if (edge.second->sourceNode().index() == node->index()) {
            edges.push_back(edge.second);
        }
    }
    return edges;
}

Graph::EdgeVector Graph::getEdgesToNode(NodeS node) const
{
    Graph::EdgeVector edges;
    for (auto && edge : m_edges) {
        if (edge.second->targetNode().index() == node->index()) {
            edges.push_back(edge.second);
        }
    }
    return edges;
}

NodeS Graph::getNode(int index) const
{
    if (const auto iter = m_nodes.find(index); iter != m_nodes.end()) {
        return iter->second;
    }
    throw std::runtime_error("Invalid node index: " + std::to_string(index));
}

Graph::NodeVector Graph::getNodes() const
{
    NodeVector nodes(m_nodes.size());
    std::transform(std::begin(m_nodes), std::end(m_nodes), std::begin(nodes), [](auto && node) { return node.second; });
    return nodes;
}

Graph::NodeVector Graph::getNodesConnectedToNode(NodeS node) const
{
    NodeVector result;
    const auto edgesToNode = getEdgesToNode(node);
    std::transform(std::begin(edgesToNode), std::end(edgesToNode), std::back_inserter(result), [this](auto && edge) { return getNode(edge->sourceNode().index()); });

    const auto edgesFromNode = getEdgesFromNode(node);
    std::transform(std::begin(edgesFromNode), std::end(edgesFromNode), std::back_inserter(result), [this](auto && edge) { return getNode(edge->targetNode().index()); });

    return result;
}

int64_t Graph::buildKeyFromIndices(int index0, int index1) const
{
    return (int64_t(index0) << 32) + index1;
}

Graph::~Graph()
{
    // Ensure that edges are always deleted before nodes
    clear();

    juzzlin::L(TAG).debug() << "Graph deleted";
}
