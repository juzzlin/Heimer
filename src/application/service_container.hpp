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

#ifndef SERVICE_CONTAINER_HPP
#define SERVICE_CONTAINER_HPP

#include "../common/types.hpp"

class ApplicationService;
class EditorService;
class ControlStrategy;
class RecentFilesManager;

class ProgressManager;
class SettingsProxy;

//! A poor man's single instance DI.
class ServiceContainer
{
public:
    ServiceContainer();

    ~ServiceContainer();

    static ServiceContainer & instance();

    ApplicationServiceS applicationService();

    ControlStrategyS controlStrategy();

    ProgressManagerS progressManager() const;

    RecentFilesManagerS recentFilesManager() const;

    SettingsProxyS settingsProxy();

    void setMainWindow(MainWindowS mainWindow);

private:
    ServiceContainer(const ServiceContainer & other) = delete;

    ServiceContainer & operator=(const ServiceContainer & other) = delete;

    SettingsProxyS m_settingsProxy;

    ApplicationServiceS m_applicationService;

    ControlStrategyS m_controlStrategy;

    ProgressManagerS m_progressManager;

    RecentFilesManagerS m_recentFilesManager;

    MainWindowS m_mainWindow;

    static ServiceContainer * m_instance;
};

#endif // SERVICE_CONTAINER_HPP
