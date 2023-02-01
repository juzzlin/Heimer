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
#include "settings_proxy.hpp"

#include <stdexcept>

std::unique_ptr<SingleInstanceContainer> SingleInstanceContainer::m_instance;

SingleInstanceContainer::SingleInstanceContainer()
  : m_controlStrategy(std::make_unique<ControlStrategy>())
  , m_settingsProxy(std::make_unique<SettingsProxy>())
{
    if (SingleInstanceContainer::m_instance) {
        throw std::runtime_error("SingleInstanceContainer already instantiated!");
    }
}

ControlStrategy & SingleInstanceContainer::controlStrategy() const
{
    return *m_controlStrategy;
}

SettingsProxy & SingleInstanceContainer::settingsProxy() const
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
