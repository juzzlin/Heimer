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

#include "graph.hpp"
#include "node.hpp"

#include "simple_logger.hpp"

CopyContext::CopyContext()
{
}

void CopyContext::clear()
{
    m_copiedData = {};
}

size_t CopyContext::copyStackSize() const
{
    return m_copiedData.nodes.size();
}

void CopyContext::pushEdges(NodePVector nodes, GraphCR graph)
{
    // Test all nodes against each other
    for (size_t i = 0; i < nodes.size(); i++) {
        for (size_t j = i + 1; j < nodes.size(); j++) {
            // Check if the nodes are directly connected
            int i0 = nodes.at(i)->index();
            int i1 = nodes.at(j)->index();
            if (const auto edge = graph.getEdge(i0, i1)) {
                m_copiedData.edges.push_back({ std::make_shared<Edge>(*edge), i0, i1 });
            }
            // Same as above, but to the other direction
            i0 = nodes.at(j)->index();
            i1 = nodes.at(i)->index();
            if (const auto edge = graph.getEdge(i0, i1)) {
                m_copiedData.edges.push_back({ std::make_shared<Edge>(*edge), i0, i1 });
            }
        }
    }
}

void CopyContext::push(NodePVector nodes, GraphCR graph)
{
    pushEdges(nodes, graph);

    for (auto && node : nodes) {
        push(*node);
    }

    juzzlin::L().debug() << m_copiedData.edges.size() << " edge(s) in copy stack";
    juzzlin::L().debug() << m_copiedData.nodes.size() << " node(s) in copy stack";
}

void CopyContext::push(NodeCR node)
{
    m_copiedData.nodes.push_back(std::make_shared<Node>(node));
    m_copiedData.copyReferencePoint *= static_cast<qreal>(m_copiedData.nodes.size() - 1);
    m_copiedData.copyReferencePoint += node.pos();
    m_copiedData.copyReferencePoint /= static_cast<qreal>(m_copiedData.nodes.size());
}

CopyContext::CopiedData CopyContext::copiedData() const
{
    return m_copiedData;
}
