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

#include "item_filter.hpp"

#include "edge.hpp"
#include "edge_text_edit.hpp"
#include "node.hpp"
#include "node_handle.hpp"
#include "text_edit.hpp"
#include "types.hpp"

#include <QGraphicsScene>

namespace ItemFilter {

Result getFirstItemAtPosition(QGraphicsScene & scene, QPointF scenePos, double tolerance)
{
    Result result;
    const QRectF clickRect(scenePos.x() - tolerance, scenePos.y() - tolerance, tolerance * 2, tolerance * 2);

    // Fetch all items at the location
    if (const auto items = scene.items(clickRect, Qt::IntersectsItemShape, Qt::DescendingOrder); items.size()) {
        const auto item = *items.begin();
        if (const auto edge = dynamic_cast<EdgeP>(item)) {
            result.edge = edge;
            result.success = true;
        } else if (const auto nodeHandle = dynamic_cast<NodeHandle *>(item)) {
            result.nodeHandle = nodeHandle;
            result.success = true;
        } else if (const auto node = dynamic_cast<NodeP>(item)) {
            result.node = node;
            result.success = true;
        } else if (const auto edgeTextEdit = dynamic_cast<EdgeTextEdit *>(item)) {
            result.edgeTextEdit = edgeTextEdit;
            result.success = true;
        } else if (const auto nodeTextEdit = dynamic_cast<TextEdit *>(item)) {
            result.nodeTextEdit = nodeTextEdit;
            result.success = true;
        }
    }

    return result;
}

} // namespace ItemFilter
