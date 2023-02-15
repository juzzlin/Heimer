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

#include "widget_factory.hpp"

#include "../core/settings_proxy.hpp"

#include <QButtonGroup>
#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>

namespace Dialogs {

EditingTab::EditingTab(QString name, QWidget * parent)
  : SettingsTabBase(name, parent)
{
    initWidgets();
}

void EditingTab::apply()
{
    settingsProxy().setSelectNodeGroupByIntersection(m_selectNodeGroupByIntersectionCheckBox->isChecked());

    settingsProxy().setAutosave(m_autosaveCheckBox->isChecked());

    emit autosaveEnabled(m_autosaveCheckBox->isChecked());

    settingsProxy().setAutoload(m_autoloadCheckBox->isChecked());
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

    const auto autoloadGroup = WidgetFactory::buildGroupBoxWithVLayout(tr("Autoload"), *mainLayout);
    const auto autoloadHelp = new QLabel(tr("Autoload feature will automatically load your recent mind map on application start."));
    autoloadHelp->setWordWrap(true);
    autoloadGroup.second->addWidget(autoloadHelp);
    m_autoloadCheckBox = new QCheckBox(tr("Enable autoload"));
    autoloadGroup.second->addWidget(m_autoloadCheckBox);

    setLayout(mainLayout);

    setActiveSettings();
}

void EditingTab::setActiveSettings()
{
    m_selectNodeGroupByIntersectionCheckBox->setChecked(settingsProxy().selectNodeGroupByIntersection());

    m_autosaveCheckBox->setChecked(settingsProxy().autosave());

    m_autoloadCheckBox->setChecked(settingsProxy().autoload());
}

} // namespace Dialogs
