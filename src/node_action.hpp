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

#ifndef NODE_ACTION_HPP
#define NODE_ACTION_HPP

#include <QColor>

struct NodeAction
{
    enum class Type
    {
        None,
        Delete,
        SetColor
    };

    NodeAction()
    {
    }

    NodeAction(Type type)
      : type(type)
    {
    }

    NodeAction(QColor color)
      : type(Type::SetColor)
      , color(color)
    {
    }

    Type type = Type::None;

    QColor color;
};

#endif // NODE_ACTION_HPP
