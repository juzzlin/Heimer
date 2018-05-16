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
    if (node && m_nodes.count(node->index()) == 0)
    {
        node->setIndex(m_count);
        m_nodes.insert({node->index(), node});
        m_count++;
    }
}

void Graph::addEdge(EdgeBasePtr newEdge)
{
    // Add if such edge doesn't already exist
    auto && edges = m_edges[newEdge->sourceNodeBase().index()];
    const auto iter = std::find_if(
        edges.begin(), edges.end(), [=](const EdgeBasePtr & edge){
            return edge->sourceNodeBase().index() == newEdge->sourceNodeBase().index() &&
                   edge->targetNodeBase().index() == newEdge->targetNodeBase().index();
        });

    if (iter == edges.end())
    {
        m_edges[newEdge->sourceNodeBase().index()].insert(newEdge);
    }
}

#ifdef HEIMER_UNIT_TEST
void Graph::addEdge(int node0, int node1)
{
    // Add if such edge doesn't already exist
    auto && edges = m_edges[node0];
    const auto iter = std::find_if(
        edges.begin(), edges.end(), [=](const EdgeBasePtr & edge){
            return edge->sourceNodeBase().index() == node0 && edge->targetNodeBase().index() == node1;
        });

    if (iter == edges.end())
    {
        m_edges[node0].insert(std::make_shared<EdgeBase>(*getNode(node0), *getNode(node1)));
    }
}
#endif

int Graph::numNodes() const
{
    return m_nodes.size();
}

Graph::EdgeVector Graph::getEdges() const
{
    EdgeVector edges;
    for (auto && node0 : m_edges)
    {
        for (auto && edge : node0.second)
        {
            edges.push_back(edge);
        }
    }
    return edges;
}

Graph::EdgeSet Graph::getEdgesFromNode(NodeBasePtr node)
{
    return m_edges[node->index()];
}

NodeBasePtr Graph::getNode(int index)
{
    return m_nodes.count(index) ? m_nodes[index] : NodeBasePtr();
}

Graph::NodeVector Graph::getNodes() const
{
    NodeVector nodes;
    for (auto const & iter : m_nodes)
    {
        nodes.push_back(iter.second);
    }

    return nodes;
}

Graph::~Graph()
{
}
