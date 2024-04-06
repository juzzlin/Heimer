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

#include "../common/constants.hpp"
#include "../common/types.hpp"

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

static QRectF adjustRect(QRectF unitedRect, double avgItemArea, double totalArea)
{
    const int margin = 60;

    // This "don't ask" heuristics tries to calculate a "nice" zoom-to-fit based on the design
    // density and node count. For example, if we have just a single node we don't want it to
    // be super big and cover the whole screen.
    const double density = totalArea / unitedRect.width() / unitedRect.height();
    const int avgItemCount = static_cast<int>(totalArea / avgItemArea);
    const double adjust = 3.0 * std::max(density * unitedRect.width(), density * unitedRect.height()) / pow(avgItemCount, 1.5);
    return unitedRect.adjusted(-adjust / 2, -adjust / 2, adjust / 2, adjust / 2).adjusted(-margin, -margin, margin, margin);
}

QRectF MagicZoom::calculateRectangleByEdges(const EdgeList & edges)
{
    if (edges.empty()) {
        return {};
    }

    double totalArea = 0;
    double avgEdgeArea = 0;
    QRectF unitedRect;
    for (auto && edge : edges) {
        const auto edgeRect = edge->translatedLabelBoundingRect();
        unitedRect = unitedRect.united(edgeRect);
        const auto edgeArea = edgeRect.width() * edgeRect.height();
        totalArea += edgeArea;
        avgEdgeArea += edgeArea;
    }
    avgEdgeArea /= static_cast<double>(edges.size());

    return adjustRect(unitedRect, avgEdgeArea, totalArea);
}

QRectF MagicZoom::calculateRectangleByNodes(const NodeList & nodes, bool isForExport)
{
    if (nodes.empty()) {
        return {};
    }

    double totalArea = 0;
    double avgNodeArea = 0;
    QRectF unitedRect;
    for (auto && node : nodes) {
        const auto nodeRect = node->placementBoundingRect();
        unitedRect = unitedRect.united(nodeRect.translated(node->pos().x(), node->pos().y()));
        const auto nodeArea = nodeRect.width() * nodeRect.height();
        totalArea += nodeArea;
        avgNodeArea += nodeArea;
    }
    avgNodeArea /= static_cast<double>(nodes.size());

    if (isForExport) {
        const int exportMargin = 60;
        return unitedRect.adjusted(-exportMargin, -exportMargin, exportMargin, exportMargin);
    }

    return adjustRect(unitedRect, avgNodeArea, totalArea);
}
