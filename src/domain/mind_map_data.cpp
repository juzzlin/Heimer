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

#include "../application/service_container.hpp"
#include "../application/settings_proxy.hpp"
#include "../domain/graph.hpp"
#include "../domain/image_manager.hpp"
#include "../view/grid.hpp"
#include "../view/scene_items/node.hpp"
#include "../view/shadow_effect_params.hpp"

struct MindMapData::Style
{
    Style(const SettingsProxy & settingsProxy)
      : arrowSize(settingsProxy.arrowSize())
      , backgroundColor(settingsProxy.backgroundColor())
      , edgeColor(settingsProxy.edgeColor())
      , edgeWidth(settingsProxy.edgeWidth())
      , font(settingsProxy.font())
      , gridColor(settingsProxy.gridColor())
      , textSize(settingsProxy.textSize())
    {
    }

    double arrowSize;

    QColor backgroundColor;

    QColor edgeColor;

    double edgeWidth;

    QFont font;

    QColor gridColor;

    int textSize;

    int cornerRadius = Constants::Node::Defaults::CORNER_RADIUS;
};

MindMapData::MindMapData(QString name)
  : MindMapDataBase(name)
  , m_style(std::make_unique<Style>(*SIC::instance().settingsProxy()))
  , m_graph(std::make_unique<Graph>())
  , m_imageManager(std::make_unique<ImageManager>())
{
}

MindMapData::MindMapData(const MindMapData & other)
  : MindMapDataBase(other)
  , m_fileName(other.m_fileName)
  , m_applicationVersion(other.m_applicationVersion)
  , m_style(std::make_unique<Style>(*other.m_style))
  , m_graph(std::make_unique<Graph>())
  , m_imageManager(std::make_unique<ImageManager>(*other.m_imageManager))
  , m_layoutOptimizerParameters(other.m_layoutOptimizerParameters)
{
    copyGraph(other);
}

void MindMapData::copyGraph(const MindMapData & other)
{
    m_graph->clear();

    // Use copy constructor for nodes
    for (auto && node : other.m_graph->getNodes()) {
        m_graph->addNode(std::make_unique<SceneItems::Node>(*node));
    }

    // Use copy constructor for edges
    for (auto && otherEdge : other.m_graph->getEdges()) {
        m_graph->addEdge(std::make_unique<SceneItems::Edge>(*otherEdge, *m_graph));
    }
}

std::optional<IO::AlzFormatVersion> MindMapData::alzFormatVersion() const
{
    return m_alzFormatVersion;
}

void MindMapData::setAlzFormatVersion(IO::AlzFormatVersion alzFormatVersion)
{
    m_alzFormatVersion = alzFormatVersion;
}

QString MindMapData::applicationVersion() const
{
    return m_applicationVersion;
}

void MindMapData::setApplicationVersion(const QString & version)
{
    m_applicationVersion = version;
}

void MindMapData::applyGrid(const Grid & grid)
{
    for (auto && node : m_graph->getNodes()) {
        node->setLocation(grid.snapToGrid(node->location()));
    }
}

double MindMapData::aspectRatio() const
{
    return m_layoutOptimizerParameters.aspectRatio;
}

void MindMapData::setAspectRatio(double aspectRatio)
{
    m_layoutOptimizerParameters.aspectRatio = aspectRatio;
}

QColor MindMapData::backgroundColor() const
{
    return m_style->backgroundColor;
}

void MindMapData::setBackgroundColor(const QColor & backgroundColor)
{
    m_style->backgroundColor = backgroundColor;
}

int MindMapData::cornerRadius() const
{
    return m_style->cornerRadius;
}

void MindMapData::setCornerRadius(int cornerRadius)
{
    m_style->cornerRadius = cornerRadius;

    for (auto && node : m_graph->getNodes()) {
        node->setCornerRadius(cornerRadius);
    }
}

QColor MindMapData::edgeColor() const
{
    return m_style->edgeColor;
}

void MindMapData::setEdgeColor(const QColor & edgeColor)
{
    m_style->edgeColor = edgeColor;

    for (auto && edge : m_graph->getEdges()) {
        edge->setColor(edgeColor);
    }
}

QColor MindMapData::gridColor() const
{
    return m_style->gridColor;
}

void MindMapData::setGridColor(const QColor & gridColor)
{
    m_style->gridColor = gridColor;
}

double MindMapData::arrowSize() const
{
    return m_style->arrowSize;
}

void MindMapData::setArrowSize(double arrowSize)
{
    m_style->arrowSize = arrowSize;

    for (auto && edge : m_graph->getEdges()) {
        edge->setArrowSize(arrowSize);
    }
}

double MindMapData::edgeWidth() const
{
    return m_style->edgeWidth;
}

void MindMapData::setEdgeWidth(double edgeWidth)
{
    m_style->edgeWidth = edgeWidth;

    for (auto && edge : m_graph->getEdges()) {
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
    return *m_graph;
}

GraphCR MindMapData::graph() const
{
    return *m_graph;
}

ImageManager & MindMapData::imageManager()
{
    return *m_imageManager;
}

const ImageManager & MindMapData::imageManager() const
{
    return *m_imageManager;
}

double MindMapData::minEdgeLength() const
{
    return m_layoutOptimizerParameters.minEdgeLength;
}

void MindMapData::setMinEdgeLength(double minEdgeLength)
{
    m_layoutOptimizerParameters.minEdgeLength = minEdgeLength;
}

void MindMapData::mirror(bool vertically)
{
    if (!m_graph->getNodes().size()) {
        return;
    }

    const auto firstNode = m_graph->getNodes().at(0);
    QRectF rect = firstNode->placementBoundingRect().translated(firstNode->location());
    for (auto && node : m_graph->getNodes()) {
        const auto pbr = node->placementBoundingRect().translated(node->location());
        rect = rect.united(pbr);
    }

    if (vertically) {
        for (auto && node : m_graph->getNodes()) {
            const auto centerY = (rect.y() + rect.height() / 2);
            node->setLocation({ node->location().x(), centerY * 2 - node->location().y() });
        }
    } else {
        for (auto && node : m_graph->getNodes()) {
            const auto centerX = (rect.x() + rect.width() / 2);
            node->setLocation({ centerX * 2 - node->location().x(), node->location().y() });
        }
    }
}

QFont MindMapData::font() const
{
    return m_style->font;
}

void MindMapData::changeFont(QFont font)
{
    m_style->font = font;

    for (auto && edge : m_graph->getEdges()) {
        edge->changeFont(font);
    }

    for (auto && node : m_graph->getNodes()) {
        node->changeFont(font);
    }
}

void MindMapData::setShadowEffect(const ShadowEffectParams & params)
{
    for (auto && node : m_graph->getNodes()) {
        node->setShadowEffect(params);
    }

    for (auto && edge : m_graph->getEdges()) {
        edge->setShadowEffect(params);
    }
}

int MindMapData::textSize() const
{
    return m_style->textSize;
}

void MindMapData::setTextSize(int textSize)
{
    m_style->textSize = textSize;

    for (auto && edge : m_graph->getEdges()) {
        edge->setTextSize(textSize);
    }

    for (auto && node : m_graph->getNodes()) {
        node->setTextSize(textSize);
    }
}

MindMapStats MindMapData::stats() const
{
    MindMapStats mms;

    if (m_graph->edgeCount()) {

        // Calculate average edge length
        double averageEdgeLength = 0;
        for (auto && edge : m_graph->getEdges()) {
            averageEdgeLength += edge->length();
        }
        averageEdgeLength /= m_graph->edgeCount();
        mms.averageEdgeLength = averageEdgeLength;

        // Calculate minimum edge length
        double minimumEdgeLength = m_graph->getEdges().at(0)->length();
        for (auto && edge : m_graph->getEdges()) {
            minimumEdgeLength = std::min(minimumEdgeLength, edge->length());
        }
        mms.minimumEdgeLength = minimumEdgeLength;

        // Calculate maximum edge length
        double maximumEdgeLength = m_graph->getEdges().at(0)->length();
        for (auto && edge : m_graph->getEdges()) {
            maximumEdgeLength = std::max(maximumEdgeLength, edge->length());
        }
        mms.maximumEdgeLength = maximumEdgeLength;
    }

    // Calculate layout aspect ratio
    QRectF rect;
    for (auto && node : m_graph->getNodes()) {
        const auto nodeRect = node->placementBoundingRect();
        rect = rect.united(nodeRect.translated(node->pos().x(), node->pos().y()));
    }
    mms.layoutAspectRatio = rect.width() / rect.height();

    return mms;
}

MindMapData::~MindMapData() = default;
