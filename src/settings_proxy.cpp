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

#include "graphics_factory.hpp"

std::unique_ptr<SettingsProxy> SettingsProxy::m_instance;

SettingsProxy::SettingsProxy()
  : m_autosave(Settings::V1::loadAutosave())
  , m_edgeArrowMode(Settings::V1::loadEdgeArrowMode(Edge::ArrowMode::Single))
  , m_reversedEdgeDirection(Settings::V1::loadReversedEdgeDirection(false))
  , m_selectNodeGroupByIntersection(Settings::V1::loadSelectNodeGroupByIntersection())
  , m_shadowEffectParams(
      static_cast<int>(Settings::V2::getNumber(Constants::Effects::EFFECTS_SETTINGS_GROUP, Constants::Effects::SHADOW_EFFECT_OFFSET_SETTINGS_KEY, Constants::Effects::DEFAULT_SHADOW_EFFECT_OFFSET)),
      static_cast<int>(Settings::V2::getNumber(Constants::Effects::EFFECTS_SETTINGS_GROUP, Constants::Effects::SHADOW_EFFECT_NORMAL_BLUR_RADIUS_SETTINGS_KEY, Constants::Effects::DEFAULT_SHADOW_EFFECT_NORMAL_BLUR_RADIUS)),
      static_cast<int>(Settings::V2::getNumber(Constants::Effects::EFFECTS_SETTINGS_GROUP, Constants::Effects::SHADOW_EFFECT_SELECTED_BLUR_RADIUS_SETTINGS_KEY, Constants::Effects::DEFAULT_SHADOW_EFFECT_SELECTED_BLUR_RADIUS)))
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
        Settings::V1::saveAutosave(autosave);
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
        Settings::V1::saveEdgeArrowMode(mode);
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
        Settings::V1::saveReversedEdgeDirection(reversedEdgeDirection);
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
        Settings::V1::saveSelectNodeGroupByIntersection(selectNodeGroupByIntersection);
    }
}

const GraphicsFactory::ShadowEffectParams SettingsProxy::shadowEffect() const
{
    return m_shadowEffectParams;
}

void SettingsProxy::setShadowEffect(const GraphicsFactory::ShadowEffectParams & params)
{
    if (m_shadowEffectParams != params) {
        m_shadowEffectParams = params;
        Settings::V2::setNumber(Constants::Effects::EFFECTS_SETTINGS_GROUP, Constants::Effects::SHADOW_EFFECT_OFFSET_SETTINGS_KEY, params.offset);
        Settings::V2::setNumber(Constants::Effects::EFFECTS_SETTINGS_GROUP, Constants::Effects::SHADOW_EFFECT_NORMAL_BLUR_RADIUS_SETTINGS_KEY, params.blurRadiusNormal);
        Settings::V2::setNumber(Constants::Effects::EFFECTS_SETTINGS_GROUP, Constants::Effects::SHADOW_EFFECT_SELECTED_BLUR_RADIUS_SETTINGS_KEY, params.blurRadiusSelected);
    }
}
