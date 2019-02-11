// This file is part of Heimer.
// Copyright (C) 2018 Jussi Lind <jussi.lind@iki.fi>
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

#include "editor_scene.hpp"

#include "constants.hpp"
#include "edge.hpp"
#include "magic_zoom.hpp"
#include "node.hpp"

#include "simple_logger.hpp"

#include <cmath>
#include <QGraphicsLineItem>

EditorScene::EditorScene()
{
    initialize();
}

void EditorScene::initialize()
{
    removeItems();

    const auto r = Constants::Scene::RADIUS;
    setSceneRect(-r, -r, r * 2, r * 2);

    const auto pen = QPen(QBrush(Constants::Scene::BARRIER_COLOR), Constants::Scene::BARRIER_WIDTH);

    auto leftLine = new QGraphicsLineItem(-r, -r, -r, r);
    leftLine->setPen(pen);
    addItem(leftLine);
    m_ownItems.push_back(ItemPtr(leftLine));

    auto topLine = new QGraphicsLineItem(-r, -r, r, -r);
    topLine->setPen(pen);
    addItem(topLine);
    m_ownItems.push_back(ItemPtr(topLine));

    auto rightLine = new QGraphicsLineItem(r, -r, r, r);
    rightLine->setPen(pen);
    addItem(rightLine);
    m_ownItems.push_back(ItemPtr(rightLine));

    auto bottomLine = new QGraphicsLineItem(-r, r, r, r);
    bottomLine->setPen(pen);
    addItem(bottomLine);
    m_ownItems.push_back(ItemPtr(bottomLine));
}

QRectF EditorScene::zoomToFit(bool isForExport) const
{
    return MagicZoom::calculateRectangle(*this, isForExport);
}

bool EditorScene::hasEdge(Node & node0, Node & node1)
{
    for (auto && item : items())
    {
        if (auto edge = dynamic_cast<Edge *>(item))
        {
            if (edge->sourceNode().index() == node0.index() &&
                edge->targetNode().index() == node1.index())
            {
                return true;
            }
        }
    }
    return false;
}

void EditorScene::removeItems()
{
    // We don't want the scene to destroy the items as they are managed elsewhere
    for (auto item : items())
    {
        removeItem(item);
    }

    // This will destroy own items that also were taken out of the scene
    m_ownItems.clear();
}

EditorScene::~EditorScene()
{
    removeItems();

    juzzlin::L().debug() << "EditorScene deleted";
}
