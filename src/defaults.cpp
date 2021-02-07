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

#include "defaults.hpp"
#include "settings.hpp"

std::unique_ptr<Defaults> Defaults::m_instance;

Defaults::Defaults()
  : m_edgeArrowMode(Settings::loadEdgeArrowMode(Edge::ArrowMode::Single))
  , m_reversedEdgeDirection(Settings::loadReversedEdgeDirection(false))
{
}

Defaults & Defaults::instance()
{
    if (!Defaults::m_instance) {
        Defaults::m_instance = std::make_unique<Defaults>();
    }
    return *Defaults::m_instance;
}

Edge::ArrowMode Defaults::edgeArrowMode() const
{
    return m_edgeArrowMode;
}

void Defaults::setEdgeArrowMode(Edge::ArrowMode mode)
{
    if (m_edgeArrowMode != mode) {
        m_edgeArrowMode = mode;
        Settings::saveEdgeArrowMode(mode);
    }
}

bool Defaults::reversedEdgeDirection() const
{
    return m_reversedEdgeDirection;
}

void Defaults::setReversedEdgeDirection(bool reversedEdgeDirection)
{
    if (m_reversedEdgeDirection != reversedEdgeDirection) {
        m_reversedEdgeDirection = reversedEdgeDirection;
        Settings::saveReversedEdgeDirection(reversedEdgeDirection);
    }
}
