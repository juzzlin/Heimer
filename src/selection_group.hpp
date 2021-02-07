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

#ifndef SELECTION_GROUP_HPP
#define SELECTION_GROUP_HPP

#include <QPointF>
#include <vector>

class Node;

class SelectionGroup
{
public:
    void addSelectedNode(Node & node);

    void clear();

    bool hasNode(Node & node) const;

    void move(Node & reference, QPointF location);

    const std::vector<Node *> & nodes() const;

    Node * selectedNode() const;

    size_t size() const;

    void toggleNode(Node & node);

private:
    // Use vector because we want to keep the order
    std::vector<Node *> m_nodes;
};

#endif // SELECTION_GROUP_HPP
