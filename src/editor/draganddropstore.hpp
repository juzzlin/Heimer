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

#ifndef DRAGANDDROPSTORE_HPP
#define DRAGANDDROPSTORE_HPP

#include <QPointF>

class Node;
class NodeHandle;

class DragAndDropStore
{
public:

    DragAndDropStore();

    void clear();

    void setDragAndDropNode(Node * tnode);

    Node * dragAndDropNode() const;
    QPointF dragAndDropSourcePos() const;

    void setDragAndDropSourcePos(QPointF pos);

    NodeHandle *dragAndDropNodeHandle() const;
    void setDragAndDropNodeHandle(NodeHandle * dragAndDropNodeHandle);

private:

    Node * m_dragAndDropNode = nullptr;

    NodeHandle * m_dragAndDropNodeHandle = nullptr;

    QPointF m_dragAndDropSourcePos;
};

#endif // DRAGANDDROPSTORE_HPP
