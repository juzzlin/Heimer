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

#include "magiczoom.hpp"

#include "editorscene.hpp"
#include "node.hpp"

#include <cmath>

QRectF MagicZoom::calculateRectangle(const EditorScene & scene, bool isForExport)
{
    double nodeArea = 0;
    QRectF rect;
    int nodes = 0;
    for (auto && item : scene.items())
    {
        if (auto node = dynamic_cast<Node *>(item))
        {
            const auto nodeRect = node->placementBoundingRect();
            rect = rect.united(nodeRect.translated(node->pos().x(), node->pos().y()));
            nodeArea += nodeRect.width() * nodeRect.height();
            nodes++;
        }
    }

    const int margin = 60;

    if (isForExport)
    {
        return rect.adjusted(-margin, -margin, margin, margin);
    }

    // This "don't ask" heuristics tries to calculate a "nice" zoom-to-fit based on the design
    // density and node count. For example, if we have just a single node we don't want it to
    // be super big and cover the whole screen.
    const double density = nodeArea / rect.width() / rect.height();
    const double adjust = 3.0 * std::max(density * rect.width(), density * rect.height()) / pow(nodes, 1.5);
    return rect.adjusted(-adjust / 2, -adjust / 2, adjust / 2, adjust / 2).adjusted(-margin, -margin, margin, margin);
}
