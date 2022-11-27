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
  : m_autoload(Settings::V1::loadAutoload())
  , m_autosave(Settings::V1::loadAutosave())
  , m_backgroundColor(Settings::V2::getColor(Constants::MindMap::Defaults::SETTINGS_GROUP, Constants::MindMap::Defaults::BACKGROUND_COLOR_SETTINGS_KEY, Constants::MindMap::Defaults::BACKGROUND_COLOR))
  , m_edgeColor(Settings::V2::getColor(Constants::MindMap::Defaults::SETTINGS_GROUP, Constants::MindMap::Defaults::EDGE_COLOR_SETTINGS_KEY, Constants::MindMap::Defaults::EDGE_COLOR))
  , m_gridColor(Settings::V2::getColor(Constants::MindMap::Defaults::SETTINGS_GROUP, Constants::MindMap::Defaults::GRID_COLOR_SETTINGS_KEY, Constants::MindMap::Defaults::GRID_COLOR))
  , m_nodeColor(Settings::V2::getColor(Constants::MindMap::Defaults::SETTINGS_GROUP, Constants::MindMap::Defaults::NODE_COLOR_SETTINGS_KEY, Constants::MindMap::Defaults::NODE_COLOR))
  , m_nodeTextColor(Settings::V2::getColor(Constants::MindMap::Defaults::SETTINGS_GROUP, Constants::MindMap::Defaults::NODE_TEXT_COLOR_SETTINGS_KEY, Constants::MindMap::Defaults::NODE_TEXT_COLOR))
  , m_edgeArrowMode(Settings::V1::loadEdgeArrowMode(EdgeModel::ArrowMode::Single))
  , m_reversedEdgeDirection(Settings::V1::loadReversedEdgeDirection(false))
  , m_selectNodeGroupByIntersection(Settings::V1::loadSelectNodeGroupByIntersection())
  , m_shadowEffectParams(
      static_cast<int>(Settings::V2::getNumber(Constants::Effects::EFFECTS_SETTINGS_GROUP, Constants::Effects::SHADOW_EFFECT_OFFSET_SETTINGS_KEY, Constants::Effects::Defaults::SHADOW_EFFECT_OFFSET)),
      static_cast<int>(Settings::V2::getNumber(Constants::Effects::EFFECTS_SETTINGS_GROUP, Constants::Effects::SHADOW_EFFECT_NORMAL_BLUR_RADIUS_SETTINGS_KEY, Constants::Effects::Defaults::SHADOW_EFFECT_BLUR_RADIUS)),
      static_cast<int>(Settings::V2::getNumber(Constants::Effects::EFFECTS_SETTINGS_GROUP, Constants::Effects::SHADOW_EFFECT_SELECTED_ITEM_BLUR_RADIUS_SETTINGS_KEY, Constants::Effects::Defaults::SELECTED_ITEM_SHADOW_EFFECT_BLUR_RADIUS)),
      Settings::V2::getColor(Constants::Effects::EFFECTS_SETTINGS_GROUP, Constants::Effects::SHADOW_EFFECT_SHADOW_COLOR_SETTINGS_KEY, Constants::Effects::Defaults::SHADOW_EFFECT_SHADOW_COLOR),
      Settings::V2::getColor(Constants::Effects::EFFECTS_SETTINGS_GROUP, Constants::Effects::SHADOW_EFFECT_SELECTED_ITEM_SHADOW_COLOR_SETTINGS_KEY, Constants::Effects::Defaults::SHADOW_EFFECT_SELECTED_ITEM_SHADOW_COLOR))
{
}

SettingsProxy & SettingsProxy::instance()
{
    if (!SettingsProxy::m_instance) {
        SettingsProxy::m_instance = std::make_unique<SettingsProxy>();
    }
    return *SettingsProxy::m_instance;
}

bool SettingsProxy::autoload() const
{
    return m_autoload;
}

void SettingsProxy::setAutoload(bool autoload)
{
    if (m_autoload != autoload) {
        m_autoload = autoload;
        Settings::V1::saveAutoload(autoload);
    }
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

const QColor & SettingsProxy::backgroundColor() const
{
    return m_backgroundColor;
}

void SettingsProxy::setBackgroundColor(const QColor & backgroundColor)
{
    if (m_backgroundColor != backgroundColor) {
        m_backgroundColor = backgroundColor;
        Settings::V2::setColor(Constants::MindMap::Defaults::SETTINGS_GROUP, Constants::MindMap::Defaults::BACKGROUND_COLOR_SETTINGS_KEY, backgroundColor);
    }
}

const QColor & SettingsProxy::edgeColor() const
{
    return m_edgeColor;
}

void SettingsProxy::setEdgeColor(const QColor & edgeColor)
{
    if (m_edgeColor != edgeColor) {
        m_edgeColor = edgeColor;
        Settings::V2::setColor(Constants::MindMap::Defaults::SETTINGS_GROUP, Constants::MindMap::Defaults::EDGE_COLOR_SETTINGS_KEY, edgeColor);
    }
}

const QColor & SettingsProxy::gridColor() const
{
    return m_gridColor;
}

void SettingsProxy::setGridColor(const QColor & gridColor)
{
    if (m_gridColor != gridColor) {
        m_gridColor = gridColor;
        Settings::V2::setColor(Constants::MindMap::Defaults::SETTINGS_GROUP, Constants::MindMap::Defaults::GRID_COLOR_SETTINGS_KEY, gridColor);
    }
}

const QColor & SettingsProxy::nodeColor() const
{
    return m_nodeColor;
}

void SettingsProxy::setNodeColor(const QColor & nodeColor)
{
    if (m_nodeColor != nodeColor) {
        m_nodeColor = nodeColor;
        Settings::V2::setColor(Constants::MindMap::Defaults::SETTINGS_GROUP, Constants::MindMap::Defaults::NODE_COLOR_SETTINGS_KEY, nodeColor);
    }
}

const QColor & SettingsProxy::nodeTextColor() const
{
    return m_nodeTextColor;
}

void SettingsProxy::setNodeTextColor(const QColor & nodeTextColor)
{
    if (m_nodeTextColor != nodeTextColor) {
        m_nodeTextColor = nodeTextColor;
        Settings::V2::setColor(Constants::MindMap::Defaults::SETTINGS_GROUP, Constants::MindMap::Defaults::NODE_TEXT_COLOR_SETTINGS_KEY, nodeTextColor);
    }
}

EdgeModel::ArrowMode SettingsProxy::edgeArrowMode() const
{
    return m_edgeArrowMode;
}

void SettingsProxy::setEdgeArrowMode(EdgeModel::ArrowMode mode)
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

const ShadowEffectParams SettingsProxy::shadowEffect() const
{
    return m_shadowEffectParams;
}

void SettingsProxy::setShadowEffect(const ShadowEffectParams & params)
{
    if (m_shadowEffectParams != params) {
        m_shadowEffectParams = params;
        Settings::V2::setNumber(Constants::Effects::EFFECTS_SETTINGS_GROUP, Constants::Effects::SHADOW_EFFECT_OFFSET_SETTINGS_KEY, params.offset);
        Settings::V2::setNumber(Constants::Effects::EFFECTS_SETTINGS_GROUP, Constants::Effects::SHADOW_EFFECT_NORMAL_BLUR_RADIUS_SETTINGS_KEY, params.blurRadius);
        Settings::V2::setNumber(Constants::Effects::EFFECTS_SETTINGS_GROUP, Constants::Effects::SHADOW_EFFECT_SELECTED_ITEM_BLUR_RADIUS_SETTINGS_KEY, params.selectedItemBlurRadius);
        Settings::V2::setColor(Constants::Effects::EFFECTS_SETTINGS_GROUP, Constants::Effects::SHADOW_EFFECT_SHADOW_COLOR_SETTINGS_KEY, params.shadowColor);
        Settings::V2::setColor(Constants::Effects::EFFECTS_SETTINGS_GROUP, Constants::Effects::SHADOW_EFFECT_SELECTED_ITEM_SHADOW_COLOR_SETTINGS_KEY, params.selectedItemShadowColor);
    }
}
