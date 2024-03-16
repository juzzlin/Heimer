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

#ifndef EDGE_SELECTION_GROUP_HPP
#define EDGE_SELECTION_GROUP_HPP

#include <QPointF>

#include <optional>
#include <unordered_map>
#include <vector>

#include "../common/types.hpp"

class EdgeSelectionGroup
{
public:
    //! Adds edge to the selection group.
    //! \param edge Edge to be added.
    //! \param isImplicit Tells if the edge is implicitly added e.g. due to some user action.
    void add(EdgeR edge, bool isImplicit = false);

    //! Clears the selection group.
    //! \param implicitOnly If true, only implicitly added edges are removed from the group.
    void clear(bool implicitOnly = false);

    bool contains(EdgeR edgee) const;

    bool isEmpty() const;

    const std::vector<EdgeP> edges() const;

    std::optional<EdgeP> selectedEdge() const;

    size_t size() const;

    void toggle(EdgeR edge);

private:
    void clearAll();

    void clearImplicitOnly();

    // Use vector because we want to keep the order
    std::vector<EdgeP> m_edges;

    // Map from edge -> is implicitly added
    std::unordered_map<EdgeP, bool> m_implicitlyAdded;
};

#endif // EDGE_SELECTION_GROUP_HPP
