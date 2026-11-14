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
#include "shadow_effect_params.hpp"
#include "theme.hpp"

MindMapData::MindMapData(QString name)
  : MindMapDataBase(name)
  , m_theme(std::make_unique<Theme>(SC::instance().settingsProxy()->defaultTheme()))
  , m_graph(std::make_unique<Graph>())
  , m_imageManager(std::make_unique<ImageManager>())
{
}

MindMapData::MindMapData(const MindMapData & other)
  : MindMapDataBase(other)
  , m_fileName(other.m_fileName)
  , m_applicationVersion(other.m_applicationVersion)
  , m_theme(std::make_unique<Theme>(*other.m_theme))
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
    return m_theme->backgroundColor;
}

void MindMapData::setBackgroundColor(const QColor & backgroundColor)
{
    m_theme->backgroundColor = backgroundColor;
}

int MindMapData::cornerRadius() const
{
    return m_theme->cornerRadius;
}

void MindMapData::setCornerRadius(int cornerRadius)
{
    m_theme->cornerRadius = cornerRadius;

    for (auto && node : m_graph->getNodes()) {
        node->setCornerRadius(cornerRadius);
    }
}

QColor MindMapData::edgeColor() const
{
    return m_theme->edgeColor;
}

void MindMapData::setEdgeColor(const QColor & edgeColor)
{
    m_theme->edgeColor = edgeColor;

    for (auto && edge : m_graph->getEdges()) {
        edge->setColor(edgeColor);
    }
}

QColor MindMapData::gridColor() const
{
    return m_theme->gridColor;
}

void MindMapData::setGridColor(const QColor & gridColor)
{
    m_theme->gridColor = gridColor;
}

double MindMapData::arrowSize() const
{
    return m_theme->arrowSize;
}

void MindMapData::setArrowSize(double arrowSize)
{
    m_theme->arrowSize = arrowSize;

    for (auto && edge : m_graph->getEdges()) {
        edge->setArrowSize(arrowSize);
    }
}

double MindMapData::edgeWidth() const
{
    return m_theme->edgeWidth;
}

void MindMapData::setEdgeWidth(double edgeWidth)
{
    m_theme->edgeWidth = edgeWidth;

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
    return m_theme->font;
}

void MindMapData::changeFont(QFont font)
{
    m_theme->font = font;

    for (auto && edge : m_graph->getEdges()) {
        edge->changeFont(font);
    }

    for (auto && node : m_graph->getNodes()) {
        node->changeFont(font);
    }
}

void MindMapData::setShadowEffect(const ShadowEffectParams & params)
{
    m_theme->shadowEffect = params;

    for (auto && node : m_graph->getNodes()) {
        node->setShadowEffect(params);
    }

    for (auto && edge : m_graph->getEdges()) {
        edge->setShadowEffect(params);
    }
}

const ShadowEffectParams & MindMapData::shadowEffect() const
{
    return m_theme->shadowEffect;
}

QColor MindMapData::edgeLabelColor() const
{
    return m_theme->edgeLabelColor;
}

QColor MindMapData::edgeLabelTextColor() const
{
    return m_theme->edgeLabelTextColor;
}

void MindMapData::setEdgeLabelColors(const QColor & labelColor, const QColor & labelTextColor)
{
    m_theme->edgeLabelColor = labelColor;
    m_theme->edgeLabelTextColor = labelTextColor;

    for (auto && edge : m_graph->getEdges()) {
        edge->setLabelColors(labelColor, labelTextColor);
    }
}

QColor MindMapData::nodeColor() const
{
    return m_theme->nodeColor;
}

void MindMapData::setNodeColor(const QColor & nodeColor)
{
    m_theme->nodeColor = nodeColor;
}

QColor MindMapData::nodeTextColor() const
{
    return m_theme->nodeTextColor;
}

void MindMapData::setNodeTextColor(const QColor & nodeTextColor)
{
    m_theme->nodeTextColor = nodeTextColor;
}

const Theme & MindMapData::theme() const
{
    return *m_theme;
}

void MindMapData::setTheme(const Theme & theme, bool applyNodeColors)
{
    *m_theme = theme;

    for (auto && edge : m_graph->getEdges()) {
        edge->setColor(theme.edgeColor);
        edge->setEdgeWidth(theme.edgeWidth);
        edge->setArrowSize(theme.arrowSize);
        edge->changeFont(theme.font);
        edge->setTextSize(theme.textSize);
        edge->setLabelColors(theme.edgeLabelColor, theme.edgeLabelTextColor);
        edge->setShadowEffect(theme.shadowEffect);
    }

    for (auto && node : m_graph->getNodes()) {
        node->setCornerRadius(theme.cornerRadius);
        node->changeFont(theme.font);
        node->setTextSize(theme.textSize);
        node->setShadowEffect(theme.shadowEffect);
        if (applyNodeColors) {
            node->setColor(theme.nodeColor);
            node->setTextColor(theme.nodeTextColor);
        }
    }
}

int MindMapData::textSize() const
{
    return m_theme->textSize;
}

void MindMapData::setTextSize(int textSize)
{
    m_theme->textSize = textSize;

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
