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
    node->setIndex(m_count++);
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
    const auto iter = std::find_if(
        m_edges.begin(), m_edges.end(), [=](const EdgeBasePtr & edge){
            return edge->sourceNodeBase().index() == newEdge->sourceNodeBase().index() &&
                   edge->targetNodeBase().index() == newEdge->targetNodeBase().index();
        });

    if (iter == m_edges.end())
    {
        m_edges.push_back(newEdge);
    }
}

#ifdef HEIMER_UNIT_TEST
void Graph::addEdge(int node0, int node1)
{
    // Add if such edge doesn't already exist
    const auto iter = std::find_if(
        m_edges.begin(), m_edges.end(), [=](const EdgeBasePtr & edge){
            return edge->sourceNodeBase().index() == node0 && edge->targetNodeBase().index() == node1;
        });

    if (iter == m_edges.end())
    {
        m_edges.push_back(std::make_shared<EdgeBase>(*getNode(node0), *getNode(node1)));
    }
}
#endif

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

Graph::~Graph()
{
}
