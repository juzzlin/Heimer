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

void Graph::addEdge(NodeBasePtr node0, NodeBasePtr node1)
{

}

int Graph::numNodes() const
{
    return m_nodes.size();
}

NodeBasePtr Graph::get(int id)
{
    if (m_nodes.count(id))
    {
        return m_nodes[id];
    }

    return NodeBasePtr();
}

Graph::NodeVector Graph::getAll() const
{
    NodeVector nodes;
    for (auto const & iter : m_nodes)
    {
        nodes.push_back(iter.second);
    }

    return nodes;
}

Graph::NodeMap::iterator Graph::begin()
{
    return m_nodes.begin();
}

Graph::NodeMap::iterator Graph::end()
{
    return m_nodes.end();
}

Graph::NodeMap::const_iterator Graph::cbegin() const
{
    return m_nodes.cbegin();
}

Graph::NodeMap::const_iterator Graph::cend() const
{
    return m_nodes.cend();
}

Graph::~Graph()
{
}
