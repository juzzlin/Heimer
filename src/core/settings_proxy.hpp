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

#include "../constants.hpp"
#include "../scene_items/edge_model.hpp"
#include "shadow_effect_params.hpp"

#include <memory>

#include <QFont>

namespace Core {

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

private:
    SettingsProxy(const SettingsProxy & other) = delete;

    SettingsProxy & operator=(const SettingsProxy & other) = delete;

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

    bool m_selectNodeGroupByIntersection = false;

    int m_textSize;

    QFont m_font;

    ShadowEffectParams m_shadowEffectParams;

    bool m_optimizeShadowEffects = true;
};

} // namespace Core

#endif // SETTINGS_PROXY_HPP
