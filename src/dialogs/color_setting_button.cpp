// This file is part of Heimer.
// Copyright (C) 2022 Jussi Lind <jussi.lind@iki.fi>
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

#include "color_setting_button.hpp"
#include "../settings_proxy.hpp"

namespace Dialogs {

ColorSettingButton::ColorSettingButton(QString text, ColorDialog::Role role, QWidget * parent)
  : QPushButton(text, parent)
  , m_role(role)
{
    // Set current default color
    if (const std::map<ColorDialog::Role, QColor> roleToColorMap = {
          { ColorDialog::Role::Background, SettingsProxy::instance().backgroundColor() },
          { ColorDialog::Role::Edge, SettingsProxy::instance().edgeColor() },
          { ColorDialog::Role::Grid, SettingsProxy::instance().gridColor() },
          { ColorDialog::Role::Node, SettingsProxy::instance().nodeColor() },
          { ColorDialog::Role::Text, SettingsProxy::instance().nodeTextColor() },
          { ColorDialog::Role::ShadowColor, SettingsProxy::instance().shadowEffect().shadowColor },
          { ColorDialog::Role::SelectedItemShadowColor, SettingsProxy::instance().shadowEffect().selectedItemShadowColor } };
        roleToColorMap.count(role)) {
        setColor(roleToColorMap.at(role));
    }

    // Apply chosen default color
    connect(this, &QPushButton::clicked, this, [=] {
        ColorDialog cd(role);
        cd.exec();
        apply(cd.color());
    });
}

const QColor & ColorSettingButton::selectedColor() const
{
    return m_selectedColor;
}

void ColorSettingButton::apply(QColor color)
{
    if (color.isValid()) {
        setColor(color);
        switch (m_role) {
        case ColorDialog::Role::Background:
            SettingsProxy::instance().setBackgroundColor(color);
            break;
        case ColorDialog::Role::Edge:
            SettingsProxy::instance().setEdgeColor(color);
            break;
        case ColorDialog::Role::Grid:
            SettingsProxy::instance().setGridColor(color);
            break;
        case ColorDialog::Role::Node:
            SettingsProxy::instance().setNodeColor(color);
            break;
        case ColorDialog::Role::Text:
            SettingsProxy::instance().setNodeTextColor(color);
            break;
        case ColorDialog::Role::ShadowColor:
            // Saving shadow effect handled in effects_tab.cpp
        case ColorDialog::Role::SelectedItemShadowColor:
            // Saving shadow effect handled in effects_tab.cpp
            break;
        }
        emit colorSelected(m_selectedColor);
    }
}

void ColorSettingButton::resetToDefault()
{
    if (const std::map<ColorDialog::Role, QColor> roleToColorMap = {
          { ColorDialog::Role::Background, Constants::MindMap::Defaults::BACKGROUND_COLOR },
          { ColorDialog::Role::Edge, Constants::MindMap::Defaults::EDGE_COLOR },
          { ColorDialog::Role::Grid, Constants::MindMap::Defaults::GRID_COLOR },
          { ColorDialog::Role::Node, Constants::MindMap::Defaults::NODE_COLOR },
          { ColorDialog::Role::Text, Constants::MindMap::Defaults::NODE_TEXT_COLOR },
          { ColorDialog::Role::ShadowColor, Constants::Effects::Defaults::SHADOW_EFFECT_SHADOW_COLOR },
          { ColorDialog::Role::SelectedItemShadowColor, Constants::Effects::Defaults::SHADOW_EFFECT_SELECTED_ITEM_SHADOW_COLOR } };
        roleToColorMap.count(m_role)) {
        setColor(roleToColorMap.at(m_role));
        apply(roleToColorMap.at(m_role));
    }
}

static int getTextColorComponent(int backgroundColorComponent)
{
    return (backgroundColorComponent + 127) % 256;
}

void ColorSettingButton::setColor(QColor backgroundColor)
{
    m_selectedColor = backgroundColor;
    const QColor textColor = { getTextColorComponent(backgroundColor.red()), getTextColorComponent(backgroundColor.green()), getTextColorComponent(backgroundColor.blue()) };
    setStyleSheet(QString("color: %1; background-color: %2").arg(textColor.name(), backgroundColor.name()));
}

} // namespace Dialogs
