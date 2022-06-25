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

#ifndef SETTINGS_DIALOG_HPP
#define SETTINGS_DIALOG_HPP

#include <QDialog>

class DefaultsTab;
class EditingTab;
class EffectsTab;
struct ShadowEffectParams;

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget * parent = nullptr);

signals:
    void shadowEffectChanged(const ShadowEffectParams & params);

private:
    void accept() override;

    void initWidgets();

    DefaultsTab * m_defaultsTab;

    EditingTab * m_editingTab;

    EffectsTab * m_effectsTab;
};

#endif // SETTINGS_DIALOG_HPP
