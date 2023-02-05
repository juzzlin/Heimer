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

#ifndef MOUSE_ACTION_HPP
#define MOUSE_ACTION_HPP

#include <QPointF>

#include "types.hpp"

class MouseAction
{
public:
    enum class Action
    {
        None,
        MoveNode,
        CreateOrConnectNode,
        RubberBand,
        Scroll
    };

    MouseAction();

    void clear();

    NodeP sourceNode() const;
    void setSourceNode(NodeP node, Action action);

    Action action() const;

    QPointF sourcePos() const;
    void setSourcePos(const QPointF & sourcePos);

    QPointF sourcePosOnNode() const;
    void setSourcePosOnNode(const QPointF & sourcePosOnNode);

    QPointF rubberBandOrigin() const;
    void setRubberBandOrigin(const QPointF & rubberBandOrigin);

    QPointF mappedPos() const;
    void setMappedPos(const QPointF & mappedPos);

    QPointF clickedScenePos() const;
    void setClickedScenePos(const QPointF & clickedScenePos);

private:
    NodeP m_sourceNode = nullptr;

    QPointF m_clickedScenePos;

    QPointF m_mappedPos;

    QPointF m_sourcePos;

    QPointF m_sourcePosOnNode;

    QPointF m_rubberBandOrigin;

    Action m_action = Action::None;
};

#endif // MOUSE_ACTION_HPP
