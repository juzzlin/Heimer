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

Graph::Graph()
{
}

void Graph::clear()
{
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
    EdgeVector::iterator edgeIter;
    bool edgeErased = false;
    do {
        edgeIter = std::find_if(
          m_edges.begin(), m_edges.end(), [=](const EdgePtr & edge) {
              return edge->sourceNode().index() == index0 && edge->targetNode().index() == index1;
          });
        edgeErased = edgeIter != m_edges.end();
        if (edgeErased) {
            deletedEdge = *edgeIter;
            m_deletedEdges.push_back(*edgeIter);
            m_edges.erase(edgeIter);
        }
    } while (edgeErased);

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
        EdgeVector::iterator edgeIter;
        bool edgeErased = false;
        do {
            edgeIter = std::find_if(
              m_edges.begin(), m_edges.end(), [=](const EdgePtr & edge) {
                  return edge->sourceNode().index() == index || edge->targetNode().index() == index;
              });
            edgeErased = edgeIter != m_edges.end();
            if (edgeErased) {
                deletedEdges.push_back(*edgeIter);
                m_deletedEdges.push_back(*edgeIter);
                m_edges.erase(edgeIter);
            }
        } while (edgeErased);

        deletedNode = *iter;
        m_deletedNodes.push_back(deletedNode);
        m_nodes.erase(iter);
    }

    return { deletedNode, deletedEdges };
}

void Graph::addEdge(EdgePtr newEdge)
{
    // Add if such edge doesn't already exist
    if (std::count_if(
          m_edges.begin(), m_edges.end(), [=](const EdgePtr & edge) {
              return edge->sourceNode().index() == newEdge->sourceNode().index() && edge->targetNode().index() == newEdge->targetNode().index();
          })
        == 0) {
        m_edges.push_back(newEdge);
    }
}

bool Graph::areDirectlyConnected(NodePtr node0, NodePtr node1)
{
    return areDirectlyConnected(node0->index(), node1->index());
}

bool Graph::areDirectlyConnected(int index0, int index1)
{
    for (auto && edge : m_edges) {
        if ((edge->sourceNode().index() == index0 && edge->targetNode().index() == index1) || (edge->sourceNode().index() == index1 && edge->targetNode().index() == index0)) {
            return true;
        }
    }
    return false;
}

size_t Graph::numNodes() const
{
    return m_nodes.size();
}

const Graph::EdgeVector & Graph::getEdges() const
{
    return m_edges;
}

Graph::EdgeVector Graph::getEdgesFromNode(NodePtr node)
{
    Graph::EdgeVector edges;
    for (auto && edge : m_edges) {
        if (edge->sourceNode().index() == node->index()) {
            edges.push_back(edge);
        }
    }
    return edges;
}

Graph::EdgeVector Graph::getEdgesToNode(NodePtr node)
{
    Graph::EdgeVector edges;
    for (auto && edge : m_edges) {
        if (edge->targetNode().index() == node->index()) {
            edges.push_back(edge);
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
    m_edges.clear();
    m_nodes.clear();

    juzzlin::L().debug() << "Graph deleted";
}
