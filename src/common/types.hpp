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

class ApplicationService;
using ApplicationServiceS = std::shared_ptr<ApplicationService>;

class ControlStrategy;

using ControlStrategyS = std::shared_ptr<ControlStrategy>;

class EditorService;

using EditorServiceS = std::shared_ptr<EditorService>;

class LanguageService;

using LanguageServiceS = std::shared_ptr<LanguageService>;

namespace SceneItems {
class Edge;
}

using EdgeCR = const SceneItems::Edge &;

using EdgeP = SceneItems::Edge *;

using EdgeR = SceneItems::Edge &;

using EdgeS = std::shared_ptr<SceneItems::Edge>;

using EdgeU = std::unique_ptr<SceneItems::Edge>;

class Graph;
class MindMapData;
class ProgressManager;
class SettingsProxy;

using GraphCR = const Graph &;

using GraphR = Graph &;

class MainWindow;
using MainWindowS = std::shared_ptr<MainWindow>;

using MindMapDataR = MindMapData &;

using MindMapDataCR = const MindMapData &;

using MindMapDataS = std::shared_ptr<MindMapData>;

using MindMapDataU = std::unique_ptr<MindMapData>;

namespace SceneItems {
class Node;
}

using NodeCR = const SceneItems::Node &;

using NodeP = SceneItems::Node *;

using NodeR = SceneItems::Node &;

using NodeS = std::shared_ptr<SceneItems::Node>;

using NodeU = std::unique_ptr<SceneItems::Node>;

using ProgressManagerS = std::shared_ptr<ProgressManager>;

class RecentFilesManager;
using RecentFilesManagerS = std::shared_ptr<RecentFilesManager>;

using SettingsProxyS = std::shared_ptr<SettingsProxy>;

class ServiceContainer;
using SC = ServiceContainer;

#endif // TYPES_HPP
