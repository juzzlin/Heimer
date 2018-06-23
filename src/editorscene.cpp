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

#include "editorscene.hpp"
#include "edge.hpp"
#include "node.hpp"

#include "contrib/mclogger.hh"

#include <cmath>
#include <QGraphicsLineItem>

EditorScene::EditorScene()
{
    const int r = 10000;
    setSceneRect(-r, -r, r * 2, r * 2);

    const int WIDTH = 100;
    const auto pen = QPen(QBrush(QColor(255, 0, 0, 128)), WIDTH);

    auto leftLine = new QGraphicsLineItem(-r, -r, -r, r);
    leftLine->setPen(pen);
    addItem(leftLine);

    auto topLine = new QGraphicsLineItem(-r, -r, r, -r);
    topLine->setPen(pen);
    addItem(topLine);

    auto rightLine = new QGraphicsLineItem(r, -r, r, r);
    rightLine->setPen(pen);
    addItem(rightLine);

    auto bottomLine = new QGraphicsLineItem(-r, r, r, r);
    bottomLine->setPen(pen);
    addItem(bottomLine);
}

QRectF EditorScene::getNodeBoundingRectWithHeuristics() const
{
    float nodeArea = 0;
    QRectF rect;
    int nodes = 0;
    for (auto && item : items())
    {
        if (auto node = dynamic_cast<Node *>(item))
        {
            rect = rect.united(node->boundingRect().adjusted(node->pos().x(), node->pos().y(), node->pos().x(), node->pos().y()));
            nodeArea += node->boundingRect().width() * node->boundingRect().height();
            nodes++;
        }
    }

    // This "don't ask" heuristics tries to calculate a "nice" zoom-to-fit based on the design
    // density and node count. For example, if we have just a single node we don't want it to
    // be super big and cover the whole screen.
    const float coverage = std::pow(nodeArea / rect.width() / rect.height(), std::pow(nodes, 1.5f));
    const float adjust = 1.5f * std::max(coverage * rect.width(), coverage * rect.height());
    return rect.adjusted(-adjust / 2, -adjust / 2, adjust / 2, adjust / 2);
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

EditorScene::~EditorScene()
{
    // We don't want the scene to destroy the items as they are managed elsewhere
    for (auto item : items())
    {
        removeItem(item);
    }

    MCLogger().info() << "EditorScene deleted";
}
