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

#ifndef NODE_SELECTION_GROUP_HPP
#define NODE_SELECTION_GROUP_HPP

#include <QPointF>

#include <optional>
#include <unordered_map>
#include <vector>

#include "../common/types.hpp"

class NodeSelectionGroup
{
public:
    //! Adds node to the selection group.
    //! \param node Node to be added.
    //! \param isImplicit Tells if the node is implicitly added e.g. due to some user action.
    void add(NodeR node, bool isImplicit = false);

    //! Clears the selection group.
    //! \param implicitOnly If true, only implicitly added nodes are removed from the group.
    void clear(bool implicitOnly = false);

    bool contains(NodeR node) const;

    bool isEmpty() const;

    void move(NodeR reference, QPointF location);

    const std::vector<NodeP> nodes() const;

    std::optional<NodeP> selectedNode() const;

    size_t size() const;

    void toggle(NodeR node);

private:
    void clearAll();

    void clearImplicitOnly();

    // Use vector because we want to keep the order
    std::vector<NodeP> m_nodes;

    // Map from node -> is implicitly added
    std::unordered_map<NodeP, bool> m_implicitlyAdded;
};

#endif // NODE_SELECTION_GROUP_HPP
