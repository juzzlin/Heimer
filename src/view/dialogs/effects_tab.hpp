// This file is part of Heimer.
// Copyright (C) 2022 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef EFFECTS_TAB_HPP
#define EFFECTS_TAB_HPP

#include "settings_tab_base.hpp"

class QCheckBox;
class QSpinBox;
struct ShadowEffectParams;

namespace Dialogs {

class ColorSettingButton;

class EffectsTab : public SettingsTabBase
{
    Q_OBJECT

public:
    explicit EffectsTab(QString name, QWidget * parent = nullptr);

    void apply() override;

signals:
    void shadowEffectChanged(const ShadowEffectParams & params);

private:
    void apply(const ShadowEffectParams & params);

    void initWidgets();

    QSpinBox * m_shadowOffsetSpinBox;

    QSpinBox * m_shadowBlurRadiusSpinBox;

    QSpinBox * m_selectedItemShadowBlurRadiusSpinBox;

    ColorSettingButton * m_shadowColorButton;

    ColorSettingButton * m_selectedItemShadowColorButton;

    QCheckBox * m_optimizeShadowsCheckBox;

    const int m_shadowEffectMaxOffset = 10;

    const int m_shadowEffectMinOffset = 0;

    const int m_shadowEffectMaxBlurRadius = 100;

    const int m_shadowEffectMinBlurRadius = 0;
};

} // namespace Dialogs

#endif // EFFECTS_TAB_HPP
