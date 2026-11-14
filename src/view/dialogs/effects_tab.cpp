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

#include "effects_tab.hpp"

#include "../../application/settings_proxy.hpp"
#include "widget_factory.hpp"

#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>

namespace Dialogs {

EffectsTab::EffectsTab(QString name, QWidget * parent)
  : SettingsTabBase { name, parent }
  , m_optimizeShadowsCheckBox(new QCheckBox { tr("Optimize shadow effects"), this })
{
    m_optimizeShadowsCheckBox->setChecked(settingsProxy()->optimizeShadowEffects());

    initWidgets();
}

void EffectsTab::apply()
{
    settingsProxy()->setOptimizeShadowEffects(m_optimizeShadowsCheckBox->isChecked());
}

void EffectsTab::initWidgets()
{
    const auto mainLayout = new QVBoxLayout;
    const auto [shadowsGroup, shadowsGroupLayout] = WidgetFactory::buildGroupBoxWithVLayout(tr("Shadows"), *mainLayout);

    const auto help = new QLabel { tr("The colors and sizes of the shadows are part of the theme, see Themes."), this };
    help->setWordWrap(true);
    shadowsGroupLayout->addWidget(help);

    shadowsGroupLayout->addWidget(m_optimizeShadowsCheckBox);
    m_optimizeShadowsCheckBox->setToolTip(tr("Optimizing shadow effects makes the performance better, but might introduce some visual glitches."));

    mainLayout->addStretch();

    setLayout(mainLayout);
}

} // namespace Dialogs
