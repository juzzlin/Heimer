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

namespace SceneItems {
class Edge;
}

using EdgeCR = const SceneItems::Edge &;

using EdgeP = SceneItems::Edge *;

using EdgeR = SceneItems::Edge &;

using EdgeS = std::shared_ptr<SceneItems::Edge>;

using EdgeU = std::unique_ptr<SceneItems::Edge>;

namespace Core {
class Graph;
class MindMapData;
} // namespace Core

using GraphCR = const Core::Graph &;

using GraphR = Core::Graph &;

using MindMapDataR = Core::MindMapData &;

using MindMapDataCR = const Core::MindMapData &;

using MindMapDataS = std::shared_ptr<Core::MindMapData>;

using MindMapDataU = std::unique_ptr<Core::MindMapData>;

namespace SceneItems {
class Node;
}

using NodeCR = const SceneItems::Node &;

using NodeP = SceneItems::Node *;

using NodeR = SceneItems::Node &;

using NodeS = std::shared_ptr<SceneItems::Node>;

using NodeU = std::unique_ptr<SceneItems::Node>;

#endif // TYPES_HPP
