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

#include "scene_items/edge.hpp"
#include "scene_items/edge_text_edit.hpp"
#include "scene_items/node.hpp"
#include "scene_items/node_handle.hpp"

#include <QGraphicsScene>

namespace ItemFilter {

ItemAtPosition getFirstItemAtPosition(QGraphicsScene & scene, QPointF scenePos, double tolerance)
{
    ItemAtPosition itemAtPosition;

    const QRectF clickRect(scenePos.x() - tolerance, scenePos.y() - tolerance, tolerance * 2, tolerance * 2);

    if (const auto itemsAtPosition = scene.items(clickRect, Qt::IntersectsItemShape, Qt::DescendingOrder); !itemsAtPosition.empty()) {
        const auto firstItem = *itemsAtPosition.begin();
        if (const auto edge = dynamic_cast<EdgeP>(firstItem); edge) {
            itemAtPosition.itemOptional = edge;
        } else if (const auto nodeHandle = dynamic_cast<SceneItems::NodeHandle *>(firstItem); nodeHandle) {
            itemAtPosition.itemOptional = nodeHandle;
        } else if (const auto node = dynamic_cast<NodeP>(firstItem); node) {
            itemAtPosition.itemOptional = node;
        } else if (const auto edgeTextEdit = dynamic_cast<SceneItems::EdgeTextEdit *>(firstItem); edgeTextEdit) {
            itemAtPosition.itemOptional = edgeTextEdit;
        }
    }

    return itemAtPosition;
}

} // namespace ItemFilter
