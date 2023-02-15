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

#ifndef EDITING_TAB_HPP
#define EDITING_TAB_HPP

#include "settings_tab_base.hpp"

class QCheckBox;

namespace Dialogs {

class EditingTab : public SettingsTabBase
{
    Q_OBJECT

public:
    explicit EditingTab(QString name, QWidget * parent = nullptr);

    void apply() override;

signals:
    void autosaveEnabled(bool enabled);

private:
    void initWidgets();

    void setActiveSettings();

    QCheckBox * m_selectNodeGroupByIntersectionCheckBox = nullptr;

    QCheckBox * m_autosaveCheckBox = nullptr;

    QCheckBox * m_autoloadCheckBox = nullptr;
};

} // namespace Dialogs

#endif // EDITING_TAB_HPP
