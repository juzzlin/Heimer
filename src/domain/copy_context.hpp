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

#ifndef COPY_CONTEXT_HPP
#define COPY_CONTEXT_HPP

#include <vector>

#include <QPointF>

#include "../common/types.hpp"

class CopyContext
{
public:
    CopyContext();

    void clear();

    size_t copyStackSize() const;

    using NodePVector = std::vector<NodeP>;
    void push(NodePVector nodes, GraphCR graph);
    void push(NodeCR node);

    //! Store node indices into a separate metadata structure
    //! because Edge instances currently need references to
    //! the actual node instances and we are working with copies
    //! here..
    struct EdgeMetadata
    {
        EdgeMetadata(EdgeS edge, int sourceNodeIndex, int targetNodeIndex)
          : edge(edge)
          , sourceNodeIndex(sourceNodeIndex)
          , targetNodeIndex(targetNodeIndex)
        {
        }

        EdgeS edge;

        int sourceNodeIndex = -1;

        int targetNodeIndex = -1;
    };

    using EdgeVector = std::vector<EdgeMetadata>;
    using NodeVector = std::vector<NodeS>;

    struct CopiedData
    {
        QPointF copyReferencePoint;

        EdgeVector edges;

        NodeVector nodes;
    };

    CopiedData copiedData() const;

private:
    CopyContext(const CopyContext & other) = delete;
    CopyContext & operator=(const CopyContext & other) = delete;

    void pushEdges(NodePVector nodes, GraphCR graph);

    CopiedData m_copiedData;
};

#endif // COPY_CONTEXT_HPP
