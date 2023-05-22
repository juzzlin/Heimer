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

#include "magic_zoom.hpp"

#include "constants.hpp"
#include "types.hpp"

#include "scene_items/node.hpp"

#include <cmath>

QRectF MagicZoom::calculateRectangleByItems(const ItemList & items, bool isForExport)
{
    NodeList nodes;
    for (auto && item : items) {
        if (const auto node = dynamic_cast<NodeP>(item); node) {
            nodes.push_back(node);
        }
    }

    return calculateRectangleByNodes(nodes, isForExport);
}

QRectF MagicZoom::calculateRectangleByNodes(const NodeList & nodes, bool isForExport)
{
    double nodeArea = 0;
    QRectF rect;
    for (auto && node : nodes) {
        const auto nodeRect = node->placementBoundingRect();
        rect = rect.united(nodeRect.translated(node->pos().x(), node->pos().y()));
        nodeArea += nodeRect.width() * nodeRect.height();
    }

    const int margin = 60;

    if (isForExport) {
        return rect.adjusted(-margin, -margin, margin, margin);
    }

    // This "don't ask" heuristics tries to calculate a "nice" zoom-to-fit based on the design
    // density and node count. For example, if we have just a single node we don't want it to
    // be super big and cover the whole screen.
    const double density = nodeArea / rect.width() / rect.height();
    const int avgNodeCount = static_cast<int>(nodeArea / Constants::Node::MIN_WIDTH / Constants::Node::MIN_HEIGHT);
    const double adjust = 3.0 * std::max(density * rect.width(), density * rect.height()) / pow(avgNodeCount, 1.5);
    return rect.adjusted(-adjust / 2, -adjust / 2, adjust / 2, adjust / 2).adjusted(-margin, -margin, margin, margin);
}
