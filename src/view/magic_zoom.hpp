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

#ifndef MAGIC_ZOOM_HPP
#define MAGIC_ZOOM_HPP

#include <QList>
#include <QRectF>

#include <vector>

#include "../common/types.hpp"

class QGraphicsItem;

namespace MagicZoom {

using ItemList = QList<QGraphicsItem *>;
QRectF calculateRectangleByItems(const ItemList & items, bool isForExport = false);

using EdgeList = std::vector<EdgeP>;
QRectF calculateRectangleByEdges(const EdgeList & edges);

using NodeList = std::vector<NodeP>;
QRectF calculateRectangleByNodes(const NodeList & nodes, bool isForExport = false);

} // namespace MagicZoom

#endif // MAGIC_ZOOM_HPP
