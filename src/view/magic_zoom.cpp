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

#include "../common/types.hpp"

#include "scene_items/node.hpp"

#include <cmath>

QRectF MagicZoom::calculateRectangleByItems(const ItemList & items, bool isForExport)
{
    NodeList nodes;
    std::transform(items.begin(), items.end(), std::back_inserter(nodes), [](const auto & item) { return dynamic_cast<NodeP>(item); });
    nodes.erase(std::remove(nodes.begin(), nodes.end(), nullptr), nodes.end());
    return calculateRectangleByNodes(nodes, isForExport);
}

static QRectF calculateZoomRectBasedOnItemDensity(QRectF unitedItemRect, double averageItemArea, double totalArea)
{
    const double amplification = 3.0;
    const double nonlinearity = 1.5;
    const double itemDensity = totalArea / unitedItemRect.width() / unitedItemRect.height();
    const double averageItemCount = totalArea / averageItemArea;
    const double maxItemRectDimension = std::max(unitedItemRect.width(), unitedItemRect.height());
    const double expansionBasedOnDensity = amplification * itemDensity * maxItemRectDimension / pow(averageItemCount, nonlinearity);

    const int margin = 60;
    return unitedItemRect.adjusted(-expansionBasedOnDensity / 2, -expansionBasedOnDensity / 2, expansionBasedOnDensity / 2, expansionBasedOnDensity / 2).adjusted(-margin, -margin, margin, margin);
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

    return calculateZoomRectBasedOnItemDensity(unitedRect, avgEdgeArea, totalArea);
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

    return calculateZoomRectBasedOnItemDensity(unitedRect, avgNodeArea, totalArea);
}
