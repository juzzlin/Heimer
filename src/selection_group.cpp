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

#include "scene_items/node.hpp"

#include <algorithm>

void SelectionGroup::addNode(NodeR node, bool isImplicit)
{
    if (!m_nodeMap.count(&node)) {
        m_nodeMap[&node] = isImplicit;
        m_nodes.push_back(&node);
        node.setSelected(true);
    }
}

void SelectionGroup::clear(bool onlyImplicitNodes)
{
    if (onlyImplicitNodes) {
        clearOnlyImplicitNodes();
    } else {
        clearAll();
    }
}

void SelectionGroup::clearAll()
{
    for (auto && node : m_nodes) {
        node->setSelected(false);
    }
    m_nodeMap.clear();
    m_nodes.clear();
}

void SelectionGroup::clearOnlyImplicitNodes()
{
    for (auto && node : m_nodes) {
        if (m_nodeMap.count(node) && m_nodeMap.at(node)) {
            node->setSelected(false);
        }
    }
    m_nodes.erase(std::remove_if(m_nodes.begin(), m_nodes.end(), [nodeMap = m_nodeMap](auto && node) {
                      return nodeMap.count(node) && nodeMap.at(node);
                  }),
                  m_nodes.end());
    // Note!!: std::remove_if will work on maps in C++20 ->
    for (auto iter = m_nodeMap.begin(); iter != m_nodeMap.end();) {
        if (iter->second) {
            iter = m_nodeMap.erase(iter);
        } else {
            iter++;
        }
    }
}

bool SelectionGroup::hasNode(NodeR node) const
{
    return m_nodeMap.count(&node);
}

bool SelectionGroup::isEmpty() const
{
    return m_nodeMap.empty();
}

void SelectionGroup::move(NodeR reference, QPointF location)
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

const std::vector<NodeP> SelectionGroup::nodes() const
{
    return m_nodes;
}

NodeP SelectionGroup::selectedNode() const
{
    return !m_nodes.empty() ? *m_nodes.begin() : nullptr;
}

size_t SelectionGroup::size() const
{
    return m_nodes.size();
}

void SelectionGroup::toggleNode(NodeR node)
{
    if (node.selected()) {
        m_nodes.erase(std::find(m_nodes.begin(), m_nodes.end(), &node));
        node.setSelected(false);
        m_nodeMap.erase(&node);
    } else {
        addNode(node);
    }
}
