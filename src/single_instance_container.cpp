// This file is part of Heimer.
// Copyright (C) 2023 Jussi Lind <jussi.lind@iki.fi>
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

#include "single_instance_container.hpp"

#include "control_strategy.hpp"
#include "core/progress_manager.hpp"
#include "core/settings_proxy.hpp"
#include "editor_service.hpp"

#include "simple_logger.hpp"

#include <stdexcept>

std::unique_ptr<SingleInstanceContainer> SingleInstanceContainer::m_instance;

SingleInstanceContainer::SingleInstanceContainer()
  : m_controlStrategy(std::make_unique<ControlStrategy>())
  , m_progressManager(std::make_unique<Core::ProgressManager>())
  , m_settingsProxy(std::make_unique<Core::SettingsProxy>())
{
    if (SingleInstanceContainer::m_instance) {
        throw std::runtime_error("SingleInstanceContainer already instantiated!");
    }
}

ControlStrategy & SingleInstanceContainer::controlStrategy() const
{
    return *m_controlStrategy;
}

EditorService & SingleInstanceContainer::editorService() const
{
    if (!m_editorService) {
        m_editorService = std::make_unique<EditorService>();
        juzzlin::L().debug() << "EditorService instantiated";
    }

    return *m_editorService;
}

Core::ProgressManager & SingleInstanceContainer::progressManager() const
{
    return *m_progressManager;
}

Core::SettingsProxy & SingleInstanceContainer::settingsProxy() const
{
    return *m_settingsProxy;
}

SingleInstanceContainer & SingleInstanceContainer::instance()
{
    if (!SingleInstanceContainer::m_instance) {
        SingleInstanceContainer::m_instance = std::make_unique<SingleInstanceContainer>();
    }
    return *SingleInstanceContainer::m_instance;
}

SingleInstanceContainer::~SingleInstanceContainer() = default;
