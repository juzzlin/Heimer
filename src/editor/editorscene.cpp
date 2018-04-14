// This file is part of Dementia.
// Copyright (C) 2018 Jussi Lind <jussi.lind@iki.fi>
//
// Dementia is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dementia is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dementia. If not, see <http://www.gnu.org/licenses/>.

#include "editorscene.hpp"
#include "edge.hpp"
#include "node.hpp"

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

QRectF EditorScene::getNodeBoundingRect() const
{
    QRectF rect;
    for (auto && item : items())
    {
        if (auto node = dynamic_cast<Node *>(item))
        {
            rect = rect.united(node->boundingRect().adjusted(node->pos().x(), node->pos().y(), node->pos().x(), node->pos().y()));
        }
    }

    const int minSize = 600;

    if (rect.width() < minSize)
    {
        rect = rect.adjusted(-minSize / 2, 0, minSize / 2, 0);
    }

    if (rect.height() < minSize)
    {
        rect = rect.adjusted(0, -minSize / 2, 0, minSize / 2);
    }

    return rect;
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
}
