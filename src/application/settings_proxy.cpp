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

#include "settings_proxy.hpp"
#include "../infra/settings.hpp"

SettingsProxy::SettingsProxy()
  : m_autoload(Settings::Custom::loadAutoload())
  , m_autosave(Settings::Custom::loadAutosave())
  , m_backgroundColor(Settings::Generic::getColor(m_defaultsSettingGroup, m_backgroundColorSettingKey, Constants::MindMap::Defaults::BACKGROUND_COLOR))
  , m_edgeColor(Settings::Generic::getColor(m_defaultsSettingGroup, m_edgeColorSettingKey, Constants::MindMap::Defaults::EDGE_COLOR))
  , m_gridColor(Settings::Generic::getColor(m_defaultsSettingGroup, m_gridColorSettingKey, Constants::MindMap::Defaults::GRID_COLOR))
  , m_nodeColor(Settings::Generic::getColor(m_defaultsSettingGroup, m_nodeColorSettingKey, Constants::MindMap::Defaults::NODE_COLOR))
  , m_nodeTextColor(Settings::Generic::getColor(m_defaultsSettingGroup, m_nodeTextColorSettingKey, Constants::MindMap::Defaults::NODE_TEXT_COLOR))
  , m_edgeArrowMode(Settings::Custom::loadEdgeArrowMode(SceneItems::EdgeModel::ArrowMode::Single))
  , m_arrowSize(Settings::Generic::getNumber(m_defaultsSettingGroup, m_arrowSizeSettingKey, Constants::Settings::defaultArrowSize()))
  , m_edgeWidth(Settings::Generic::getNumber(m_defaultsSettingGroup, m_edgeWidthSettingKey, Constants::Settings::defaultEdgeWidth()))
  , m_invertedControls(Settings::Generic::getBoolean(m_editingSettingGroup, m_invertedControlsSettingKey, false))
  , m_reversedEdgeDirection(Settings::Custom::loadReversedEdgeDirection(false))
  , m_selectNodeGroupByIntersection(Settings::Custom::loadSelectNodeGroupByIntersection())
  , m_textSize(Settings::Generic::getNumber(m_defaultsSettingGroup, m_textSizeSettingKey, Constants::MindMap::Defaults::TEXT_SIZE))
  , m_font(Settings::Generic::getFont(m_defaultsSettingGroup, m_fontSettingKey, {}))
  , m_shadowEffectParams(
      static_cast<int>(Settings::Generic::getNumber(m_effectsSettingGroup, m_shadowEffectOffsetSettingKey, Constants::Settings::defaultShadowEffectOffset())),
      static_cast<int>(Settings::Generic::getNumber(m_effectsSettingGroup, m_shadowEffectNormalBlurRadiusSettingKey, Constants::Settings::defaultShadowEffectBlurRadius())),
      static_cast<int>(Settings::Generic::getNumber(m_effectsSettingGroup, m_shadowEffectSelectedItemBlurRadiusSettingKey, Constants::Settings::defaultSelectedItemShadowEffectBlurRadius())),
      Settings::Generic::getColor(m_effectsSettingGroup, m_shadowEffectShadowColorSettingKey, Constants::Settings::defaultShadowEffectShadowColor()),
      Settings::Generic::getColor(m_effectsSettingGroup, m_shadowEffectSelectedItemShadowColorSettingKey, Constants::Settings::defaultShadowEffectSelectedItemShadowColor()))
  , m_optimizeShadowEffects(Settings::Generic::getBoolean(m_effectsSettingGroup, m_optimizeShadowEffectsSettingKey, true))
{
}

bool SettingsProxy::autoload() const
{
    return m_autoload;
}

void SettingsProxy::setAutoload(bool autoload)
{
    if (m_autoload != autoload) {
        m_autoload = autoload;
        Settings::Custom::saveAutoload(autoload);
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
        Settings::Custom::saveAutosave(autosave);
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
        Settings::Generic::setColor(m_defaultsSettingGroup, m_backgroundColorSettingKey, backgroundColor);
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
        Settings::Generic::setColor(m_defaultsSettingGroup, m_edgeColorSettingKey, edgeColor);
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
        Settings::Generic::setColor(m_defaultsSettingGroup, m_gridColorSettingKey, gridColor);
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
        Settings::Generic::setColor(m_defaultsSettingGroup, m_nodeColorSettingKey, nodeColor);
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
        Settings::Generic::setColor(m_defaultsSettingGroup, m_nodeTextColorSettingKey, nodeTextColor);
    }
}

SceneItems::EdgeModel::ArrowMode SettingsProxy::edgeArrowMode() const
{
    return m_edgeArrowMode;
}

void SettingsProxy::setEdgeArrowMode(SceneItems::EdgeModel::ArrowMode mode)
{
    if (m_edgeArrowMode != mode) {
        m_edgeArrowMode = mode;
        Settings::Custom::saveEdgeArrowMode(mode);
    }
}

double SettingsProxy::arrowSize() const
{
    return m_arrowSize;
}

void SettingsProxy::setArrowSize(double arrowSize)
{
    if (!qFuzzyCompare(m_arrowSize, arrowSize)) {
        m_arrowSize = arrowSize;
        Settings::Generic::setNumber(m_defaultsSettingGroup, m_arrowSizeSettingKey, arrowSize);
    }
}

double SettingsProxy::edgeWidth() const
{
    return m_edgeWidth;
}

void SettingsProxy::setEdgeWidth(double edgeWidth)
{
    if (!qFuzzyCompare(m_edgeWidth, edgeWidth)) {
        m_edgeWidth = edgeWidth;
        Settings::Generic::setNumber(m_defaultsSettingGroup, m_edgeWidthSettingKey, edgeWidth);
    }
}

bool SettingsProxy::invertedControls() const
{
    return m_invertedControls;
}

void SettingsProxy::setInvertedControls(bool invertedControls)
{
    if (m_invertedControls != invertedControls) {
        m_invertedControls = invertedControls;
        Settings::Generic::setBoolean(m_editingSettingGroup, m_invertedControlsSettingKey, invertedControls);
    }
}

int SettingsProxy::textSize() const
{
    return m_textSize;
}

void SettingsProxy::setTextSize(int textSize)
{
    if (m_textSize != textSize) {
        m_textSize = textSize;
        Settings::Generic::setNumber(m_defaultsSettingGroup, m_textSizeSettingKey, textSize);
    }
}

QFont SettingsProxy::font() const
{
    return m_font;
}

void SettingsProxy::setFont(const QFont & font)
{
    if (m_font != font) {
        m_font = font;
        Settings::Generic::setFont(m_defaultsSettingGroup, m_fontSettingKey, font);
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
        Settings::Custom::saveReversedEdgeDirection(reversedEdgeDirection);
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
        Settings::Custom::saveSelectNodeGroupByIntersection(selectNodeGroupByIntersection);
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
        Settings::Generic::setNumber(m_effectsSettingGroup, m_shadowEffectOffsetSettingKey, params.offset());
        Settings::Generic::setNumber(m_effectsSettingGroup, m_shadowEffectNormalBlurRadiusSettingKey, params.blurRadius());
        Settings::Generic::setNumber(m_effectsSettingGroup, m_shadowEffectSelectedItemBlurRadiusSettingKey, params.selectedItemBlurRadius());
        Settings::Generic::setColor(m_effectsSettingGroup, m_shadowEffectShadowColorSettingKey, params.shadowColor());
        Settings::Generic::setColor(m_effectsSettingGroup, m_shadowEffectSelectedItemShadowColorSettingKey, params.selectedItemShadowColor());
    }
}

bool SettingsProxy::optimizeShadowEffects() const
{
    return m_optimizeShadowEffects;
}

void SettingsProxy::setOptimizeShadowEffects(bool optimizeShadowEffects)
{
    if (m_optimizeShadowEffects != optimizeShadowEffects) {
        m_optimizeShadowEffects = optimizeShadowEffects;
        Settings::Generic::setBoolean(m_effectsSettingGroup, m_optimizeShadowEffectsSettingKey, optimizeShadowEffects);
    }
}

SettingsProxy::~SettingsProxy() = default;
