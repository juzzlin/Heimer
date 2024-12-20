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

#ifndef SETTINGS_PROXY_HPP
#define SETTINGS_PROXY_HPP

#include "../view/scene_items/edge_model.hpp"
#include "../view/shadow_effect_params.hpp"

#include <QFont>

//! Some kind of a frond-end or facade for stateless Settings to enable faster value reads (QSettings can be quite slow)
//! and to create more complex settings (e.g. setShadowEffect()) that use the Settings API. However, the situation is a
//! bit of a mess right now as some features still use Settings directly and some features use SettingsProxy.
//!
//! The current policy for new settings is to use the Settings::Generic API and hide it behind SettingsProxy to
//! expose a clean API without visible setting keys or low-level functions.
//!
//! SettingsProxy can be accessed via ServiceContainer throughout the application.
class SettingsProxy
{
public:
    SettingsProxy();

    ~SettingsProxy();

    bool autoload() const;

    void setAutoload(bool autoload);

    bool autosave() const;

    void setAutosave(bool autosave);

    const QColor & backgroundColor() const;

    void setBackgroundColor(const QColor & backgroundColor);

    const QColor & edgeColor() const;

    void setEdgeColor(const QColor & edgeColor);

    const QColor & gridColor() const;

    void setGridColor(const QColor & gridColor);

    const QColor & nodeColor() const;

    void setNodeColor(const QColor & nodeColor);

    const QColor & nodeTextColor() const;

    void setNodeTextColor(const QColor & nodeTextColor);

    SceneItems::EdgeModel::ArrowMode edgeArrowMode() const;

    void setEdgeArrowMode(SceneItems::EdgeModel::ArrowMode mode);

    double arrowSize() const;

    void setArrowSize(double arrowSize);

    double edgeWidth() const;

    void setEdgeWidth(double edgeWidth);

    bool invertedControls() const;

    void setInvertedControls(bool invertedControls);

    bool reversedEdgeDirection() const;

    void setReversedEdgeDirection(bool reversedEdgeDirection);

    bool raiseNodeOnMouseHover() const;

    void setRaiseNodeOnMouseHover(bool raiseNodeOnMouseHover);

    bool selectNodeGroupByIntersection() const;

    void setSelectNodeGroupByIntersection(bool selectNodeGroupByIntersection);

    const ShadowEffectParams shadowEffect() const;

    void setShadowEffect(const ShadowEffectParams & params);

    bool optimizeShadowEffects() const;

    void setOptimizeShadowEffects(bool optimizeShadowEffects);

    int textSize() const;

    void setTextSize(int textSize);

    QFont font() const;

    void setFont(const QFont & font);

    QString userLanguage() const;

    void setUserLanguage(const QString & language);

private:
    SettingsProxy(const SettingsProxy & other) = delete;

    SettingsProxy & operator=(const SettingsProxy & other) = delete;

    const QString m_defaultsSettingGroup = Constants::Settings::defaultsSettingGroup();

    const QString m_arrowSizeSettingKey = "arrowSize";

    const QString m_backgroundColorSettingKey = "backgroundColor";

    const QString m_edgeColorSettingKey = "edgeColor";

    const QString m_edgeWidthSettingKey = "edgeWidth";

    const QString m_fontSettingKey = "font";

    const QString m_gridColorSettingKey = "gridColor";

    const QString m_nodeColorSettingKey = "nodeColor";

    const QString m_nodeTextColorSettingKey = "nodeTextColor";

    const QString m_textSizeSettingKey = "textSize";

    const QString m_effectsSettingGroup = "Effects";

    const QString m_shadowEffectOffsetSettingKey = "shadowEffectOffset";

    const QString m_shadowEffectNormalBlurRadiusSettingKey = "shadowEffectBlurRadius";

    const QString m_shadowEffectSelectedItemBlurRadiusSettingKey = "shadowEffectSelectedItemBlurRadius";

    const QString m_shadowEffectShadowColorSettingKey = "shadowEffectShadowColor";

    const QString m_shadowEffectSelectedItemShadowColorSettingKey = "shadowEffectSelectedItemShadowColor";

    const QString m_userLanguageSettingKey = "userLanguage";

    const QString m_optimizeShadowEffectsSettingKey = "optimizeShadowEffects";

    const QString m_editingSettingGroup = "Editing";

    const QString m_invertedControlsSettingKey = "invertedControls";

    const QString m_raiseNodeOnMouseHoverKey = "raiseNodeOnMouseHoverKey";

    bool m_autoload = false;

    bool m_autosave = false;

    QColor m_backgroundColor;

    QColor m_edgeColor;

    QColor m_gridColor;

    QColor m_nodeColor;

    QColor m_nodeTextColor;

    SceneItems::EdgeModel::ArrowMode m_edgeArrowMode;

    double m_arrowSize;

    double m_edgeWidth;

    bool m_invertedControls = false;

    bool m_reversedEdgeDirection = false;

    bool m_raiseNodeOnMouseHover = true;

    bool m_selectNodeGroupByIntersection = false;

    int m_textSize;

    QFont m_font;

    ShadowEffectParams m_shadowEffectParams;

    bool m_optimizeShadowEffects = true;

    QString m_userLanguage;
};

#endif // SETTINGS_PROXY_HPP
