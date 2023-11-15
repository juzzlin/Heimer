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

#include "settings_tab_base.hpp"

#include "../core/settings_proxy.hpp"
#include "../single_instance_container.hpp"

namespace Dialogs {

SettingsTabBase::SettingsTabBase(QString name, QWidget * parent)
  : QWidget(parent)
  , m_name(name)
{
}

void SettingsTabBase::apply()
{
}

void SettingsTabBase::reject()
{
}

QString SettingsTabBase::name() const
{
    return m_name;
}

Core::SettingsProxy & SettingsTabBase::settingsProxy() const
{
    return SingleInstanceContainer::instance().settingsProxy();
}

} // namespace Dialogs
