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

#include "../../application/settings_proxy.hpp"

#include <QButtonGroup>
#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>

namespace Dialogs {

EditingTab::EditingTab(QString name, QWidget * parent)
  : SettingsTabBase { name, parent }
  , m_selectNodeGroupByIntersectionCheckBox(new QCheckBox(tr("Select node group by intersection")))
  , m_raiseNodeOnMouseHoverCheckBox(new QCheckBox(tr("Raise node on mouse hover")))
  , m_invertedControlsCheckBox(new QCheckBox(tr("Inverted controls")))
  , m_autoloadCheckBox(new QCheckBox(tr("Enable autoload")))
  , m_autosaveCheckBox(new QCheckBox(tr("Enable autosave")))
{
    initWidgets();
}

void EditingTab::apply()
{
    settingsProxy()->setSelectNodeGroupByIntersection(m_selectNodeGroupByIntersectionCheckBox->isChecked());

    settingsProxy()->setRaiseNodeOnMouseHover(m_raiseNodeOnMouseHoverCheckBox->isChecked());

    settingsProxy()->setAutosave(m_autosaveCheckBox->isChecked());

    emit autosaveEnabled(m_autosaveCheckBox->isChecked());

    settingsProxy()->setAutoload(m_autoloadCheckBox->isChecked());

    settingsProxy()->setInvertedControls(m_invertedControlsCheckBox->isChecked());
}

void EditingTab::initWidgets()
{
    const auto mainLayout = new QVBoxLayout;

    const auto && [editingGroup, editingGroupLayout] = WidgetFactory::buildGroupBoxWithVLayout(tr("Selecting Nodes"), *mainLayout);
    m_selectNodeGroupByIntersectionCheckBox->setToolTip(tr("The rectangle selection will select nodes also by intersection instead of inclusion only."));
    editingGroupLayout->addWidget(m_selectNodeGroupByIntersectionCheckBox);

    m_raiseNodeOnMouseHoverCheckBox->setToolTip(tr("Moving mouse cursor on a zoomed-out node will raise it for easier editing."));
    editingGroupLayout->addWidget(m_raiseNodeOnMouseHoverCheckBox);

    const auto && [fileOperationsGroup, fileOperationsGroupLayout] = WidgetFactory::buildGroupBoxWithVLayout(tr("File Operations"), *mainLayout);
    m_autosaveCheckBox->setToolTip(tr("Autosave feature will automatically save your mind map on every modification after it has been initially saved once."));
    fileOperationsGroupLayout->addWidget(m_autosaveCheckBox);

    fileOperationsGroupLayout->addWidget(WidgetFactory::buildHorizontalLine());

    m_autoloadCheckBox->setToolTip(tr("Autoload feature will automatically load your recent mind map on application start."));
    fileOperationsGroupLayout->addWidget(m_autoloadCheckBox);

    const auto && [controlsGroup, controlsGroupLayout] = WidgetFactory::buildGroupBoxWithVLayout(tr("Controls"), *mainLayout);
    m_invertedControlsCheckBox->setToolTip(tr("Scroll the view with a modifier key pressed and select a group of items without a modifier key being pressed."));
    controlsGroupLayout->addWidget(m_invertedControlsCheckBox);

    setLayout(mainLayout);

    setActiveSettings();
}

void EditingTab::setActiveSettings()
{
    m_selectNodeGroupByIntersectionCheckBox->setChecked(settingsProxy()->selectNodeGroupByIntersection());

    m_raiseNodeOnMouseHoverCheckBox->setChecked(settingsProxy()->raiseNodeOnMouseHover());

    m_autosaveCheckBox->setChecked(settingsProxy()->autosave());

    m_autoloadCheckBox->setChecked(settingsProxy()->autoload());

    m_invertedControlsCheckBox->setChecked(settingsProxy()->invertedControls());
}

} // namespace Dialogs
