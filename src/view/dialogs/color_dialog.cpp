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

#include "color_dialog.hpp"
#include "../../common/constants.hpp"

#include <vector>

namespace Dialogs {

ColorDialog::ColorDialog(Role role)
  : m_role(role)
{
    setOption(QColorDialog::DontUseNativeDialog, true);

    // Based on default system color picker on Ubuntu 20.04
    const std::vector<QColor> standardPalette = {
        "#ef2929", // Red
        "#cc0000",
        "#a40000",
        "#111111",
        "#111111",
        "#111111",
        "#fcaf3e", // Orange
        "#f57900",
        "#ce5c00",
        "#333333",
        "#333333",
        "#333333",
        "#fce94f", // Yellow
        "#edd400",
        "#c4a000",
        "#555555",
        "#555555",
        "#555555",
        "#8AE234", // Green
        "#73D216",
        "#4E9A06",
        "#777777",
        "#777777",
        "#777777",
        "#729FCF", // Blue
        "#3465A4",
        "#204A87",
        "#999999",
        "#999999",
        "#999999",
        "#AD7FA8", // Purple
        "#75507B",
        "#5C3566",
        Constants::MindMap::defaultBackgroundColor(),
        "#bbbbbb",
        "#bbbbbb",
        "#E9B96E", // Brown
        "#C17D11",
        "#8F5902",
        "#dddddd",
        "#dddddd",
        "#dddddd",
        "#888A85", // Grey
        "#555753",
        "#2E3436",
        "#ffffff",
        "#ffffff",
        "#ffffff",
    };

    // Based on https://lospec.com/palette-list/island-joy-16
    const std::vector<QColor> customPalette = {
        "#ffffff",
        "#393457",
        "#6df7c1",
        "#1e8875",
        "#11adc1",
        "#5bb361",
        "#606c81",
        "#a1e55a",
        "#f7e476",
        "#c92464",
        "#f99252",
        "#f48cb6",
        "#cb4d68",
        "#f7b69e",
        "#6a3771",
        "#9b9c82"
    };

    int index = 0;
    for (auto && color : standardPalette) {
        setStandardColor(index++, color);
    }

    index = 0;
    for (auto && color : customPalette) {
        setCustomColor(index++, color);
    }

    connect(this, &QColorDialog::colorSelected, [=](QColor color) {
        if (color.isValid()) {
            m_color = color;
        }
    });
}

const QColor & ColorDialog::color() const
{
    return m_color;
}

ColorDialog::~ColorDialog() = default;

} // namespace Dialogs
