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

#include "edgebase.hpp"
#include "nodebase.hpp"

EdgeBase::EdgeBase(NodeBase & sourceNode, NodeBase & targetNode)
    : m_sourceNode(&sourceNode)
    , m_targetNode(&targetNode)
{
}

void EdgeBase::setSourceNode(NodeBase & sourceNode)
{
    m_sourceNode = &sourceNode;
}

void EdgeBase::setTargetNode(NodeBase & targetNode)
{
    m_targetNode = &targetNode;
}

void EdgeBase::setText(const QString & text)
{
    m_text = text;
}

double EdgeBase::width() const
{
    return m_width;
}

void EdgeBase::setWidth(double width)
{
    m_width = width;
}

int EdgeBase::textSize() const
{
    return m_textSize;
}

void EdgeBase::setTextSize(int textSize)
{
    m_textSize = textSize;
}

NodeBase & EdgeBase::sourceNodeBase() const
{
    return *m_sourceNode;
}

NodeBase & EdgeBase::targetNodeBase() const
{
    return *m_targetNode;
}

QString EdgeBase::text() const
{
    return m_text;
}
