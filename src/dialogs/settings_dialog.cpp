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

#include "settings_dialog.hpp"

#include "../shadow_effect_params.hpp"
#include "defaults_tab.hpp"
#include "editing_tab.hpp"
#include "effects_tab.hpp"

#include <QDialogButtonBox>
#include <QTabWidget>
#include <QVBoxLayout>

namespace Dialogs {

SettingsDialog::SettingsDialog(QWidget * parent)
  : QDialog(parent)
  , m_defaultsTab(new DefaultsTab(this))
  , m_editingTab(new EditingTab(this))
  , m_effectsTab(new EffectsTab(this))
{
    setWindowTitle(tr("Settings"));
    setMinimumWidth(640);

    initWidgets();

    connect(m_effectsTab, &EffectsTab::shadowEffectChanged, this, &SettingsDialog::shadowEffectChanged);

    connect(m_editingTab, &EditingTab::autosaveEnabled, this, &SettingsDialog::autosaveEnabled);
}

void SettingsDialog::accept()
{
    m_defaultsTab->apply();
    m_editingTab->apply();
    m_effectsTab->apply();

    QDialog::accept();
}

void SettingsDialog::reject()
{
    m_defaultsTab->reject();

    QDialog::reject();
}

void SettingsDialog::initWidgets()
{
    const auto tabWidget = new QTabWidget;

    tabWidget->addTab(m_defaultsTab, tr("Defaults"));
    tabWidget->addTab(m_editingTab, tr("Editing"));
    tabWidget->addTab(m_effectsTab, tr("Effects"));

    const auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                | QDialogButtonBox::Cancel);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    const auto mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);
}

} // namespace Dialogs
