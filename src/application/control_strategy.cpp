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

#include "control_strategy.hpp"

#include "service_container.hpp"
#include "settings_proxy.hpp"

#include <QMouseEvent>

ControlStrategy::ControlStrategy()
{
}

bool ControlStrategy::isModifierPressed(const QMouseEvent & event) const
{
    return event.modifiers().testFlag(Qt::ControlModifier) || event.modifiers().testFlag(Qt::ShiftModifier);
}

bool ControlStrategy::backgroundDragInitiated(const QMouseEvent & event) const
{
    const bool invertedControls = SC::instance().settingsProxy()->invertedControls();

    return (invertedControls == isModifierPressed(event)) && event.button() == Qt::LeftButton;
}

bool ControlStrategy::rubberBandInitiated(const QMouseEvent & event) const
{
    const bool invertedControls = SC::instance().settingsProxy()->invertedControls();

    return ((!invertedControls ^ !isModifierPressed(event)) && (event.button() == Qt::LeftButton)) || event.button() == Qt::MiddleButton;
}

bool ControlStrategy::primaryButtonClicked(const QMouseEvent & event) const
{
    return event.button() == Qt::LeftButton;
}

bool ControlStrategy::secondaryButtonClicked(const QMouseEvent & event) const
{
    return event.button() == Qt::RightButton;
}

QString ControlStrategy::mirrorLayoutHorizontallyShortcut() const
{
    return "Ctrl+Shift+H";
}

QString ControlStrategy::mirrorLayoutVerticallyShortcut() const
{
    return "Ctrl+Shift+V";
}

ControlStrategy::~ControlStrategy() = default;
