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

#include "mindmapdata.hpp"

#include "node.hpp"

#include <memory>

MindMapData::MindMapData(QString name)
    : MindMapDataBase(name)
{}

MindMapData::MindMapData(const MindMapData & other)
    : MindMapDataBase(other)
    , m_fileName(other.m_fileName)
{
    copyGraph(other);
}

void MindMapData::copyGraph(const MindMapData & other)
{
    m_graph.clear();

    // TODO
    //for (auto iter = other.m_graph.cbegin(); iter != other.m_graph.cend(); iter++)
    //{
    //    auto node = std::dynamic_pointer_cast<Node>(*iter);
    //    auto newNode = new Node(*(node.get()));
    //
    //    m_graph.add(NodeBasePtr(newNode));
    //}
}

QString MindMapData::fileName() const
{
    return m_fileName;
}

void MindMapData::setFileName(QString newFileName)
{
    m_fileName = newFileName;
}

Graph & MindMapData::graph()
{
    return m_graph;
}

const Graph & MindMapData::graph() const
{
    return m_graph;
}

MindMapData::~MindMapData()
{
}
