// This file is part of Heimer.
// Copyright (C) 2022 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef TYPES_HPP
#define TYPES_HPP

#include <memory>

class Edge;

using EdgeCR = const Edge &;

using EdgeP = Edge *;

using EdgeR = Edge &;

using EdgeS = std::shared_ptr<Edge>;

using EdgeU = std::unique_ptr<Edge>;

class Graph;

using GraphCR = const Graph &;

using GraphR = Graph &;

class Node;

using NodeCR = const Node &;

using NodeP = Node *;

using NodeR = Node &;

using NodeS = std::shared_ptr<Node>;

using NodeU = std::unique_ptr<Node>;

#endif // TYPES_HPP
