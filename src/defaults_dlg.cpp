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

#include "defaults_dlg.hpp"
#include "settings_proxy.hpp"

#include <QButtonGroup>
#include <QCheckBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>

#include "simple_logger.hpp"

DefaultsDlg::DefaultsDlg(QWidget * parent)
  : QDialog(parent)
{
    setWindowTitle(tr("Defaults"));
    setMinimumWidth(640);

    initWidgets();
}

void DefaultsDlg::accept()
{
    for (auto && iter : m_edgeArrowStyleRadioMap) {
        if (iter.second->isChecked()) {
            SettingsProxy::instance().setEdgeArrowMode(iter.first);
            juzzlin::L().info() << "'" << iter.second->text().toStdString() << "' set as new default";
        }
    }

    SettingsProxy::instance().setReversedEdgeDirection(m_edgeDirectionCheckBox->isChecked());

    SettingsProxy::instance().setSelectNodeGroupByIntersection(m_selectNodeGroupByIntersectionCheckBox->isChecked());

    QDialog::accept();
}

void DefaultsDlg::initWidgets()
{
    const auto mainLayout = new QVBoxLayout;
    const auto edgeGroup = new QGroupBox;
    edgeGroup->setTitle(tr("Edge Arrow Style"));
    mainLayout->addWidget(edgeGroup);

    m_edgeArrowStyleRadioMap = {
        { Edge::ArrowMode::Hidden, new QRadioButton(tr("No arrow")) },
        { Edge::ArrowMode::Single, new QRadioButton(tr("Single arrow")) },
        { Edge::ArrowMode::Double, new QRadioButton(tr("Double arrow")) }
    };

    const auto edgeArrowRadioGroup = new QButtonGroup(this);
    const auto edgeArrowRadioLayout = new QVBoxLayout;
    for (auto && iter : m_edgeArrowStyleRadioMap) {
        edgeArrowRadioGroup->addButton(iter.second);
        edgeArrowRadioLayout->addWidget(iter.second);
    }
    m_edgeDirectionCheckBox = new QCheckBox(tr("Reversed direction"));
    edgeArrowRadioLayout->addWidget(m_edgeDirectionCheckBox);
    edgeGroup->setLayout(edgeArrowRadioLayout);

    const auto editingGroup = new QGroupBox;
    editingGroup->setTitle(tr("Editing"));
    const auto editingLayout = new QVBoxLayout;
    editingGroup->setLayout(editingLayout);
    mainLayout->addWidget(editingGroup);
    m_selectNodeGroupByIntersectionCheckBox = new QCheckBox(tr("Select node group by intersection"));
    editingLayout->addWidget(m_selectNodeGroupByIntersectionCheckBox);
    edgeGroup->setLayout(edgeArrowRadioLayout);

    const auto buttonLayout = new QHBoxLayout();
    const auto button = new QPushButton("&Ok", this);
    connect(button, &QPushButton::clicked, this, &DefaultsDlg::accept);
    buttonLayout->addWidget(button);
    buttonLayout->insertStretch(0);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);

    setActiveDefaults();
}

void DefaultsDlg::setActiveDefaults()
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

    m_selectNodeGroupByIntersectionCheckBox->setChecked(SettingsProxy::instance().selectNodeGroupByIntersection());
}
