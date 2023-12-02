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

#ifndef GRID_HPP
#define GRID_HPP

#include <QLineF>
#include <QPointF>
#include <QRectF>

#include <vector>

class Grid
{
public:
    Grid();

    using LineArray = std::vector<QLineF>;
    const LineArray & calculateLines(const QRectF & sceneRect) const;

    void setSize(int gridSize);

    int size() const;

    QPointF snapToGrid(QPointF in) const;

private:
    int m_size = 0;

    mutable LineArray m_lines;
};

#endif // GRID_HPP
