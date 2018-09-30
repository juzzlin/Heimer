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

#ifndef EDGEBASE_HPP
#define EDGEBASE_HPP

#include <memory>

#include <QString>

class NodeBase;

class EdgeBase
{
public:

    EdgeBase(NodeBase & sourceNode, NodeBase & targetNode);

    virtual void setSourceNode(NodeBase & sourceNode);

    virtual void setTargetNode(NodeBase & targetNode);

    virtual NodeBase & sourceNodeBase() const;

    virtual NodeBase & targetNodeBase() const;

    QString text() const;

    virtual void setText(const QString & text);

    virtual ~EdgeBase() {}

    virtual double width() const;

    virtual void setWidth(double width);

private:

    NodeBase * m_sourceNode;

    NodeBase * m_targetNode;

    QString m_text;

    double m_width;
};

using EdgeBasePtr = std::shared_ptr<EdgeBase>;

#endif // EDGEBASE_HPP
