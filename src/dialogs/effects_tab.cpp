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

#include "../constants.hpp"
#include "../settings_proxy.hpp"
#include "../shadow_effect_params.hpp"
#include "../widget_factory.hpp"
#include "color_setting_button.hpp"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

EffectsTab::EffectsTab(QWidget * parent)
  : QWidget(parent)
  , m_shadowOffsetSpinBox(new QSpinBox(this))
  , m_shadowBlurRadiusSpinBox(new QSpinBox(this))
  , m_selectedItemShadowBlurRadiusSpinBox(new QSpinBox(this))
  , m_shadowColorButton(new ColorSettingButton(tr("Shadow color"), ColorDialog::Role::ShadowColor, this))
  , m_selectedItemShadowColorButton(new ColorSettingButton(tr("Selected item shadow color"), ColorDialog::Role::SelectedItemShadowColor, this))
{
    m_shadowOffsetSpinBox->setMinimum(Constants::Effects::SHADOW_EFFECT_MIN_OFFSET);
    m_shadowOffsetSpinBox->setMaximum(Constants::Effects::SHADOW_EFFECT_MAX_OFFSET);
    m_shadowOffsetSpinBox->setValue(SettingsProxy::instance().shadowEffect().offset);

    m_shadowBlurRadiusSpinBox->setMinimum(Constants::Effects::SHADOW_EFFECT_MIN_BLUR_RADIUS);
    m_shadowBlurRadiusSpinBox->setMaximum(Constants::Effects::SHADOW_EFFECT_MAX_BLUR_RADIUS);
    m_shadowBlurRadiusSpinBox->setValue(SettingsProxy::instance().shadowEffect().blurRadius);

    m_selectedItemShadowBlurRadiusSpinBox->setMinimum(Constants::Effects::SHADOW_EFFECT_MIN_BLUR_RADIUS);
    m_selectedItemShadowBlurRadiusSpinBox->setMaximum(Constants::Effects::SHADOW_EFFECT_MAX_BLUR_RADIUS);
    m_selectedItemShadowBlurRadiusSpinBox->setValue(SettingsProxy::instance().shadowEffect().selectedItemBlurRadius);

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
    if (SettingsProxy::instance().shadowEffect() != params) {
        SettingsProxy::instance().setShadowEffect(params);
        emit shadowEffectChanged(params);
    }
}

void EffectsTab::initWidgets()
{
    const auto mainLayout = new QVBoxLayout;
    const auto shadowsGroup = WidgetFactory::buildGroupBoxWithVLayout(tr("Shadows"), *mainLayout);

    const auto offsetLayout = new QHBoxLayout;
    offsetLayout->addWidget(new QLabel(tr("Shadow effect offset:")));
    const auto spacer1 = new QWidget;
    spacer1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    offsetLayout->addWidget(spacer1);
    offsetLayout->addWidget(m_shadowOffsetSpinBox);
    shadowsGroup.second->addLayout(offsetLayout);

    const auto blurRadiusLayout = new QHBoxLayout;
    blurRadiusLayout->addWidget(new QLabel(tr("Shadow blur radius:")));
    const auto spacer2 = new QWidget;
    spacer2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    blurRadiusLayout->addWidget(spacer2);
    blurRadiusLayout->addWidget(m_shadowBlurRadiusSpinBox);
    shadowsGroup.second->addLayout(blurRadiusLayout);

    shadowsGroup.second->addWidget(m_shadowColorButton);

    const auto selectedItemBlurRadiusLayout = new QHBoxLayout;
    selectedItemBlurRadiusLayout->addWidget(new QLabel(tr("Selected item shadow blur radius:")));
    const auto spacer3 = new QWidget;
    spacer3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    selectedItemBlurRadiusLayout->addWidget(spacer3);
    selectedItemBlurRadiusLayout->addWidget(m_selectedItemShadowBlurRadiusSpinBox);
    shadowsGroup.second->addLayout(selectedItemBlurRadiusLayout);

    shadowsGroup.second->addWidget(m_selectedItemShadowColorButton);

    const auto resetToDefaultsButton = WidgetFactory::buildResetToDefaultsButtonWithHLayout();
    shadowsGroup.second->addLayout(resetToDefaultsButton.second);
    connect(resetToDefaultsButton.first, &QPushButton::clicked, this, [=] {
        m_shadowOffsetSpinBox->setValue(Constants::Effects::Defaults::SHADOW_EFFECT_OFFSET);
        m_shadowBlurRadiusSpinBox->setValue(Constants::Effects::Defaults::SHADOW_EFFECT_BLUR_RADIUS);
        m_selectedItemShadowBlurRadiusSpinBox->setValue(Constants::Effects::Defaults::SELECTED_ITEM_SHADOW_EFFECT_BLUR_RADIUS);
        m_shadowColorButton->resetToDefault();
        m_selectedItemShadowColorButton->resetToDefault();
        apply();
    });

    setLayout(mainLayout);
}
