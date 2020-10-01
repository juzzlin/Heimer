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

#include "mind_map_data.hpp"

#include "node.hpp"

#include <memory>

ImageManager MindMapData::m_imageManager {};

MindMapData::MindMapData(QString name)
  : MindMapDataBase(name)
{
}

MindMapData::MindMapData(const MindMapData & other)
  : MindMapDataBase(other)
  , m_fileName(other.m_fileName)
  , m_version(other.m_version)
  , m_backgroundColor(other.m_backgroundColor)
  , m_edgeColor(other.m_edgeColor)
  , m_gridColor(other.m_gridColor)
  , m_edgeWidth(other.m_edgeWidth)
  , m_textSize(other.m_textSize)
  , m_cornerRadius(other.m_cornerRadius)
{
    copyGraph(other);
}

void MindMapData::copyGraph(const MindMapData & other)
{
    m_graph.clear();

    // Use copy constructor for nodes
    for (auto && node : other.m_graph.getNodes()) {
        m_graph.addNode(std::make_shared<Node>(*node));
    }

    // Create new edges
    for (auto && otherEdge : other.m_graph.getEdges()) {
        auto sourceNode = m_graph.getNode(otherEdge->sourceNode().index());
        auto targetNode = m_graph.getNode(otherEdge->targetNode().index());
        auto edge = std::make_shared<Edge>(*sourceNode, *targetNode);
        edge->setArrowMode(otherEdge->arrowMode());
        edge->setText(otherEdge->text());
        edge->setReversed(otherEdge->reversed());
        m_graph.addEdge(edge);
    }
}

double MindMapData::aspectRatio() const
{
    return m_aspectRatio;
}

void MindMapData::setAspectRatio(double aspectRatio)
{
    m_aspectRatio = aspectRatio;
}

QColor MindMapData::backgroundColor() const
{
    return m_backgroundColor;
}

void MindMapData::setBackgroundColor(const QColor & backgroundColor)
{
    m_backgroundColor = backgroundColor;
}

int MindMapData::cornerRadius() const
{
    return m_cornerRadius;
}

void MindMapData::setCornerRadius(int cornerRadius)
{
    m_cornerRadius = cornerRadius;

    for (auto && node : m_graph.getNodes()) {
        node->setCornerRadius(cornerRadius);
    }
}

QColor MindMapData::edgeColor() const
{
    return m_edgeColor;
}

void MindMapData::setEdgeColor(const QColor & edgeColor)
{
    m_edgeColor = edgeColor;

    for (auto && edge : m_graph.getEdges()) {
        edge->setColor(edgeColor);
    }
}

QColor MindMapData::gridColor() const
{
    return m_gridColor;
}

void MindMapData::setGridColor(const QColor & gridColor)
{
    m_gridColor = gridColor;
}

double MindMapData::edgeWidth() const
{
    return m_edgeWidth;
}

void MindMapData::setEdgeWidth(double width)
{
    m_edgeWidth = width;

    for (auto && edge : m_graph.getEdges()) {
        edge->setWidth(width);
    }
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

ImageManager & MindMapData::imageManager()
{
    return m_imageManager;
}

const ImageManager & MindMapData::imageManager() const
{
    return m_imageManager;
}

double MindMapData::minEdgeLength() const
{
    return m_minEdgeLength;
}

void MindMapData::setMinEdgeLength(double minEdgeLength)
{
    m_minEdgeLength = minEdgeLength;
}

int MindMapData::textSize() const
{
    return m_textSize;
}

void MindMapData::setTextSize(int textSize)
{
    m_textSize = textSize;

    for (auto && edge : m_graph.getEdges()) {
        edge->setTextSize(textSize);
    }

    for (auto && node : m_graph.getNodes()) {
        node->setTextSize(textSize);
    }
}

QString MindMapData::version() const
{
    return m_version;
}

void MindMapData::setVersion(const QString & version)
{
    m_version = version;
}

MindMapData::~MindMapData() = default;
