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

#ifndef DEFAULTS_TAB_HPP
#define DEFAULTS_TAB_HPP

#include <map>

#include "../scene_items/edge_model.hpp"
#include "settings_tab_base.hpp"

class QCheckBox;
class QRadioButton;
class QVBoxLayout;

namespace Dialogs {

class DefaultsTab : public SettingsTabBase
{
    Q_OBJECT

public:
    explicit DefaultsTab(QString name, QWidget * parent = nullptr);

    bool apply() override;

    void reject() override;

private:
    void createEdgeWidgets(QVBoxLayout & mainLayout);

    void initWidgets();

    void setActiveDefaults();

    std::map<SceneItems::EdgeModel::ArrowMode, QRadioButton *> m_edgeArrowStyleRadioMap;

    QCheckBox * m_edgeDirectionCheckBox = nullptr;
};

} // namespace Dialogs

#endif // DEFAULTS_TAB_HPP
