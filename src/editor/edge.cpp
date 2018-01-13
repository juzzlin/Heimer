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

#include "edge.hpp"
#include "node.hpp"

#include <QGraphicsDropShadowEffect>

Edge::Edge(Node & sourceNode, Node & targetNode)
    : m_sourceNode(sourceNode)
    , m_targetNode(targetNode)
{
    QGraphicsDropShadowEffect * shadow = new QGraphicsDropShadowEffect();
    shadow->setOffset(QPointF(3, 3));
    shadow->setBlurRadius(5);
    setGraphicsEffect(shadow);
}

void Edge::updateLine()
{
    setLine(m_sourceNode.pos().x(), m_sourceNode.pos().y(), m_targetNode.pos().x(), m_targetNode.pos().y());
}
