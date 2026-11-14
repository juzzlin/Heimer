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

static const auto TAG = "DefaultsTab";

DefaultsTab::DefaultsTab(QString name, QWidget * parent)
  : SettingsTabBase { name, parent }
  , m_edgeDirectionCheckBox(new QCheckBox { tr("Reversed direction"), this })
{
    initWidgets();
}

bool DefaultsTab::apply()
{
    for (auto && [arrowMode, radioButton] : m_edgeArrowStyleRadioMap) {
        if (radioButton->isChecked()) {
            settingsProxy()->setEdgeArrowMode(arrowMode);
            juzzlin::L(TAG).info() << "'" << radioButton->text().toStdString() << "' set as new default";
        }
    }

    settingsProxy()->setReversedEdgeDirection(m_edgeDirectionCheckBox->isChecked());

    return true;
}

void DefaultsTab::reject()
{
    setActiveDefaults();
}

void DefaultsTab::initWidgets()
{
    const auto mainLayout = new QVBoxLayout;

    const auto defaultsHelp = new QLabel { tr("These default settings will be applied to new edges. "
                                              "The colors, fonts and sizes of new mind maps are defined by the default theme, see Themes."),
                                           this };
    defaultsHelp->setWordWrap(true);
    mainLayout->addWidget(defaultsHelp);

    createEdgeWidgets(*mainLayout);

    mainLayout->addStretch();

    setLayout(mainLayout);

    setActiveDefaults();
}

void DefaultsTab::createEdgeWidgets(QVBoxLayout & mainLayout)
{
    const auto edgeGroup = WidgetFactory::buildGroupBoxWithVLayout(tr("Edge Arrow Style"), mainLayout);

    using SceneItems::EdgeModel;

    m_edgeArrowStyleRadioMap = {
        { EdgeModel::ArrowMode::Hidden, new QRadioButton { tr("No arrow") } },
        { EdgeModel::ArrowMode::Single, new QRadioButton { tr("Single arrow") } },
        { EdgeModel::ArrowMode::Double, new QRadioButton { tr("Double arrow") } }
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

    // Reset to defaults button for edge style
    const auto [resetToDefaultsButton, resetToDefaultsButtonLayout] = WidgetFactory::buildResetToDefaultsButtonWithHLayout();
    edgeGroup.second->addLayout(resetToDefaultsButtonLayout);
    connect(resetToDefaultsButton, &QPushButton::clicked, this, [=] {
        m_edgeArrowStyleRadioMap.at(EdgeModel::ArrowMode::Single)->setChecked(true);
        m_edgeDirectionCheckBox->setChecked(false);
    });
}

void DefaultsTab::setActiveDefaults()
{
    if (const auto defaultArrowStyle = settingsProxy()->edgeArrowMode(); m_edgeArrowStyleRadioMap.count(defaultArrowStyle)) {
        const auto radio = m_edgeArrowStyleRadioMap[defaultArrowStyle];
        radio->setChecked(true);
    } else {
        juzzlin::L(TAG).error() << "Invalid arrow style: " << static_cast<int>(defaultArrowStyle);
    }

    m_edgeDirectionCheckBox->setChecked(settingsProxy()->reversedEdgeDirection());
}

} // namespace Dialogs
