// This file is part of Heimer.
// Copyright (C) 2021 Jussi Lind <jussi.lind@iki.fi>
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

#include "defaults_tab.hpp"

#include "../../application/settings_proxy.hpp"
#include "../widgets/font_button.hpp"
#include "color_setting_button.hpp"
#include "widget_factory.hpp"

#include "simple_logger.hpp"

#include <QButtonGroup>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QVBoxLayout>

namespace Dialogs {

DefaultsTab::DefaultsTab(QString name, QWidget * parent)
  : SettingsTabBase { name, parent }
  , m_edgeDirectionCheckBox(new QCheckBox(tr("Reversed direction"), this))
  , m_arrowSizeSpinBox(new QDoubleSpinBox(this))
  , m_edgeWidthSpinBox(new QDoubleSpinBox(this))
  , m_backgroundColorButton(new ColorSettingButton(tr("Background"), ColorDialog::Role::Background, this))
  , m_edgeColorButton(new ColorSettingButton(tr("Edge color"), ColorDialog::Role::Edge, this))
  , m_gridColorButton(new ColorSettingButton(tr("Grid color"), ColorDialog::Role::Grid, this))
  , m_nodeColorButton(new ColorSettingButton(tr("Node color"), ColorDialog::Role::Node, this))
  , m_nodeTextColorButton(new ColorSettingButton(tr("Node text color"), ColorDialog::Role::Text, this))
  , m_fontButton(new Widgets::FontButton(this))
  , m_textSizeSpinBox(new QSpinBox(this))
{
    initWidgets();
}

void DefaultsTab::apply()
{
    for (auto && [arrowMode, radioButton] : m_edgeArrowStyleRadioMap) {
        if (radioButton->isChecked()) {
            settingsProxy()->setEdgeArrowMode(arrowMode);
            juzzlin::L().info() << "'" << radioButton->text().toStdString() << "' set as new default";
        }
    }

    settingsProxy()->setReversedEdgeDirection(m_edgeDirectionCheckBox->isChecked());
    settingsProxy()->setArrowSize(m_arrowSizeSpinBox->value());
    settingsProxy()->setEdgeWidth(m_edgeWidthSpinBox->value());

    settingsProxy()->setTextSize(m_textSizeSpinBox->value());
    settingsProxy()->setFont(m_fontButton->font());
}

void DefaultsTab::reject()
{
    setActiveDefaults();
}

void DefaultsTab::initWidgets()
{
    const auto mainLayout = new QVBoxLayout;

    const auto defaultsHelp = new QLabel(tr("These default settings will be applied each time a new mind map is created."));
    defaultsHelp->setWordWrap(true);
    mainLayout->addWidget(defaultsHelp);

    createEdgeWidgets(*mainLayout);

    createTextWidgets(*mainLayout);

    createColorWidgets(*mainLayout);

    setLayout(mainLayout);

    setActiveDefaults();
}

void DefaultsTab::createEdgeWidgets(QVBoxLayout & mainLayout)
{
    m_arrowSizeSpinBox->setSingleStep(Constants::Edge::arrowSizeStep());
    m_arrowSizeSpinBox->setMinimum(Constants::Edge::minArrowSize());
    m_arrowSizeSpinBox->setMaximum(Constants::Edge::maxArrowSize());
    m_arrowSizeSpinBox->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_edgeWidthSpinBox->setSingleStep(Constants::Edge::edgeWidthStep());
    m_edgeWidthSpinBox->setMinimum(Constants::Edge::minEdgeWidth());
    m_edgeWidthSpinBox->setMaximum(Constants::Edge::maxEdgeWidth());
    m_edgeWidthSpinBox->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    const auto edgeGroup = WidgetFactory::buildGroupBoxWithVLayout(tr("Edge Arrow Style"), mainLayout);

    using SceneItems::EdgeModel;

    m_edgeArrowStyleRadioMap = {
        { EdgeModel::ArrowMode::Hidden, new QRadioButton(tr("No arrow")) },
        { EdgeModel::ArrowMode::Single, new QRadioButton(tr("Single arrow")) },
        { EdgeModel::ArrowMode::Double, new QRadioButton(tr("Double arrow")) }
    };

    const auto edgeArrowRadioGroup = new QButtonGroup;
    edgeArrowRadioGroup->setParent(this); // Set parent explicitly instead of a constructor arg to silence analyzer warning
    const auto edgeArrowRadioLayout = new QVBoxLayout;
    for (auto && [arrowMode, radioButton] : m_edgeArrowStyleRadioMap) {
        edgeArrowRadioGroup->addButton(radioButton);
        edgeArrowRadioLayout->addWidget(radioButton);
    }
    edgeArrowRadioLayout->addWidget(m_edgeDirectionCheckBox);
    edgeGroup.second->addLayout(edgeArrowRadioLayout);

    edgeArrowRadioLayout->addWidget(WidgetFactory::buildHorizontalLine());

    const auto arrowSizeEdgeWidthLayout = new QHBoxLayout;
    arrowSizeEdgeWidthLayout->addWidget(new QLabel(tr("Arrow size:")));
    arrowSizeEdgeWidthLayout->addWidget(m_arrowSizeSpinBox);
    arrowSizeEdgeWidthLayout->addSpacing(10);
    arrowSizeEdgeWidthLayout->addWidget(new QLabel(tr("Edge width:")));
    arrowSizeEdgeWidthLayout->addWidget(m_edgeWidthSpinBox);
    arrowSizeEdgeWidthLayout->addStretch();
    edgeGroup.second->addLayout(arrowSizeEdgeWidthLayout);

    // Reset to defaults button for edge style
    const auto && [resetToDefaultsButton, resetToDefaultsButtonLayout] = WidgetFactory::buildResetToDefaultsButtonWithHLayout();
    arrowSizeEdgeWidthLayout->addLayout(resetToDefaultsButtonLayout);
    connect(resetToDefaultsButton, &QPushButton::clicked, this, [=] {
        m_edgeArrowStyleRadioMap.at(EdgeModel::ArrowMode::Single)->setChecked(true);
        m_edgeDirectionCheckBox->setChecked(false);
        m_arrowSizeSpinBox->setValue(Constants::Settings::defaultArrowSize());
        m_edgeWidthSpinBox->setValue(Constants::Settings::defaultEdgeWidth());
    });
}

void DefaultsTab::createColorWidgets(QVBoxLayout & mainLayout)
{
    const auto && [colorsGroup, colorsGroupLayout] = WidgetFactory::buildGroupBoxWithVLayout(tr("Colors"), mainLayout);
    const auto colorButtonLayout = new QVBoxLayout;
    colorButtonLayout->addWidget(m_backgroundColorButton);
    m_colorSettingButtons.push_back(m_backgroundColorButton);
    colorButtonLayout->addWidget(m_edgeColorButton);
    m_colorSettingButtons.push_back(m_edgeColorButton);
    colorButtonLayout->addWidget(m_gridColorButton);
    m_colorSettingButtons.push_back(m_gridColorButton);
    colorButtonLayout->addWidget(m_nodeColorButton);
    m_colorSettingButtons.push_back(m_nodeColorButton);
    colorButtonLayout->addWidget(m_nodeTextColorButton);
    m_colorSettingButtons.push_back(m_nodeTextColorButton);
    colorsGroupLayout->addLayout(colorButtonLayout);
    colorButtonLayout->addSpacing(3);

    // Reset to defaults button for colors
    const auto && [resetToDefaultsButton, resetToDefaultsButtonLayout] = WidgetFactory::buildResetToDefaultsButtonWithHLayout();
    colorsGroupLayout->addLayout(resetToDefaultsButtonLayout);
    connect(resetToDefaultsButton, &QPushButton::clicked, this, [=] {
        for (auto && colorSettingButton : m_colorSettingButtons) {
            colorSettingButton->resetToDefault();
        }
    });
}

void DefaultsTab::createTextWidgets(QVBoxLayout & mainLayout)
{
    const auto textGroup = WidgetFactory::buildGroupBoxWithVLayout(tr("Text Style"), mainLayout);
    const auto textLayout = new QHBoxLayout;

    m_textSizeSpinBox->setMinimum(Constants::View::minTextSize());
    m_textSizeSpinBox->setMaximum(Constants::View::maxTextSize());

    textLayout->addWidget(new QLabel(tr("Text size:")));
    textLayout->addWidget(m_textSizeSpinBox);
    textLayout->addSpacing(10);
    textLayout->addWidget(m_fontButton);
    textLayout->addStretch();

    // Reset to defaults button for text styles
    const auto && [resetToDefaultsButton, resetToDefaultsButtonLayout] = WidgetFactory::buildResetToDefaultsButtonWithHLayout();
    connect(resetToDefaultsButton, &QPushButton::clicked, this, [=] {
        m_fontButton->setFont({});
        m_textSizeSpinBox->setValue(Constants::MindMap::defaultTextSize());
    });

    connect(m_fontButton, &Widgets::FontButton::defaultFontSizeRequested, this, [=] {
        m_fontButton->setDefaultPointSize(m_textSizeSpinBox->value());
    });

    connect(m_fontButton, &Widgets::FontButton::fontSizeChanged, m_textSizeSpinBox, &QSpinBox::setValue);

    textLayout->addLayout(resetToDefaultsButtonLayout);

    textGroup.second->addLayout(textLayout);
}

void DefaultsTab::setActiveDefaults()
{
    if (const auto defaultArrowStyle = settingsProxy()->edgeArrowMode(); m_edgeArrowStyleRadioMap.count(defaultArrowStyle)) {
        const auto radio = m_edgeArrowStyleRadioMap[defaultArrowStyle];
        radio->setChecked(true);
    } else {
        juzzlin::L().error() << "Invalid arrow style: " << static_cast<int>(defaultArrowStyle);
    }

    m_edgeDirectionCheckBox->setChecked(settingsProxy()->reversedEdgeDirection());

    m_arrowSizeSpinBox->setValue(settingsProxy()->arrowSize());
    m_edgeWidthSpinBox->setValue(settingsProxy()->edgeWidth());

    m_textSizeSpinBox->setValue(settingsProxy()->textSize());
    m_fontButton->setFont(settingsProxy()->font());
}

} // namespace Dialogs
