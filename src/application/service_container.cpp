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

#include "service_container.hpp"

#include "application_service.hpp"
#include "control_strategy.hpp"
#include "progress_manager.hpp"
#include "recent_files_manager.hpp"
#include "settings_proxy.hpp"

#include "simple_logger.hpp"

#include <stdexcept>

static const auto TAG = "ServiceContainer";

ServiceContainer * ServiceContainer::m_instance = nullptr;

ServiceContainer::ServiceContainer()
  : m_settingsProxy(std::make_unique<SettingsProxy>())
  , m_controlStrategy(std::make_unique<ControlStrategy>())
  , m_progressManager(std::make_unique<ProgressManager>())
  , m_recentFilesManager(std::make_unique<RecentFilesManager>())
{
    if (!ServiceContainer::m_instance) {
        ServiceContainer::m_instance = this;
    } else {
        throw std::runtime_error("ServiceContainer already instantiated!");
    }
}

ApplicationServiceS ServiceContainer::applicationService()
{
    if (!m_applicationService) {
        if (!m_mainWindow) {
            throw std::runtime_error("MainWindow not set when instantiating ApplicationService!");
        }
        if (!ServiceContainer::m_instance) {
            throw std::runtime_error("ApplicatonService requested while SIC is deleting!");
        }
        m_applicationService = std::make_unique<ApplicationService>(m_mainWindow);
        juzzlin::L(TAG).debug() << "ApplicationService instantiated";
    }
    return m_applicationService;
}

ControlStrategyS ServiceContainer::controlStrategy()
{
    return m_controlStrategy;
}

ProgressManagerS ServiceContainer::progressManager() const
{
    return m_progressManager;
}

RecentFilesManagerS ServiceContainer::recentFilesManager() const
{
    return m_recentFilesManager;
}

SettingsProxyS ServiceContainer::settingsProxy()
{
    return m_settingsProxy;
}

ServiceContainer & SC::instance()
{
    if (!ServiceContainer::m_instance) {
        throw std::runtime_error("ServiceContainer not instantiated");
    }
    return *ServiceContainer::m_instance;
}

void ServiceContainer::setMainWindow(MainWindowS mainWindow)
{
    m_mainWindow = mainWindow;
}

ServiceContainer::~ServiceContainer()
{
    m_applicationService.reset();

    ServiceContainer::m_instance = nullptr;
}
