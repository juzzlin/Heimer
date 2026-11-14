// This file is part of Heimer.
// Copyright (C) 2026 Jussi Lind <jussi.lind@iki.fi>
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

#include "factory_themes.hpp"

namespace FactoryThemes {

QString classicName()
{
    return "Classic";
}

static Theme classic()
{
    Theme theme;
    theme.name = classicName();
    return theme;
}

static Theme highContrast()
{
    Theme theme;
    theme.name = "High Contrast";
    theme.backgroundColor = QColor { "#000000" };
    theme.gridColor = QColor { "#404040" };
    theme.nodeColor = QColor { "#ffff00" };
    theme.nodeTextColor = QColor { "#000000" };
    theme.cornerRadius = 0;
    theme.textSize = 16;
    theme.edgeColor = QColor { "#ffffff" };
    theme.edgeWidth = 3;
    theme.arrowSize = 14;
    theme.edgeLabelColor = QColor { "#00ffff" };
    theme.edgeLabelTextColor = QColor { "#000000" };
    // Shadows only reduce the contrast, but keep a strong selection highlight
    theme.shadowEffect = { 0, 0, 50, QColor { 0, 0, 0, 0 }, QColor { "#ff00ff" } };
    return theme;
}

static Theme black()
{
    Theme theme;
    theme.name = "Black";
    theme.backgroundColor = QColor { "#121212" };
    theme.gridColor = QColor { "#2a2a2a" };
    theme.nodeColor = QColor { "#2d2d2d" };
    theme.nodeTextColor = QColor { "#eeeeee" };
    theme.edgeColor = QColor { "#8a8a8a" };
    theme.edgeLabelColor = QColor { "#3a3a3a" };
    theme.edgeLabelTextColor = QColor { "#eeeeee" };
    theme.shadowEffect = { 3, 8, 50, QColor { 0, 0, 0, 220 }, QColor { "#4fc3f7" } };
    return theme;
}

static Theme white()
{
    Theme theme;
    theme.name = "White";
    theme.backgroundColor = QColor { "#ffffff" };
    theme.gridColor = QColor { "#ededed" };
    theme.nodeColor = QColor { "#ffffff" };
    theme.nodeTextColor = QColor { "#212121" };
    theme.edgeColor = QColor { "#9e9e9e" };
    theme.edgeLabelColor = QColor { "#f2f2f2" };
    theme.edgeLabelTextColor = QColor { "#212121" };
    theme.shadowEffect = { 2, 8, 50, QColor { 0, 0, 0, 60 }, QColor { "#2196f3" } };
    return theme;
}

static Theme accent()
{
    // Neutral greys with a single accent color for the connections and the selection
    const QColor accentColor { "#e8590c" };
    Theme theme;
    theme.name = "Accent";
    theme.backgroundColor = QColor { "#f4f5f7" };
    theme.gridColor = QColor { "#e2e4e8" };
    theme.nodeColor = QColor { "#ffffff" };
    theme.nodeTextColor = QColor { "#2b2f36" };
    theme.edgeColor = accentColor;
    theme.edgeLabelColor = accentColor;
    theme.edgeLabelTextColor = QColor { "#ffffff" };
    theme.shadowEffect = { 2, 6, 50, QColor { 0, 0, 0, 50 }, accentColor };
    return theme;
}

static Theme paper()
{
    // Warm notebook: cream paper, faint grid and ink-colored text
    Theme theme;
    theme.name = "Paper";
    theme.backgroundColor = QColor { "#f6f0e1" };
    theme.gridColor = QColor { "#e6dcc4" };
    theme.nodeColor = QColor { "#fffaf0" };
    theme.nodeTextColor = QColor { "#2e3a59" };
    theme.cornerRadius = 2;
    theme.font.setStyleHint(QFont::Serif);
    theme.font.setFamily("Serif");
    theme.edgeColor = QColor { "#6b5b45" };
    theme.edgeWidth = 1.5;
    theme.edgeLabelColor = QColor { "#efe3c8" };
    theme.edgeLabelTextColor = QColor { "#2e3a59" };
    theme.shadowEffect = { 2, 4, 40, QColor { 90, 70, 40, 70 }, QColor { "#b5651d" } };
    return theme;
}

std::vector<Theme> themes()
{
    return { classic(), highContrast(), black(), white(), accent(), paper() };
}

} // namespace FactoryThemes
