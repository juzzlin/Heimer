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
#include "../../common/constants.hpp"
#include "../shadow_effect_params.hpp"
#include "color_setting_button.hpp"
#include "widget_factory.hpp"

#include <QCheckBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

namespace Dialogs {

EffectsTab::EffectsTab(QString name, QWidget * parent)
  : SettingsTabBase { name, parent }
  , m_shadowOffsetSpinBox(new QSpinBox(this))
  , m_shadowBlurRadiusSpinBox(new QSpinBox(this))
  , m_selectedItemShadowBlurRadiusSpinBox(new QSpinBox(this))
  , m_shadowColorButton(new ColorSettingButton(tr("Shadow color"), ColorDialog::Role::ShadowColor, this))
  , m_selectedItemShadowColorButton(new ColorSettingButton(tr("Selected item shadow color"), ColorDialog::Role::SelectedItemShadowColor, this))
  , m_optimizeShadowsCheckBox(new QCheckBox(tr("Optimize shadow effects"), this))
{
    m_shadowOffsetSpinBox->setMinimum(m_shadowEffectMinOffset);
    m_shadowOffsetSpinBox->setMaximum(m_shadowEffectMaxOffset);
    m_shadowOffsetSpinBox->setValue(settingsProxy()->shadowEffect().offset());

    m_shadowBlurRadiusSpinBox->setMinimum(m_shadowEffectMinBlurRadius);
    m_shadowBlurRadiusSpinBox->setMaximum(m_shadowEffectMaxBlurRadius);
    m_shadowBlurRadiusSpinBox->setValue(settingsProxy()->shadowEffect().blurRadius());

    m_selectedItemShadowBlurRadiusSpinBox->setMinimum(m_shadowEffectMinBlurRadius);
    m_selectedItemShadowBlurRadiusSpinBox->setMaximum(m_shadowEffectMaxBlurRadius);
    m_selectedItemShadowBlurRadiusSpinBox->setValue(settingsProxy()->shadowEffect().selectedItemBlurRadius());

    m_optimizeShadowsCheckBox->setChecked(settingsProxy()->optimizeShadowEffects());

    initWidgets();

#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    const auto signal = QOverload<int>::of(&QSpinBox::valueChanged);
    const auto target = QOverload<>::of(&EffectsTab::apply);
#else
    const auto signal = static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged);
    const auto target = static_cast<void (EffectsTab::*)()>(&EffectsTab::apply);
#endif
    connect(m_shadowOffsetSpinBox, signal, this, target);
    connect(m_shadowBlurRadiusSpinBox, signal, this, target);
    connect(m_selectedItemShadowBlurRadiusSpinBox, signal, this, target);
    connect(m_shadowColorButton, &ColorSettingButton::colorSelected, this, [this] {
        apply();
    });
    connect(m_selectedItemShadowColorButton, &ColorSettingButton::colorSelected, this, [this] {
        apply();
    });
}

void EffectsTab::apply()
{
    apply({ m_shadowOffsetSpinBox->value(), m_shadowBlurRadiusSpinBox->value(), m_selectedItemShadowBlurRadiusSpinBox->value(), m_shadowColorButton->selectedColor(), m_selectedItemShadowColorButton->selectedColor() });
}

void EffectsTab::apply(const ShadowEffectParams & params)
{
    if (settingsProxy()->shadowEffect() != params) {
        settingsProxy()->setShadowEffect(params);
        emit shadowEffectChanged(params);
    }

    settingsProxy()->setOptimizeShadowEffects(m_optimizeShadowsCheckBox->isChecked());
}

void EffectsTab::initWidgets()
{
    const auto mainLayout = new QVBoxLayout;
    const auto && [shadowsGroup, shadowsGroupLayout] = WidgetFactory::buildGroupBoxWithVLayout(tr("Shadows"), *mainLayout);

    const auto offsetLayout = new QHBoxLayout;
    offsetLayout->addWidget(new QLabel(tr("Shadow effect offset:")));
    const auto offsetLayoutSpacer = new QWidget;
    offsetLayoutSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    offsetLayout->addWidget(offsetLayoutSpacer);
    offsetLayout->addWidget(m_shadowOffsetSpinBox);
    shadowsGroupLayout->addLayout(offsetLayout);

    const auto blurRadiusLayout = new QHBoxLayout;
    blurRadiusLayout->addWidget(new QLabel(tr("Shadow blur radius:")));
    const auto blurRadiusLayoutSpacer = new QWidget;
    blurRadiusLayoutSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    blurRadiusLayout->addWidget(blurRadiusLayoutSpacer);
    blurRadiusLayout->addWidget(m_shadowBlurRadiusSpinBox);
    shadowsGroupLayout->addLayout(blurRadiusLayout);

    shadowsGroupLayout->addWidget(m_shadowColorButton);

    const auto selectedItemBlurRadiusLayout = new QHBoxLayout;
    selectedItemBlurRadiusLayout->addWidget(new QLabel(tr("Selected item shadow blur radius:")));
    const auto selectedItemBlurRadiusLayoutSpacer = new QWidget;
    selectedItemBlurRadiusLayoutSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    selectedItemBlurRadiusLayout->addWidget(selectedItemBlurRadiusLayoutSpacer);
    selectedItemBlurRadiusLayout->addWidget(m_selectedItemShadowBlurRadiusSpinBox);
    shadowsGroupLayout->addLayout(selectedItemBlurRadiusLayout);

    shadowsGroupLayout->addWidget(m_selectedItemShadowColorButton);

    shadowsGroupLayout->addWidget(WidgetFactory::buildHorizontalLine());

    shadowsGroupLayout->addWidget(m_optimizeShadowsCheckBox);
    m_optimizeShadowsCheckBox->setToolTip(tr("Optimizing shadow effects makes the performance better, but might introduce some visual glitches."));

    shadowsGroupLayout->addWidget(WidgetFactory::buildHorizontalLine());

    const auto && [resetToDefaultsButton, resetToDefaultsButtonLayout] = WidgetFactory::buildResetToDefaultsButtonWithHLayout();
    shadowsGroupLayout->addLayout(resetToDefaultsButtonLayout);
    connect(resetToDefaultsButton, &QPushButton::clicked, this, [=] {
        m_shadowOffsetSpinBox->setValue(Constants::Settings::defaultShadowEffectOffset());
        m_shadowBlurRadiusSpinBox->setValue(Constants::Settings::defaultShadowEffectBlurRadius());
        m_selectedItemShadowBlurRadiusSpinBox->setValue(Constants::Settings::defaultSelectedItemShadowEffectBlurRadius());
        m_shadowColorButton->resetToDefault();
        m_selectedItemShadowColorButton->resetToDefault();
        apply();
    });

    setLayout(mainLayout);
}

} // namespace Dialogs
