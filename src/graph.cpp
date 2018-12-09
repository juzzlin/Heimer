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
#include "nodebase.hpp"

#include "simple_logger.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>

Graph::Graph()
{
}

void Graph::clear()
{
    m_nodes.clear();
}

void Graph::addNode(NodeBasePtr node)
{
    if (node->index() == -1)
    {
        node->setIndex(m_count++);
    }
    else
    {
        if (node->index() >= m_count)
        {
            m_count = node->index() + 1;
        }
    }

    m_nodes.push_back(node);
}

void Graph::deleteNode(int index)
{
    const auto iter = std::find_if(m_nodes.begin(), m_nodes.end(), [=] (const NodeBasePtr & node) {
        return node->index() == index;
    });

    if (iter != m_nodes.end())
    {
        EdgeVector::iterator edgeIter;
        bool edgeErased = false;
        do
        {
            edgeIter = std::find_if(
                m_edges.begin(), m_edges.end(), [=](const EdgeBasePtr & edge){
                    return edge->sourceNodeBase().index() == index ||
                           edge->targetNodeBase().index() == index;
                });
            edgeErased = edgeIter != m_edges.end();
            if (edgeErased)
            {
                m_edges.erase(edgeIter);
            }
        } while (edgeErased);

        m_nodes.erase(iter);
    }
}

void Graph::addEdge(EdgeBasePtr newEdge)
{
    // Add if such edge doesn't already exist
    if (std::count_if(
        m_edges.begin(), m_edges.end(), [=](const EdgeBasePtr & edge){
            return edge->sourceNodeBase().index() == newEdge->sourceNodeBase().index() &&
                   edge->targetNodeBase().index() == newEdge->targetNodeBase().index();
        }) == 0)
    {
        m_edges.push_back(newEdge);
    }
}

#ifdef HEIMER_UNIT_TEST
void Graph::addEdge(int node0, int node1)
{
    // Add if such edge doesn't already exist
    if (std::count_if(
        m_edges.begin(), m_edges.end(), [=](const EdgeBasePtr & edge){
            return edge->sourceNodeBase().index() == node0 && edge->targetNodeBase().index() == node1;
        }) == 0)
    {
        m_edges.push_back(std::make_shared<EdgeBase>(*getNode(node0), *getNode(node1)));
    }
}
#endif

bool Graph::areDirectlyConnected(NodeBasePtr node0, NodeBasePtr node1)
{
    for (auto && edge : m_edges)
    {
        if ((edge->sourceNodeBase().index() == node0->index() && edge->targetNodeBase().index() == node1->index()) ||
            (edge->sourceNodeBase().index() == node1->index() && edge->targetNodeBase().index() == node0->index()))
        {
          return true;
        }
    }
    return false;
}

int Graph::numNodes() const
{
    return m_nodes.size();
}

const Graph::EdgeVector & Graph::getEdges() const
{
    return m_edges;
}

Graph::EdgeVector Graph::getEdgesFromNode(NodeBasePtr node)
{
    Graph::EdgeVector edges;
    for (auto && edge : m_edges)
    {
        if (edge->sourceNodeBase().index() == node->index())
        {
            edges.push_back(edge);
        }
    }
    return edges;
}

Graph::EdgeVector Graph::getEdgesToNode(NodeBasePtr node)
{
    Graph::EdgeVector edges;
    for (auto && edge : m_edges)
    {
        if (edge->targetNodeBase().index() == node->index())
        {
            edges.push_back(edge);
        }
    }
    return edges;
}

NodeBasePtr Graph::getNode(int index)
{
    auto iter = std::find_if(m_nodes.begin(), m_nodes.end(), [=] (const NodeBasePtr & node) {
        return node->index() == index;
    });
    return iter != m_nodes.end() ? *iter : NodeBasePtr();
}

const Graph::NodeVector & Graph::getNodes() const
{
    return m_nodes;
}

Graph::NodeVector Graph::getNodesConnectedToNode(NodeBasePtr node)
{
    NodeVector result;

    auto && from = getEdgesFromNode(node);
    auto && to = getEdgesToNode(node);

    for (auto && edge : to)
    {
        result.push_back(getNode(edge->sourceNodeBase().index()));
    }

    for (auto && edge : from)
    {
        result.push_back(getNode(edge->targetNodeBase().index()));
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
