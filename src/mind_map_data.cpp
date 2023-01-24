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

#include "grid.hpp"
#include "node.hpp"
#include "settings_proxy.hpp"
#include "shadow_effect_params.hpp"

#include <memory>

MindMapData::MindMapData(QString name)
  : MindMapDataBase(name)
  , m_backgroundColor(SettingsProxy::instance().backgroundColor())
  , m_arrowSize(SettingsProxy::instance().arrowSize())
  , m_edgeColor(SettingsProxy::instance().edgeColor())
  , m_gridColor(SettingsProxy::instance().gridColor())
  , m_edgeWidth(SettingsProxy::instance().edgeWidth())
{
}

MindMapData::MindMapData(const MindMapData & other)
  : MindMapDataBase(other)
  , m_fileName(other.m_fileName)
  , m_version(other.m_version)
  , m_backgroundColor(other.m_backgroundColor)
  , m_arrowSize(other.m_arrowSize)
  , m_edgeColor(other.m_edgeColor)
  , m_gridColor(other.m_gridColor)
  , m_edgeWidth(other.m_edgeWidth)
  , m_font(other.m_font)
  , m_textSize(other.m_textSize)
  , m_cornerRadius(other.m_cornerRadius)
  , m_imageManager(other.m_imageManager)
{
    copyGraph(other);
}

void MindMapData::copyGraph(const MindMapData & other)
{
    m_graph.clear();

    // Use copy constructor for nodes
    for (auto && node : other.m_graph.getNodes()) {
        m_graph.addNode(std::make_unique<Node>(*node));
    }

    // Use copy constructor for edges
    for (auto && otherEdge : other.m_graph.getEdges()) {
        m_graph.addEdge(std::make_unique<Edge>(*otherEdge, m_graph));
    }
}

void MindMapData::applyGrid(const Grid & grid)
{
    for (auto && node : m_graph.getNodes()) {
        node->setLocation(grid.snapToGrid(node->location()));
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

double MindMapData::arrowSize() const
{
    return m_arrowSize;
}

void MindMapData::setArrowSize(double arrowSize)
{
    m_arrowSize = arrowSize;

    for (auto && edge : m_graph.getEdges()) {
        edge->setArrowSize(arrowSize);
    }
}

double MindMapData::edgeWidth() const
{
    return m_edgeWidth;
}

void MindMapData::setEdgeWidth(double edgeWidth)
{
    m_edgeWidth = edgeWidth;

    for (auto && edge : m_graph.getEdges()) {
        edge->setEdgeWidth(edgeWidth);
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

GraphR MindMapData::graph()
{
    return m_graph;
}

GraphCR MindMapData::graph() const
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

QFont MindMapData::font() const
{
    return m_font;
}

void MindMapData::changeFont(QFont font)
{
    m_font = font;

    for (auto && edge : m_graph.getEdges()) {
        edge->changeFont(font);
    }

    for (auto && node : m_graph.getNodes()) {
        node->changeFont(font);
    }
}

void MindMapData::setShadowEffect(const ShadowEffectParams & params)
{
    for (auto && node : m_graph.getNodes()) {
        node->setShadowEffect(params);
    }

    for (auto && edge : m_graph.getEdges()) {
        edge->setShadowEffect(params);
    }
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
