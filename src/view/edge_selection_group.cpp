// This file is part of Heimer.
// Copyright (C) 2024 Jussi Lind <jussi.lind@iki.fi>
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

#include "edge_selection_group.hpp"

#include "scene_items/edge.hpp"

#include <algorithm>

void EdgeSelectionGroup::add(EdgeR edge, bool isImplicit)
{
    if (!m_implicitlyAdded.count(&edge)) {
        m_implicitlyAdded[&edge] = isImplicit;
        m_edges.push_back(&edge);
        edge.setSelected(true);
    }
}

void EdgeSelectionGroup::clear(bool implicitOnly)
{
    if (implicitOnly) {
        clearImplicitOnly();
    } else {
        clearAll();
    }
}

void EdgeSelectionGroup::clearAll()
{
    for (auto && edge : m_edges) {
        edge->setSelected(false);
    }
    m_implicitlyAdded.clear();
    m_edges.clear();
}

void EdgeSelectionGroup::clearImplicitOnly()
{
    for (auto && edge : m_edges) {
        if (m_implicitlyAdded.count(edge) && m_implicitlyAdded.at(edge)) {
            edge->setSelected(false);
        }
    }
    m_edges.erase(std::remove_if(m_edges.begin(), m_edges.end(), [edgeMap = m_implicitlyAdded](auto && edge) {
                      return edgeMap.count(edge) && edgeMap.at(edge);
                  }),
                  m_edges.end());
    // Note!!: std::remove_if will work on maps in C++20 ->
    for (auto iter = m_implicitlyAdded.begin(); iter != m_implicitlyAdded.end();) {
        if (iter->second) {
            iter = m_implicitlyAdded.erase(iter);
        } else {
            iter++;
        }
    }
}

bool EdgeSelectionGroup::contains(EdgeR edge) const
{
    return m_implicitlyAdded.count(&edge);
}

bool EdgeSelectionGroup::isEmpty() const
{
    return m_implicitlyAdded.empty();
}

const std::vector<EdgeP> EdgeSelectionGroup::edges() const
{
    return m_edges;
}

std::optional<EdgeP> EdgeSelectionGroup::selectedEdge() const
{
    return !m_edges.empty() ? std::optional<EdgeP> { *m_edges.begin() } : std::optional<EdgeP> {};
}

size_t EdgeSelectionGroup::size() const
{
    return m_edges.size();
}

void EdgeSelectionGroup::toggle(EdgeR edge)
{
    if (edge.selected()) {
        m_edges.erase(std::find(m_edges.begin(), m_edges.end(), &edge));
        edge.setSelected(false);
        m_implicitlyAdded.erase(&edge);
    } else {
        add(edge);
    }
}
