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

#ifndef CONTROL_STRATEGY_HPP
#define CONTROL_STRATEGY_HPP

class QMouseEvent;
class QString;

class ControlStrategy
{
public:
    ControlStrategy();

    virtual ~ControlStrategy();

    bool backgroundDragInitiated(const QMouseEvent & event) const;

    bool rubberBandInitiated(const QMouseEvent & event) const;

    bool primaryButtonClicked(const QMouseEvent & event) const;

    bool secondaryButtonClicked(const QMouseEvent & event) const;

    QString mirrorLayoutHorizontallyShortcut() const;

    QString mirrorLayoutVerticallyShortcut() const;

private:
    bool isModifierPressed(const QMouseEvent & event) const;
};

#endif // CONTROL_STRATEGY_HPP
