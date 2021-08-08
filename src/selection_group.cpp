// This file is part of Heimer.
// Copyright (C) 2019 Jussi Lind <jussi.lind@iki.fi>
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

#include "selection_group.hpp"

#include "node.hpp"

#include <algorithm>

void SelectionGroup::addSelectedNode(Node & node)
{
    if (!m_nodeSet.count(&node)) {
        m_nodeSet.insert(&node);
        m_nodes.push_back(&node);
        node.setSelected(true);
    }
}

void SelectionGroup::clear()
{
    for (auto && node : m_nodes) {
        node->setSelected(false);
    }
    m_nodeSet.clear();
    m_nodes.clear();
}

bool SelectionGroup::hasNode(Node & node) const
{
    return m_nodeSet.count(&node);
}

void SelectionGroup::move(Node & reference, QPointF location)
{
    std::map<int, QPointF> delta;
    for (auto && node : m_nodes) {
        if (node->index() != reference.index()) {
            delta[node->index()] = node->location() - reference.location();
        }
    }

    reference.setLocation(location);

    for (auto && node : m_nodes) {
        if (node->index() != reference.index()) {
            node->setLocation(reference.location() + delta[node->index()]);
        }
    }
}

const std::vector<Node *> & SelectionGroup::nodes() const
{
    return m_nodes;
}

Node * SelectionGroup::selectedNode() const
{
    return !m_nodes.empty() ? *m_nodes.begin() : nullptr;
}

size_t SelectionGroup::size() const
{
    return m_nodes.size();
}

void SelectionGroup::toggleNode(Node & node)
{
    if (node.selected()) {
        m_nodes.erase(std::find(m_nodes.begin(), m_nodes.end(), &node));
        node.setSelected(false);
        m_nodeSet.erase(&node);
    } else {
        addSelectedNode(node);
    }
}
