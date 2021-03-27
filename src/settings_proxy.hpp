// This file is part of Heimer.
// Copyright (C) 2020 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef SETTINGS_PROXY_HPP
#define SETTINGS_PROXY_HPP

#include "edge.hpp"

#include <memory>

class SettingsProxy
{
public:
    SettingsProxy();

    static SettingsProxy & instance();

    Edge::ArrowMode edgeArrowMode() const;

    void setEdgeArrowMode(Edge::ArrowMode mode);

    bool reversedEdgeDirection() const;

    void setReversedEdgeDirection(bool reversedEdgeDirection);

    bool selectNodeGroupByIntersection() const;

    void setSelectNodeGroupByIntersection(bool selectNodeGroupByIntersection);

private:
    Edge::ArrowMode m_edgeArrowMode;

    bool m_reversedEdgeDirection = false;

    bool m_selectNodeGroupByIntersection = false;

    static std::unique_ptr<SettingsProxy> m_instance;
};

#endif // SETTINGS_PROXY_HPP
