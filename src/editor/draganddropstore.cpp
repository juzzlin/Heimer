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

DragAndDropStore::Action DragAndDropStore::action() const
{
    return m_action;
}

void DragAndDropStore::clear()
{
    m_sourceNode = nullptr;
    m_sourcePos = QPointF();
    m_action = Action::None;
}

void DragAndDropStore::setSourceNode(Node * node, DragAndDropStore::Action action)
{
    m_sourceNode = node;
    m_action = action;
}

Node * DragAndDropStore::sourceNode() const
{
    return m_sourceNode;
}

void DragAndDropStore::setSourcePos(QPointF pos)
{
    m_sourcePos = pos;
}

QPointF DragAndDropStore::sourcePos() const
{
    return m_sourcePos;
}
