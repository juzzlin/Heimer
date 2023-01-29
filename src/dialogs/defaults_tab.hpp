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
#include <vector>

#include "../edge_model.hpp"
#include "settings_tab_base.hpp"

class QCheckBox;
class QDoubleSpinBox;
class QRadioButton;
class QVBoxLayout;
class QSpinBox;

namespace Widgets {
class FontButton;
}

namespace Dialogs {

class ColorSettingButton;

class DefaultsTab : public SettingsTabBase
{
    Q_OBJECT

public:
    explicit DefaultsTab(QString name, QWidget * parent = nullptr);

    void apply() override;

    void reject() override;

private:
    void createColorWidgets(QVBoxLayout & mainLayout);

    void createEdgeWidgets(QVBoxLayout & mainLayout);

    void createTextWidgets(QVBoxLayout & mainLayout);

    void initWidgets();

    void setActiveDefaults();

    std::map<EdgeModel::ArrowMode, QRadioButton *> m_edgeArrowStyleRadioMap;

    QCheckBox * m_edgeDirectionCheckBox = nullptr;

    QDoubleSpinBox * m_arrowSizeSpinBox = nullptr;

    QDoubleSpinBox * m_edgeWidthSpinBox = nullptr;

    ColorSettingButton * m_backgroundColorButton = nullptr;

    ColorSettingButton * m_edgeColorButton = nullptr;

    ColorSettingButton * m_gridColorButton = nullptr;

    ColorSettingButton * m_nodeColorButton = nullptr;

    ColorSettingButton * m_nodeTextColorButton = nullptr;

    std::vector<ColorSettingButton *> m_colorSettingButtons;

    Widgets::FontButton * m_fontButton = nullptr;

    QSpinBox * m_textSizeSpinBox = nullptr;
};

} // namespace Dialogs

#endif // DEFAULTS_TAB_HPP
