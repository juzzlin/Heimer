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

#include <cmath>

Grid::Grid()
{
}

Grid::LineArray Grid::calculateLines(const QRectF & rect) const
{
    LineArray lines;

    if (m_size) {
        const qreal left = static_cast<int>(rect.left()) - static_cast<int>(rect.left()) % m_size;
        const qreal top = static_cast<int>(rect.top()) - static_cast<int>(rect.top()) % m_size;

        auto x = left;
        while (x < rect.right()) {
            lines.append({ x, rect.top(), x, rect.bottom() });
            x += m_size;
        }

        auto y = top;
        while (y < rect.bottom()) {
            lines.append({ rect.left(), y, rect.right(), y });
            y += m_size;
        }
    }

    return lines;
}

void Grid::setSize(int size)
{
    m_size = size;
}

int Grid::size() const
{
    return m_size;
}

QPointF Grid::snapToGrid(QPointF in) const
{
    if (!m_size) {
        return in;
    }

    return {
        static_cast<double>(std::round(in.x() / m_size) * m_size),
        static_cast<double>(std::round(in.y() / m_size) * m_size)
    };
}
