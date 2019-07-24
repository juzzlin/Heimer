// This file is part of Heimer.
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

#include "mouse_action.hpp"

#include "node.hpp"

MouseAction::MouseAction()
{
}

MouseAction::Action MouseAction::action() const
{
    return m_action;
}

void MouseAction::clear()
{
    m_sourceNode = nullptr;
    m_sourcePos = QPointF();
    m_sourcePosOnNode = QPointF();
    m_action = Action::None;
}

void MouseAction::setSourceNode(Node * node, MouseAction::Action action)
{
    m_sourceNode = node;
    m_action = action;
}

Node * MouseAction::sourceNode() const
{
    return m_sourceNode;
}

void MouseAction::setSourcePos(const QPointF & sourcePos)
{
    m_sourcePos = sourcePos;
}

void MouseAction::setSourcePosOnNode(const QPointF & sourcePos)
{
    m_sourcePosOnNode = sourcePos;
}

QPointF MouseAction::rubberBandOrigin() const
{
    return m_rubberBandOrigin;
}

void MouseAction::setRubberBandOrigin(const QPointF & rubberBandOrigin)
{
    m_rubberBandOrigin = rubberBandOrigin;
    m_action = Action::RubberBand;
}

QPointF MouseAction::mappedPos() const
{
    return m_mappedPos;
}

void MouseAction::setMappedPos(const QPointF & mappedPos)
{
    m_mappedPos = mappedPos;
}

QPointF MouseAction::clickedScenePos() const
{
    return m_clickedScenePos;
}

void MouseAction::setClickedScenePos(const QPointF & clickedScenePos)
{
    m_clickedScenePos = clickedScenePos;
}

QPointF MouseAction::sourcePos() const
{
    return m_sourcePos;
}

QPointF MouseAction::sourcePosOnNode() const
{
    return m_sourcePosOnNode;
}
