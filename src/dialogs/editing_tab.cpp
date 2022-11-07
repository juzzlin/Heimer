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

#include "editing_tab.hpp"
#include "../settings_proxy.hpp"
#include "../widget_factory.hpp"

#include <QButtonGroup>
#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>

EditingTab::EditingTab(QWidget * parent)
  : QWidget(parent)
{
    initWidgets();
}

void EditingTab::apply()
{
    SettingsProxy::instance().setSelectNodeGroupByIntersection(m_selectNodeGroupByIntersectionCheckBox->isChecked());

    SettingsProxy::instance().setAutosave(m_autosaveCheckBox->isChecked());
}

void EditingTab::initWidgets()
{
    const auto mainLayout = new QVBoxLayout;

    const auto editingGroup = WidgetFactory::buildGroupBoxWithVLayout(tr("Selecting Nodes"), *mainLayout);
    m_selectNodeGroupByIntersectionCheckBox = new QCheckBox(tr("Select node group by intersection"));
    editingGroup.second->addWidget(m_selectNodeGroupByIntersectionCheckBox);

    const auto autosaveGroup = WidgetFactory::buildGroupBoxWithVLayout(tr("Autosave"), *mainLayout);
    const auto autosaveHelp = new QLabel(tr("Autosave feature will automatically save your mind map on every modification after it has been initially saved once."));
    autosaveHelp->setWordWrap(true);
    autosaveGroup.second->addWidget(autosaveHelp);
    m_autosaveCheckBox = new QCheckBox(tr("Enable autosave"));
    autosaveGroup.second->addWidget(m_autosaveCheckBox);

    setLayout(mainLayout);

    setActiveSettings();

    connect(m_autosaveCheckBox, &QCheckBox::stateChanged, this, [=](auto state) {
        emit autosaveEnabled(state == Qt::CheckState::Checked);
    });
}

void EditingTab::setActiveSettings()
{
    m_selectNodeGroupByIntersectionCheckBox->setChecked(SettingsProxy::instance().selectNodeGroupByIntersection());

    m_autosaveCheckBox->setChecked(SettingsProxy::instance().autosave());
}
