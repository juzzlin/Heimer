// This file is part of Dementia.
// Copyright (C) 2018 Jussi Lind <jussi.lind@iki.fi>
//
// Dust Racing 2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dust Racing 2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dust Racing 2D. If not, see <http://www.gnu.org/licenses/>.

#include "draganddropstore.hpp"

#include "node.hpp"

DragAndDropStore::DragAndDropStore()
{
}

void DragAndDropStore::clear()
{
    m_dragAndDropNode = nullptr;
    m_dragAndDropNodeHandle = nullptr;
}

void DragAndDropStore::setDragAndDropNode(Node * tnode)
{
    m_dragAndDropNode = tnode;
}

Node * DragAndDropStore::dragAndDropNode() const
{
    return m_dragAndDropNode;
}

void DragAndDropStore::setDragAndDropSourcePos(QPointF pos)
{
    m_dragAndDropSourcePos = pos;
}

NodeHandle *DragAndDropStore::dragAndDropNodeHandle() const
{
    return m_dragAndDropNodeHandle;
}

void DragAndDropStore::setDragAndDropNodeHandle(NodeHandle * dragAndDropNodeHandle)
{
    m_dragAndDropNodeHandle = dragAndDropNodeHandle;
}

QPointF DragAndDropStore::dragAndDropSourcePos() const
{
    return m_dragAndDropSourcePos;
}
