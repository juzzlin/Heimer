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

#ifndef NODE_MODEL_HPP
#define NODE_MODEL_HPP

#include <QColor>
#include <QPointF>
#include <QSizeF>
#include <QString>

struct NodeModel
{
    NodeModel(QColor color, QColor textColor)
      : color(color)
      , textColor(textColor)
    {
    }

    QColor color;

    int index = -1;

    size_t imageRef = 0;

    QPointF location;

    QSizeF size;

    QColor textColor;

    QString text;
};

#endif // NODE_MODEL_HPP
