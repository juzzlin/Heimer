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
#include "../graphics_factory.hpp"
#include "../settings_proxy.hpp"
#include "../widget_factory.hpp"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

EffectsTab::EffectsTab(QWidget * parent)
  : QWidget(parent)
  , m_shadowOffsetSpinBox(new QSpinBox(this))
  , m_shadowNormalBlurRadiusSpinBox(new QSpinBox(this))
  , m_shadowSelectedBlurRadiusSpinBox(new QSpinBox(this))
{
    m_shadowOffsetSpinBox->setMinimum(Constants::Effects::SHADOW_EFFECT_MIN_OFFSET);
    m_shadowOffsetSpinBox->setMaximum(Constants::Effects::SHADOW_EFFECT_MAX_OFFSET);
    m_shadowOffsetSpinBox->setValue(SettingsProxy::instance().shadowEffect().offset);

    m_shadowNormalBlurRadiusSpinBox->setMinimum(Constants::Effects::SHADOW_EFFECT_MIN_BLUR_RADIUS);
    m_shadowNormalBlurRadiusSpinBox->setMaximum(Constants::Effects::SHADOW_EFFECT_MAX_BLUR_RADIUS);
    m_shadowNormalBlurRadiusSpinBox->setValue(SettingsProxy::instance().shadowEffect().blurRadiusNormal);

    m_shadowSelectedBlurRadiusSpinBox->setMinimum(Constants::Effects::SHADOW_EFFECT_MIN_BLUR_RADIUS);
    m_shadowSelectedBlurRadiusSpinBox->setMaximum(Constants::Effects::SHADOW_EFFECT_MAX_BLUR_RADIUS);
    m_shadowSelectedBlurRadiusSpinBox->setValue(SettingsProxy::instance().shadowEffect().blurRadiusSelected);

    initWidgets();

#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    const auto signal = QOverload<int>::of(&QSpinBox::valueChanged);
    const auto target = QOverload<>::of(&EffectsTab::apply);
#else
    const auto signal = static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged);
    const auto target = static_cast<void (EffectsTab::*)()>(&EffectsTab::apply);
#endif
    connect(m_shadowOffsetSpinBox, signal, this, target);
    connect(m_shadowNormalBlurRadiusSpinBox, signal, this, target);
    connect(m_shadowSelectedBlurRadiusSpinBox, signal, this, target);
}

void EffectsTab::apply()
{
    apply({ m_shadowOffsetSpinBox->value(), m_shadowNormalBlurRadiusSpinBox->value(), m_shadowSelectedBlurRadiusSpinBox->value() });
}

void EffectsTab::apply(const GraphicsFactory::ShadowEffectParams & params)
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
    offsetLayout->addWidget(new QLabel(tr("Normal shadow effect offset:")));
    const auto spacer1 = new QWidget;
    spacer1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    offsetLayout->addWidget(spacer1);
    offsetLayout->addWidget(m_shadowOffsetSpinBox);

    const auto normalBlurRadiusLayout = new QHBoxLayout;
    normalBlurRadiusLayout->addWidget(new QLabel(tr("Normal shadow blur radius:")));
    const auto spacer2 = new QWidget;
    spacer2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    normalBlurRadiusLayout->addWidget(spacer2);
    normalBlurRadiusLayout->addWidget(m_shadowNormalBlurRadiusSpinBox);

    const auto selectedBlurRadiusLayout = new QHBoxLayout;
    selectedBlurRadiusLayout->addWidget(new QLabel(tr("Selected item shadow blur radius:")));
    const auto spacer3 = new QWidget;
    spacer3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    selectedBlurRadiusLayout->addWidget(spacer3);
    selectedBlurRadiusLayout->addWidget(m_shadowSelectedBlurRadiusSpinBox);

    shadowsGroup.second->addLayout(offsetLayout);
    shadowsGroup.second->addLayout(normalBlurRadiusLayout);
    shadowsGroup.second->addLayout(selectedBlurRadiusLayout);

    const auto resetToDefaultsButton = WidgetFactory::buildResetToDefaultsButtonWithHLayout();
    shadowsGroup.second->addLayout(resetToDefaultsButton.second);
    connect(resetToDefaultsButton.first, &QPushButton::clicked, this, [=] {
        m_shadowOffsetSpinBox->setValue(Constants::Effects::Defaults::SHADOW_EFFECT_OFFSET);
        m_shadowNormalBlurRadiusSpinBox->setValue(Constants::Effects::Defaults::SHADOW_EFFECT_NORMAL_BLUR_RADIUS);
        m_shadowSelectedBlurRadiusSpinBox->setValue(Constants::Effects::Defaults::SHADOW_EFFECT_SELECTED_BLUR_RADIUS);
        apply();
    });

    setLayout(mainLayout);
}
