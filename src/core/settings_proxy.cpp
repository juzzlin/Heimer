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

namespace Core {

SettingsProxy::SettingsProxy()
  : m_autoload(Settings::V1::loadAutoload())
  , m_autosave(Settings::V1::loadAutosave())
  , m_backgroundColor(Settings::V2::getColor(Constants::MindMap::Defaults::SETTINGS_GROUP, Constants::MindMap::Defaults::BACKGROUND_COLOR_SETTINGS_KEY, Constants::MindMap::Defaults::BACKGROUND_COLOR))
  , m_edgeColor(Settings::V2::getColor(Constants::MindMap::Defaults::SETTINGS_GROUP, Constants::MindMap::Defaults::EDGE_COLOR_SETTINGS_KEY, Constants::MindMap::Defaults::EDGE_COLOR))
  , m_gridColor(Settings::V2::getColor(Constants::MindMap::Defaults::SETTINGS_GROUP, Constants::MindMap::Defaults::GRID_COLOR_SETTINGS_KEY, Constants::MindMap::Defaults::GRID_COLOR))
  , m_nodeColor(Settings::V2::getColor(Constants::MindMap::Defaults::SETTINGS_GROUP, Constants::MindMap::Defaults::NODE_COLOR_SETTINGS_KEY, Constants::MindMap::Defaults::NODE_COLOR))
  , m_nodeTextColor(Settings::V2::getColor(Constants::MindMap::Defaults::SETTINGS_GROUP, Constants::MindMap::Defaults::NODE_TEXT_COLOR_SETTINGS_KEY, Constants::MindMap::Defaults::NODE_TEXT_COLOR))
  , m_edgeArrowMode(Settings::V1::loadEdgeArrowMode(SceneItems::EdgeModel::ArrowMode::Single))
  , m_arrowSize(Settings::V2::getNumber(Constants::MindMap::Defaults::SETTINGS_GROUP, Constants::MindMap::Defaults::ARROW_SIZE_SETTINGS_KEY, Constants::Edge::Defaults::ARROW_SIZE))
  , m_edgeWidth(Settings::V2::getNumber(Constants::MindMap::Defaults::SETTINGS_GROUP, Constants::MindMap::Defaults::EDGE_WIDTH_SETTINGS_KEY, Constants::MindMap::Defaults::EDGE_WIDTH))
  , m_invertedControls(Settings::V2::getBoolean(Constants::View::EDITING_SETTINGS_GROUP, Constants::View::INVERTED_CONTROLS_SETTINGS_KEY, false))
  , m_reversedEdgeDirection(Settings::V1::loadReversedEdgeDirection(false))
  , m_selectNodeGroupByIntersection(Settings::V1::loadSelectNodeGroupByIntersection())
  , m_textSize(Settings::V2::getNumber(Constants::MindMap::Defaults::SETTINGS_GROUP, Constants::MindMap::Defaults::TEXT_SIZE_SETTINGS_KEY, Constants::MindMap::Defaults::TEXT_SIZE))
  , m_font(Settings::V2::getFont(Constants::MindMap::Defaults::SETTINGS_GROUP, Constants::MindMap::Defaults::FONT_SETTINGS_KEY, {}))
  , m_shadowEffectParams(
      static_cast<int>(Settings::V2::getNumber(Constants::Effects::EFFECTS_SETTINGS_GROUP, Constants::Effects::SHADOW_EFFECT_OFFSET_SETTINGS_KEY, Constants::Effects::Defaults::SHADOW_EFFECT_OFFSET)),
      static_cast<int>(Settings::V2::getNumber(Constants::Effects::EFFECTS_SETTINGS_GROUP, Constants::Effects::SHADOW_EFFECT_NORMAL_BLUR_RADIUS_SETTINGS_KEY, Constants::Effects::Defaults::SHADOW_EFFECT_BLUR_RADIUS)),
      static_cast<int>(Settings::V2::getNumber(Constants::Effects::EFFECTS_SETTINGS_GROUP, Constants::Effects::SHADOW_EFFECT_SELECTED_ITEM_BLUR_RADIUS_SETTINGS_KEY, Constants::Effects::Defaults::SELECTED_ITEM_SHADOW_EFFECT_BLUR_RADIUS)),
      Settings::V2::getColor(Constants::Effects::EFFECTS_SETTINGS_GROUP, Constants::Effects::SHADOW_EFFECT_SHADOW_COLOR_SETTINGS_KEY, Constants::Effects::Defaults::SHADOW_EFFECT_SHADOW_COLOR),
      Settings::V2::getColor(Constants::Effects::EFFECTS_SETTINGS_GROUP, Constants::Effects::SHADOW_EFFECT_SELECTED_ITEM_SHADOW_COLOR_SETTINGS_KEY, Constants::Effects::Defaults::SHADOW_EFFECT_SELECTED_ITEM_SHADOW_COLOR))
  , m_optimizeShadowEffects(Settings::V2::getBoolean(Constants::Effects::EFFECTS_SETTINGS_GROUP, Constants::Effects::OPTIMIZE_SHADOW_EFFECTS_SETTINGS_KEY, true))
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

SceneItems::EdgeModel::ArrowMode SettingsProxy::edgeArrowMode() const
{
    return m_edgeArrowMode;
}

void SettingsProxy::setEdgeArrowMode(SceneItems::EdgeModel::ArrowMode mode)
{
    if (m_edgeArrowMode != mode) {
        m_edgeArrowMode = mode;
        Settings::V1::saveEdgeArrowMode(mode);
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
        Settings::V2::setNumber(Constants::MindMap::Defaults::SETTINGS_GROUP, Constants::MindMap::Defaults::ARROW_SIZE_SETTINGS_KEY, arrowSize);
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
        Settings::V2::setNumber(Constants::MindMap::Defaults::SETTINGS_GROUP, Constants::MindMap::Defaults::EDGE_WIDTH_SETTINGS_KEY, edgeWidth);
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
        Settings::V2::setBoolean(Constants::View::EDITING_SETTINGS_GROUP, Constants::View::INVERTED_CONTROLS_SETTINGS_KEY, invertedControls);
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
        Settings::V2::setNumber(Constants::MindMap::Defaults::SETTINGS_GROUP, Constants::MindMap::Defaults::TEXT_SIZE_SETTINGS_KEY, textSize);
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
        Settings::V2::setFont(Constants::MindMap::Defaults::SETTINGS_GROUP, Constants::MindMap::Defaults::FONT_SETTINGS_KEY, font);
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

bool SettingsProxy::optimizeShadowEffects() const
{
    return m_optimizeShadowEffects;
}

void SettingsProxy::setOptimizeShadowEffects(bool optimizeShadowEffects)
{
    if (m_optimizeShadowEffects != optimizeShadowEffects) {
        m_optimizeShadowEffects = optimizeShadowEffects;
        Settings::V2::setBoolean(Constants::Effects::EFFECTS_SETTINGS_GROUP, Constants::Effects::OPTIMIZE_SHADOW_EFFECTS_SETTINGS_KEY, optimizeShadowEffects);
    }
}

SettingsProxy::~SettingsProxy() = default;

} // namespace Core
