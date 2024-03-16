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

#include "node_selection_group.hpp"

#include "scene_items/node.hpp"

#include <algorithm>

void NodeSelectionGroup::add(NodeR node, bool isImplicit)
{
    if (!m_implicitlyAdded.count(&node)) {
        m_implicitlyAdded[&node] = isImplicit;
        m_nodes.push_back(&node);
        node.setSelected(true);
    }
}

void NodeSelectionGroup::clear(bool onlyImplicitNodes)
{
    if (onlyImplicitNodes) {
        clearImplicitOnly();
    } else {
        clearAll();
    }
}

void NodeSelectionGroup::clearAll()
{
    for (auto && node : m_nodes) {
        node->setSelected(false);
    }
    m_implicitlyAdded.clear();
    m_nodes.clear();
}

void NodeSelectionGroup::clearImplicitOnly()
{
    for (auto && node : m_nodes) {
        if (m_implicitlyAdded.count(node) && m_implicitlyAdded.at(node)) {
            node->setSelected(false);
        }
    }
    m_nodes.erase(std::remove_if(m_nodes.begin(), m_nodes.end(), [nodeMap = m_implicitlyAdded](auto && node) {
                      return nodeMap.count(node) && nodeMap.at(node);
                  }),
                  m_nodes.end());
    // Note!!: std::remove_if will work on maps in C++20 ->
    for (auto iter = m_implicitlyAdded.begin(); iter != m_implicitlyAdded.end();) {
        if (iter->second) {
            iter = m_implicitlyAdded.erase(iter);
        } else {
            iter++;
        }
    }
}

bool NodeSelectionGroup::contains(NodeR node) const
{
    return m_implicitlyAdded.count(&node);
}

bool NodeSelectionGroup::isEmpty() const
{
    return m_implicitlyAdded.empty();
}

void NodeSelectionGroup::move(NodeR reference, QPointF location)
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

const std::vector<NodeP> NodeSelectionGroup::nodes() const
{
    return m_nodes;
}

std::optional<NodeP> NodeSelectionGroup::selectedNode() const
{
    return !m_nodes.empty() ? std::optional<NodeP> { *m_nodes.begin() } : std::optional<NodeP> {};
}

size_t NodeSelectionGroup::size() const
{
    return m_nodes.size();
}

void NodeSelectionGroup::toggle(NodeR node)
{
    if (node.selected()) {
        m_nodes.erase(std::find(m_nodes.begin(), m_nodes.end(), &node));
        node.setSelected(false);
        m_implicitlyAdded.erase(&node);
    } else {
        add(node);
    }
}
