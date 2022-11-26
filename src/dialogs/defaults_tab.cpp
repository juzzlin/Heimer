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

#include "../settings_proxy.hpp"
#include "color_setting_button.hpp"
#include "widget_factory.hpp"

#include "simple_logger.hpp"

#include <QButtonGroup>
#include <QCheckBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>

namespace Dialogs {

DefaultsTab::DefaultsTab(QWidget * parent)
  : QWidget(parent)
  , m_edgeDirectionCheckBox(new QCheckBox(tr("Reversed direction"), this))
  , m_backgroundColorButton(new ColorSettingButton(tr("Background"), ColorDialog::Role::Background, this))
  , m_edgeColorButton(new ColorSettingButton(tr("Edge color"), ColorDialog::Role::Edge, this))
  , m_gridColorButton(new ColorSettingButton(tr("Grid color"), ColorDialog::Role::Grid, this))
  , m_nodeColorButton(new ColorSettingButton(tr("Node color"), ColorDialog::Role::Node, this))
  , m_nodeTextColorButton(new ColorSettingButton(tr("Node text color"), ColorDialog::Role::Text, this))
{
    initWidgets();
}

void DefaultsTab::apply()
{
    for (auto && iter : m_edgeArrowStyleRadioMap) {
        if (iter.second->isChecked()) {
            SettingsProxy::instance().setEdgeArrowMode(iter.first);
            juzzlin::L().info() << "'" << iter.second->text().toStdString() << "' set as new default";
        }
    }

    SettingsProxy::instance().setReversedEdgeDirection(m_edgeDirectionCheckBox->isChecked());
}

void DefaultsTab::initWidgets()
{
    const auto mainLayout = new QVBoxLayout;

    const auto defaultsHelp = new QLabel(tr("These default settings will be applied each time a new mind map is created."));
    defaultsHelp->setWordWrap(true);
    mainLayout->addWidget(defaultsHelp);

    createEdgeWidgets(*mainLayout);

    createColorWidgets(*mainLayout);

    setLayout(mainLayout);

    setActiveDefaults();
}

void DefaultsTab::createEdgeWidgets(QVBoxLayout & mainLayout)
{
    const auto edgeGroup = WidgetFactory::buildGroupBoxWithVLayout(tr("Edge Arrow Style"), mainLayout);

    m_edgeArrowStyleRadioMap = {
        { EdgeModel::ArrowMode::Hidden, new QRadioButton(tr("No arrow")) },
        { EdgeModel::ArrowMode::Single, new QRadioButton(tr("Single arrow")) },
        { EdgeModel::ArrowMode::Double, new QRadioButton(tr("Double arrow")) }
    };

    const auto edgeArrowRadioGroup = new QButtonGroup;
    edgeArrowRadioGroup->setParent(this); // Set parent explicitly instead of a constructor arg to silence analyzer warning
    const auto edgeArrowRadioLayout = new QVBoxLayout;
    for (auto && iter : m_edgeArrowStyleRadioMap) {
        edgeArrowRadioGroup->addButton(iter.second);
        edgeArrowRadioLayout->addWidget(iter.second);
    }
    edgeArrowRadioLayout->addWidget(m_edgeDirectionCheckBox);
    edgeGroup.second->addLayout(edgeArrowRadioLayout);
}

void DefaultsTab::createColorWidgets(QVBoxLayout & mainLayout)
{
    const auto colorsGroup = WidgetFactory::buildGroupBoxWithVLayout(tr("Colors"), mainLayout);
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
    colorsGroup.second->addLayout(colorButtonLayout);

    const auto resetToDefaultsButton = WidgetFactory::buildResetToDefaultsButtonWithHLayout();
    colorsGroup.second->addLayout(resetToDefaultsButton.second);
    connect(resetToDefaultsButton.first, &QPushButton::clicked, this, [=] {
        for (auto && colorSettingButton : m_colorSettingButtons) {
            colorSettingButton->resetToDefault();
        }
    });
}

void DefaultsTab::setActiveDefaults()
{
    const auto defaultArrowStyle = SettingsProxy::instance().edgeArrowMode();
    if (m_edgeArrowStyleRadioMap.count(defaultArrowStyle)) {
        const auto radio = m_edgeArrowStyleRadioMap[defaultArrowStyle];
        radio->setChecked(true);
        juzzlin::L().info() << "'" << radio->text().toStdString() << "' set as active default";
    } else {
        juzzlin::L().error() << "Invalid arrow style: " << static_cast<int>(defaultArrowStyle);
    }

    m_edgeDirectionCheckBox->setChecked(SettingsProxy::instance().reversedEdgeDirection());
}

} // namespace Dialogs
