// This file is part of Heimer.
// Copyright (C) 2019 Jussi Lind <jussi.lind@iki.fi>
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

#include "grid.hpp"

Grid::Grid()
{
}

void Grid::setSize(int size)
{
    m_size = size;
}

QPointF Grid::snapToGrid(QPointF in) const
{
    if (!m_size)
    {
        return in;
    }

    return {
        static_cast<double>(static_cast<int>(in.x() / m_size) * m_size),
        static_cast<double>(static_cast<int>(in.y() / m_size) * m_size)};
}
