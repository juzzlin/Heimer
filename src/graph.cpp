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

void Graph::deleteEdge(int index0, int index1)
{
    EdgeVector::iterator edgeIter;
    bool edgeErased = false;
    do {
        edgeIter = std::find_if(
          m_edges.begin(), m_edges.end(), [=](const EdgePtr & edge) {
              return edge->sourceNode().index() == index0 && edge->targetNode().index() == index1;
          });
        edgeErased = edgeIter != m_edges.end();
        if (edgeErased) {
            m_edges.erase(edgeIter);
        }
    } while (edgeErased);
}

void Graph::deleteNode(int index)
{
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
                m_edges.erase(edgeIter);
            }
        } while (edgeErased);

        m_nodes.erase(iter);
    }
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

#ifdef HEIMER_UNIT_TEST
void Graph::addEdge(int node0, int node1)
{
    if (node0 == node1) {
        return;
    }

    // Add if such edge doesn't already exist
    if (std::count_if(
          m_edges.begin(), m_edges.end(), [=](const EdgePtr & edge) {
              return edge->sourceNode().index() == node0 && edge->targetNode().index() == node1;
          })
        == 0) {
        m_edges.push_back(std::make_shared<Edge>(*getNode(node0), *getNode(node1)));
    }
}
#endif

bool Graph::areDirectlyConnected(NodePtr node0, NodePtr node1)
{
    for (auto && edge : m_edges) {
        if ((edge->sourceNode().index() == node0->index() && edge->targetNode().index() == node1->index()) || (edge->sourceNode().index() == node1->index() && edge->targetNode().index() == node0->index())) {
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
