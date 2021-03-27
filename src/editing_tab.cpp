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
#include "settings_proxy.hpp"
#include "widget_factory.hpp"

#include <QButtonGroup>
#include <QCheckBox>
#include <QGroupBox>
#include <QVBoxLayout>

EditingTab::EditingTab(QWidget * parent)
  : QWidget(parent)
{
    initWidgets();
}

void EditingTab::apply()
{
    SettingsProxy::instance().setSelectNodeGroupByIntersection(m_selectNodeGroupByIntersectionCheckBox->isChecked());
}

void EditingTab::initWidgets()
{
    const auto mainLayout = new QVBoxLayout;

    const auto editingGroup = WidgetFactory::buildGroupBoxWithVLayout(tr("Selecting Nodes"), *mainLayout);
    m_selectNodeGroupByIntersectionCheckBox = new QCheckBox(tr("Select node group by intersection"));
    editingGroup.second->addWidget(m_selectNodeGroupByIntersectionCheckBox);

    setLayout(mainLayout);

    setActiveDefaults();
}

void EditingTab::setActiveDefaults()
{
    m_selectNodeGroupByIntersectionCheckBox->setChecked(SettingsProxy::instance().selectNodeGroupByIntersection());
}
