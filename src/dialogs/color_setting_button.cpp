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

#include "../core/settings_proxy.hpp"
#include "../single_instance_container.hpp"

namespace Dialogs {

ColorSettingButton::ColorSettingButton(QString text, ColorDialog::Role role, QWidget * parent)
  : QPushButton(text, parent)
  , m_role(role)
  , m_settingsProxy(SingleInstanceContainer::instance().settingsProxy())
{
    // Set current default color
    if (const std::map<ColorDialog::Role, QColor> roleToColorMap = {
          { ColorDialog::Role::Background, m_settingsProxy->backgroundColor() },
          { ColorDialog::Role::Edge, m_settingsProxy->edgeColor() },
          { ColorDialog::Role::Grid, m_settingsProxy->gridColor() },
          { ColorDialog::Role::Node, m_settingsProxy->nodeColor() },
          { ColorDialog::Role::Text, m_settingsProxy->nodeTextColor() },
          { ColorDialog::Role::ShadowColor, m_settingsProxy->shadowEffect().shadowColor },
          { ColorDialog::Role::SelectedItemShadowColor, m_settingsProxy->shadowEffect().selectedItemShadowColor } };
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
            m_settingsProxy->setBackgroundColor(color);
            break;
        case ColorDialog::Role::Edge:
            m_settingsProxy->setEdgeColor(color);
            break;
        case ColorDialog::Role::Grid:
            m_settingsProxy->setGridColor(color);
            break;
        case ColorDialog::Role::Node:
            m_settingsProxy->setNodeColor(color);
            break;
        case ColorDialog::Role::Text:
            m_settingsProxy->setNodeTextColor(color);
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
