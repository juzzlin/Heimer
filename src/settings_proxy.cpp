// This file is part of Heimer.
// Copyright (C) 2020 Jussi Lind <jussi.lind@iki.fi>
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

#include "settings.hpp"
#include "settings_proxy.hpp"

std::unique_ptr<SettingsProxy> SettingsProxy::m_instance;

SettingsProxy::SettingsProxy()
  : m_autosave(Settings::loadAutosave())
  , m_edgeArrowMode(Settings::loadEdgeArrowMode(Edge::ArrowMode::Single))
  , m_reversedEdgeDirection(Settings::loadReversedEdgeDirection(false))
  , m_selectNodeGroupByIntersection(Settings::loadSelectNodeGroupByIntersection())
{
}

SettingsProxy & SettingsProxy::instance()
{
    if (!SettingsProxy::m_instance) {
        SettingsProxy::m_instance = std::make_unique<SettingsProxy>();
    }
    return *SettingsProxy::m_instance;
}

bool SettingsProxy::autosave() const
{
    return m_autosave;
}

void SettingsProxy::setAutosave(bool autosave)
{
    if (m_autosave != autosave) {
        m_autosave = autosave;
        Settings::saveAutosave(autosave);
    }
}

Edge::ArrowMode SettingsProxy::edgeArrowMode() const
{
    return m_edgeArrowMode;
}

void SettingsProxy::setEdgeArrowMode(Edge::ArrowMode mode)
{
    if (m_edgeArrowMode != mode) {
        m_edgeArrowMode = mode;
        Settings::saveEdgeArrowMode(mode);
    }
}

bool SettingsProxy::reversedEdgeDirection() const
{
    return m_reversedEdgeDirection;
}

void SettingsProxy::setReversedEdgeDirection(bool reversedEdgeDirection)
{
    if (m_reversedEdgeDirection != reversedEdgeDirection) {
        m_reversedEdgeDirection = reversedEdgeDirection;
        Settings::saveReversedEdgeDirection(reversedEdgeDirection);
    }
}

bool SettingsProxy::selectNodeGroupByIntersection() const
{
    return m_selectNodeGroupByIntersection;
}

void SettingsProxy::setSelectNodeGroupByIntersection(bool selectNodeGroupByIntersection)
{
    if (m_selectNodeGroupByIntersection != selectNodeGroupByIntersection) {
        m_selectNodeGroupByIntersection = selectNodeGroupByIntersection;
        Settings::saveSelectNodeGroupByIntersection(selectNodeGroupByIntersection);
    }
}
