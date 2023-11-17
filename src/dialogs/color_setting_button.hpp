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

#ifndef COLOR_SETTING_BUTTON_HPP
#define COLOR_SETTING_BUTTON_HPP

#include "color_dialog.hpp"
#include <QColor>
#include <QPushButton>

#include "types.hpp"

namespace Core {
class SettingsProxy;
}

namespace Dialogs {

class ColorSettingButton : public QPushButton
{
    Q_OBJECT

public:
    explicit ColorSettingButton(QString text, ColorDialog::Role role, QWidget * parent = nullptr);

    const QColor & selectedColor() const;

    void resetToDefault();

    void setColor(QColor color);

signals:
    void colorSelected(const QColor & color);

private:
    void apply(QColor color);

    ColorDialog::Role m_role;

    QColor m_selectedColor;

    SettingsProxyS m_settingsProxy;
};

} // namespace Dialogs

#endif // COLOR_SETTING_BUTTON_HPP
