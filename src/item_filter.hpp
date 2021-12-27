// This file is part of Heimer.
// Copyright (C) 2021 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef ITEM_FILTER_HPP
#define ITEM_FILTER_HPP

#include <QPointF>

#include "types.hpp"

class QGraphicsScene;

class Node;
class NodeHandle;
class Edge;
class EdgeTextEdit;
class TextEdit;

namespace ItemFilter {

struct Result
{
    EdgeP edge = nullptr;

    EdgeTextEdit * edgeTextEdit = nullptr;

    NodeP node = nullptr;

    NodeHandle * nodeHandle = nullptr;

    TextEdit * nodeTextEdit = nullptr;

    bool success = false;
};

Result getFirstItemAtPosition(QGraphicsScene & scene, QPointF scenePos, double tolerance);

} // namespace ItemFilter

#endif // ITEM_FILTER_HPP
