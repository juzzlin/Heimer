// This file is part of Heimer.
// Copyright (C) 2021 Jussi Lind <jussi.lind@iki.fi>
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

#include "copy_context.hpp"

#include "node.hpp"

CopyContext::CopyContext()
{
}

void CopyContext::clear()
{
    m_copiedNodes.clear();
    m_copyReferencePoint = {};
}

size_t CopyContext::copyStackSize() const
{
    return m_copiedNodes.size();
}

void CopyContext::push(Node & node)
{
    m_copiedNodes.push_back(std::make_shared<Node>(node));
    m_copyReferencePoint *= static_cast<qreal>(m_copiedNodes.size() - 1);
    m_copyReferencePoint += node.pos();
    m_copyReferencePoint /= static_cast<qreal>(m_copiedNodes.size());
}

std::vector<std::shared_ptr<Node>> CopyContext::copiedNodes() const
{
    return m_copiedNodes;
}

QPointF CopyContext::copyReferencePoint() const
{
    return m_copyReferencePoint;
}
