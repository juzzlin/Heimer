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

#include "application_service.hpp"
#include "control_strategy.hpp"
#include "core/progress_manager.hpp"
#include "core/settings_proxy.hpp"
#include "recent_files_manager.hpp"

#include "simple_logger.hpp"

#include <stdexcept>

SingleInstanceContainer * SingleInstanceContainer::m_instance = nullptr;

SingleInstanceContainer::SingleInstanceContainer()
  : m_settingsProxy(std::make_unique<Core::SettingsProxy>())
  , m_controlStrategy(std::make_unique<ControlStrategy>())
  , m_progressManager(std::make_unique<Core::ProgressManager>())
  , m_recentFilesManager(std::make_unique<RecentFilesManager>())
{
    if (!SingleInstanceContainer::m_instance) {
        SingleInstanceContainer::m_instance = this;
    } else {
        throw std::runtime_error("SingleInstanceContainer already instantiated!");
    }
}

ApplicationServiceS SingleInstanceContainer::applicationService()
{
    if (!m_applicationService) {
        if (!m_mainWindow) {
            throw std::runtime_error("MainWindow not set when instantiating ApplicationService!");
        }
        if (!SingleInstanceContainer::m_instance) {
            throw std::runtime_error("ApplicatonService requested while SIC is deleting!");
        }
        m_applicationService = std::make_unique<ApplicationService>(m_mainWindow);
        juzzlin::L().debug() << "ApplicationService instantiated";
    }
    return m_applicationService;
}

ControlStrategyS SingleInstanceContainer::controlStrategy()
{
    return m_controlStrategy;
}

ProgressManagerS SingleInstanceContainer::progressManager() const
{
    return m_progressManager;
}

RecentFilesManagerS SingleInstanceContainer::recentFilesManager() const
{
    return m_recentFilesManager;
}

SettingsProxyS SingleInstanceContainer::settingsProxy()
{
    return m_settingsProxy;
}

SingleInstanceContainer & SIC::instance()
{
    if (!SingleInstanceContainer::m_instance) {
        throw std::runtime_error("SingleInstanceContainer not instantiated");
    }
    return *SingleInstanceContainer::m_instance;
}

void SingleInstanceContainer::setMainWindow(MainWindowS mainWindow)
{
    m_mainWindow = mainWindow;
}

SingleInstanceContainer::~SingleInstanceContainer()
{
    m_applicationService.reset();

    SingleInstanceContainer::m_instance = nullptr;
}
