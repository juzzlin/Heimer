// This file is part of Heimer.
// Copyright (C) 2022 Jussi Lind <jussi.lind@iki.fi>
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

#include "alz_file_io_worker.hpp"

#include "../../application/progress_manager.hpp"
#include "../../application/service_container.hpp"
#include "../../common/constants.hpp"
#include "../../common/test_mode.hpp"
#include "../../common/types.hpp"
#include "../../common/utils.hpp"
#include "../../domain/graph.hpp"
#include "../../domain/image_manager.hpp"
#include "../../domain/mind_map_data.hpp"
#include "../../view/scene_items/edge.hpp"
#include "../../view/scene_items/node.hpp"
#include "xml_reader.hpp"
#include "xml_writer.hpp"

#include "simple_logger.hpp"

#include <functional>
#include <map>
#include <set>

#include <QApplication>
#include <QDebug>
#include <QDomElement>
#include <QFile>
#include <QTemporaryDir>

namespace IO {

namespace DataKeywords::MindMap {

const auto ATTRIBUTE_APPLICATION_VERSION = "version";

namespace V2 {

const auto ATTRIBUTE_APPLICATION_VERSION = "application-version";

const auto ATTRIBUTE_ALZ_FORMAT_VERSION = "alz-format-version";

namespace Metadata {

const auto ELEMENT_METADATA = "metadata";

} // namespace Metadata

namespace Style {

const auto ELEMENT_STYLE = "style";

} // namespace Style

} // namespace V2

const auto ELEMENT_ARROW_SIZE = "arrow-size";

const auto ELEMENT_COLOR = "color";

const auto ELEMENT_CORNER_RADIUS = "corner-radius";

const auto ELEMENT_EDGE_COLOR = "edge-color";

const auto ELEMENT_EDGE_THICKNESS = "edge-width";

const auto ELEMENT_FONT_FAMILY = "font-family";

const auto ATTRIBUTE_FONT_BOLD = "bold";

const auto ATTRIBUTE_FONT_OVERLINE = "overline";

const auto ATTRIBUTE_FONT_STRIKE_OUT = "strike-out";

const auto ATTRIBUTE_FONT_UNDERLINE = "underline";

const auto ATTRIBUTE_FONT_WEIGHT = "weight";

const auto ATTRIBUTE_FONT_ITALIC = "italic";

const auto ELEMENT_GRAPH = "graph";

const auto ELEMENT_GRID_COLOR = "grid-color";

const auto ELEMENT_HEIMER_MIND_MAP = "heimer-mind-map";

const auto ELEMENT_IMAGE = "image";

const auto ELEMENT_TEXT_SIZE = "text-size";

// Used for Design and Node
namespace Color {

const auto ATTRIBUTE_R = "r";

const auto ATTRIBUTE_G = "g";

const auto ATTRIBUTE_B = "b";
} // namespace Color

namespace Graph {

const auto ELEMENT_NODE = "node";

namespace Node {

const auto ELEMENT_TEXT = "text";

const auto ATTRIBUTE_COLOR = "color";

const auto ATTRIBUTE_IMAGE = "image";

const auto ATTRIBUTE_INDEX = "index";

const auto ATTRIBUTE_TEXT_COLOR = "text-color";

const auto ATTRIBUTE_X = "x";

const auto ATTRIBUTE_Y = "y";

const auto ATTRIBUTE_W = "w";

const auto ATTRIBUTE_H = "h";

namespace Image {

const auto ATTRIBUTE_REF = "ref";
} // namespace Image

namespace V2 {

const auto ATTRIBUTE_INDEX = "i";
}

} // namespace Node

const auto ELEMENT_EDGE = "edge";

namespace Edge {

const auto ATTRIBUTE_ARROW_MODE = "arrow-mode";

const auto ATTRIBUTE_DASHED_LINE = "dashed-line";

const auto ATTRIBUTE_INDEX0 = "index0";

const auto ATTRIBUTE_INDEX1 = "index1";

const auto ATTRIBUTE_REVERSED = "reversed";

namespace V2 {

const auto ATTRIBUTE_INDEX0 = "i0";

const auto ATTRIBUTE_INDEX1 = "i1";

} // namespace V2

} // namespace Edge
} // namespace Graph

namespace Image {

const auto ATTRIBUTE_ID = "id";

const auto ATTRIBUTE_PATH = "path";

} // namespace Image

// Note!!: Moved into metadata element in V2
namespace LayoutOptimizer {

const auto ELEMENT_LAYOUT_OPTIMIZER = "layout-optimizer";

const auto ATTRIBUTE_ASPECT_RATIO = "aspect-ratio";

const auto ATTRIBUTE_MIN_EDGE_LENGTH = "min-edge-length";

} // namespace LayoutOptimizer

} // namespace DataKeywords::MindMap

static const double SCALE = 1000; // https://bugreports.qt.io/browse/QTBUG-67129

static void writeColor(QDomElement & parent, QDomDocument & doc, QColor color, QString elementName)
{
    auto colorElement = doc.createElement(elementName);
    colorElement.setAttribute(DataKeywords::MindMap::Color::ATTRIBUTE_R, color.red());
    colorElement.setAttribute(DataKeywords::MindMap::Color::ATTRIBUTE_G, color.green());
    colorElement.setAttribute(DataKeywords::MindMap::Color::ATTRIBUTE_B, color.blue());
    parent.appendChild(colorElement);
}

static void writeImageRef(QDomElement & parent, QDomDocument & doc, size_t imageRef, QString elementName)
{
    auto colorElement = doc.createElement(elementName);
    colorElement.setAttribute(DataKeywords::MindMap::Graph::Node::Image::ATTRIBUTE_REF, static_cast<int>(imageRef));
    parent.appendChild(colorElement);
}

static void writeNodes(MindMapDataS mindMapData, QDomElement & root, QDomDocument & doc, AlzFormatVersion outputVersion)
{
    for (auto && node : mindMapData->graph().getNodes()) {

        using namespace DataKeywords::MindMap::Graph;

        auto nodeElement = doc.createElement(ELEMENT_NODE);
        nodeElement.setAttribute(outputVersion == AlzFormatVersion::V1 ? Node::ATTRIBUTE_INDEX : Node::V2::ATTRIBUTE_INDEX, node->index());
        nodeElement.setAttribute(Node::ATTRIBUTE_X, static_cast<int>(node->location().x() * SCALE));
        nodeElement.setAttribute(Node::ATTRIBUTE_Y, static_cast<int>(node->location().y() * SCALE));
        nodeElement.setAttribute(Node::ATTRIBUTE_W, static_cast<int>(node->size().width() * SCALE));
        nodeElement.setAttribute(Node::ATTRIBUTE_H, static_cast<int>(node->size().height() * SCALE));
        root.appendChild(nodeElement);

        // Create a child node for the text content
        if (!node->text().isEmpty()) {
            auto textElement = doc.createElement(Node::ELEMENT_TEXT);
            textElement.appendChild(doc.createTextNode(node->text()));
            nodeElement.appendChild(textElement);
        }

        // Create a child node for color
        writeColor(nodeElement, doc, node->color(), Node::ATTRIBUTE_COLOR);

        // Create a child node for text color
        writeColor(nodeElement, doc, node->textColor(), Node::ATTRIBUTE_TEXT_COLOR);

        // Create a child node for image ref
        if (node->imageRef()) {
            writeImageRef(nodeElement, doc, node->imageRef(), Node::ATTRIBUTE_IMAGE);
        }
    }
}

static void writeEdges(MindMapDataS mindMapData, QDomElement & root, QDomDocument & doc, AlzFormatVersion outputVersion)
{
    for (auto && node : mindMapData->graph().getNodes()) {
        for (auto && edge : mindMapData->graph().getEdgesFromNode(node)) {
            using namespace DataKeywords::MindMap::Graph;
            auto edgeElement = doc.createElement(ELEMENT_EDGE);
            edgeElement.setAttribute(Edge::ATTRIBUTE_ARROW_MODE, static_cast<int>(edge->arrowMode()));
            if (edge->dashedLine()) {
                edgeElement.setAttribute(Edge::ATTRIBUTE_DASHED_LINE, edge->dashedLine());
            }
            edgeElement.setAttribute(outputVersion == AlzFormatVersion::V1 ? Edge::ATTRIBUTE_INDEX0 : Edge::V2::ATTRIBUTE_INDEX0, edge->sourceNode().index());
            edgeElement.setAttribute(outputVersion == AlzFormatVersion::V1 ? Edge::ATTRIBUTE_INDEX1 : Edge::V2::ATTRIBUTE_INDEX1, edge->targetNode().index());
            if (edge->reversed()) {
                edgeElement.setAttribute(Edge::ATTRIBUTE_REVERSED, edge->reversed());
            }
            root.appendChild(edgeElement);

            // Create a child node for the text content
            if (!edge->text().isEmpty()) {
                auto textElement = doc.createElement(Node::ELEMENT_TEXT);
                edgeElement.appendChild(textElement);
                const auto textNode = doc.createTextNode(edge->text());
                textElement.appendChild(textNode);
            }
        }
    }
}

static void writeGraph(MindMapDataS mindMapData, QDomElement & root, QDomDocument & doc, AlzFormatVersion outputVersion)
{
    auto graph = doc.createElement(DataKeywords::MindMap::ELEMENT_GRAPH);
    root.appendChild(graph);

    writeNodes(mindMapData, graph, doc, outputVersion);

    writeEdges(mindMapData, graph, doc, outputVersion);
}

static void writeStyle(MindMapDataS mindMapData, QDomElement & root, QDomDocument & doc, AlzFormatVersion outputVersion)
{
    const auto doWriteStyle = [](MindMapDataS mindMapData, QDomElement & root, QDomDocument & doc) {
        writeColor(root, doc, mindMapData->backgroundColor(), DataKeywords::MindMap::ELEMENT_COLOR);

        writeColor(root, doc, mindMapData->edgeColor(), DataKeywords::MindMap::ELEMENT_EDGE_COLOR);

        writeColor(root, doc, mindMapData->gridColor(), DataKeywords::MindMap::ELEMENT_GRID_COLOR);

        auto arrowSizeElement = doc.createElement(DataKeywords::MindMap::ELEMENT_ARROW_SIZE);
        arrowSizeElement.appendChild(doc.createTextNode(QString::number(static_cast<int>(mindMapData->arrowSize() * SCALE))));
        root.appendChild(arrowSizeElement);

        auto edgeWidthElement = doc.createElement(DataKeywords::MindMap::ELEMENT_EDGE_THICKNESS);
        edgeWidthElement.appendChild(doc.createTextNode(QString::number(static_cast<int>(mindMapData->edgeWidth() * SCALE))));
        root.appendChild(edgeWidthElement);

        auto fontFamilyElement = doc.createElement(DataKeywords::MindMap::ELEMENT_FONT_FAMILY);
        fontFamilyElement.setAttribute(DataKeywords::MindMap::ATTRIBUTE_FONT_BOLD, mindMapData->font().bold());
        fontFamilyElement.setAttribute(DataKeywords::MindMap::ATTRIBUTE_FONT_ITALIC, mindMapData->font().italic());
        fontFamilyElement.setAttribute(DataKeywords::MindMap::ATTRIBUTE_FONT_OVERLINE, mindMapData->font().overline());
        fontFamilyElement.setAttribute(DataKeywords::MindMap::ATTRIBUTE_FONT_STRIKE_OUT, mindMapData->font().strikeOut());
        fontFamilyElement.setAttribute(DataKeywords::MindMap::ATTRIBUTE_FONT_UNDERLINE, mindMapData->font().underline());
        fontFamilyElement.setAttribute(DataKeywords::MindMap::ATTRIBUTE_FONT_WEIGHT, Utils::fontWeightToInt(mindMapData->font().weight()));
        fontFamilyElement.appendChild(doc.createTextNode(mindMapData->font().family()));
        root.appendChild(fontFamilyElement);

        auto textSizeElement = doc.createElement(DataKeywords::MindMap::ELEMENT_TEXT_SIZE);
        textSizeElement.appendChild(doc.createTextNode(QString::number(static_cast<int>(mindMapData->textSize() * SCALE))));
        root.appendChild(textSizeElement);

        auto cornerRadiusElement = doc.createElement(DataKeywords::MindMap::ELEMENT_CORNER_RADIUS);
        cornerRadiusElement.appendChild(doc.createTextNode(QString::number(static_cast<int>(mindMapData->cornerRadius() * SCALE))));
        root.appendChild(cornerRadiusElement);
    };

    if (outputVersion == AlzFormatVersion::V1) {
        doWriteStyle(mindMapData, root, doc);
    } else {
        auto styleElement = doc.createElement(DataKeywords::MindMap::V2::Style::ELEMENT_STYLE);
        doWriteStyle(mindMapData, styleElement, doc);
        root.appendChild(styleElement);
    }
}

static void writeVersion(QDomElement & root, QDomDocument & doc, AlzFormatVersion outputVersion)
{
    if (outputVersion == AlzFormatVersion::V1) {
        root.setAttribute(DataKeywords::MindMap::ATTRIBUTE_APPLICATION_VERSION, Constants::Application::applicationVersion());
    } else {
        root.setAttribute(DataKeywords::MindMap::V2::ATTRIBUTE_APPLICATION_VERSION, Constants::Application::applicationVersion());
        root.setAttribute(DataKeywords::MindMap::V2::ATTRIBUTE_ALZ_FORMAT_VERSION, static_cast<int>(Constants::Application::alzFormatVersion()));
    }
    doc.appendChild(root);
}

static QString getBase64Data(std::string path)
{
    if (!TestMode::enabled()) {
        QFile in(path.c_str());
        if (!in.open(QIODevice::ReadOnly)) {
            throw std::runtime_error("Cannot open file: '" + path + "'");
        }
        return in.readAll().toBase64(QByteArray::Base64Encoding);
    } else {
        TestMode::logDisabledCode("getBase64Data");
        return {};
    }
}

static QImage base64ToQImage(const std::string & base64, size_t imageId, std::string imagePath)
{
    juzzlin::L().debug() << "Extracting embedded Image id=" << imageId << ", path=" << imagePath;
    const auto ba = QByteArray::fromBase64(base64.c_str(), QByteArray::Base64Encoding);
    QImage in;
    if (!in.loadFromData(ba)) {
        juzzlin::L().error() << "Could not load embedded Image id=" << imageId << ", path=" << imagePath;
    }
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    juzzlin::L().debug() << "Image size: " << in.sizeInBytes() << " bytes";
#endif
    return in;
}

static void writeImages(MindMapDataS mindMapData, QDomElement & root, QDomDocument & doc)
{
    std::set<size_t> writtenImageRefs;
    for (auto && node : mindMapData->graph().getNodes()) {
        if (node->imageRef()) {
            if (writtenImageRefs.count(node->imageRef())) {
                juzzlin::L().debug() << "Image id=" << node->imageRef() << " already written";
            } else {
                if (const auto image = mindMapData->imageManager().getImage(node->imageRef()); image.has_value()) {
                    auto imageElement = doc.createElement(DataKeywords::MindMap::ELEMENT_IMAGE);
                    imageElement.setAttribute(DataKeywords::MindMap::Image::ATTRIBUTE_ID, static_cast<int>(image->id()));
                    imageElement.setAttribute(DataKeywords::MindMap::Image::ATTRIBUTE_PATH, image->path().c_str());
                    root.appendChild(imageElement);

                    // Create a child node for the image content
                    if (!TestMode::enabled()) {
                        QTemporaryDir dir;
                        const QFileInfo info(image->path().c_str());
                        const QString tempImagePath = (dir.path() + QDir::separator() + info.fileName());
                        image->image().save(tempImagePath);
                        imageElement.appendChild(doc.createTextNode(getBase64Data(tempImagePath.toStdString())));
                        writtenImageRefs.insert(image->id());
                    } else {
                        TestMode::logDisabledCode("writeImages");
                    }
                } else {
                    throw std::runtime_error("Image id=" + std::to_string(node->imageRef()) + " doesn't exist!");
                }
            }
        }
    }
}

static void writeLayoutOptimizer(MindMapDataS mindMapData, QDomElement & root, QDomDocument & doc)
{
    auto layoutOptimizerElement = doc.createElement(DataKeywords::MindMap::LayoutOptimizer::ELEMENT_LAYOUT_OPTIMIZER);
    layoutOptimizerElement.setAttribute(DataKeywords::MindMap::LayoutOptimizer::ATTRIBUTE_ASPECT_RATIO, mindMapData->aspectRatio() * SCALE);
    layoutOptimizerElement.setAttribute(DataKeywords::MindMap::LayoutOptimizer::ATTRIBUTE_MIN_EDGE_LENGTH, mindMapData->minEdgeLength() * SCALE);
    root.appendChild(layoutOptimizerElement);
}

static void writeMetadata(MindMapDataS mindMapData, QDomElement & root, QDomDocument & doc, AlzFormatVersion outputVersion)
{
    if (outputVersion == AlzFormatVersion::V1) {
        writeLayoutOptimizer(mindMapData, root, doc);
    } else {
        auto metadataElement = doc.createElement(DataKeywords::MindMap::V2::Metadata::ELEMENT_METADATA);
        writeLayoutOptimizer(mindMapData, metadataElement, doc);
        root.appendChild(metadataElement);
    }
}

static QColor readColorElement(const QDomElement & element)
{
    return {
        element.attribute(DataKeywords::MindMap::Color::ATTRIBUTE_R, "255").toInt(),
        element.attribute(DataKeywords::MindMap::Color::ATTRIBUTE_G, "255").toInt(),
        element.attribute(DataKeywords::MindMap::Color::ATTRIBUTE_B, "255").toInt()
    };
}

static size_t readImageElement(const QDomElement & element)
{
    return static_cast<size_t>(element.attribute(DataKeywords::MindMap::Graph::Node::Image::ATTRIBUTE_REF, "0").toInt());
}

static QString readFirstTextNodeContent(const QDomElement & element)
{
    for (int i = 0; i < element.childNodes().count(); i++) {
        const auto child = element.childNodes().at(i);
        if (child.isText()) {
            // See: https://github.com/juzzlin/Heimer/issues/73
            return child.toText().nodeValue().replace(QChar(QChar::CarriageReturn), "");
        }
    }
    return {};
}

static void elementWarning(const QDomElement & element)
{
    juzzlin::L().warning() << "Unknown element '" << element.nodeName().toStdString() << "'";
}

// Generic helper that loops through element's children
static void readChildren(const QDomElement & root, std::map<QString, std::function<void(const QDomElement &)>> handlerMap)
{
    auto domNode = root.firstChild();
    while (!domNode.isNull()) {
        SC::instance().progressManager()->updateProgress();
        const auto element = domNode.toElement();
        if (!element.isNull()) {
            if (handlerMap.count(element.nodeName())) {
                handlerMap[element.nodeName()](element);
            } else {
                elementWarning(element);
            }
        }

        domNode = domNode.nextSibling();
    }
}

static NodeU readNode(const QDomElement & element)
{
    // Init a new node. QGraphicsScene will take the ownership eventually.
    using namespace DataKeywords::MindMap::Graph;
    auto node = std::make_unique<SceneItems::Node>();
    const auto noIndex = "-1";
    if (element.hasAttribute(Node::V2::ATTRIBUTE_INDEX)) {
        node->setIndex(element.attribute(Node::V2::ATTRIBUTE_INDEX, noIndex).toInt());
    } else {
        node->setIndex(element.attribute(Node::ATTRIBUTE_INDEX, noIndex).toInt());
    }

    node->setLocation(QPointF(
      element.attribute(Node::ATTRIBUTE_X, "0").toInt() / SCALE,
      element.attribute(Node::ATTRIBUTE_Y, "0").toInt() / SCALE));

    if (element.hasAttribute(Node::ATTRIBUTE_W) && element.hasAttribute(Node::ATTRIBUTE_H)) {
        node->setSize(QSizeF(
          element.attribute(Node::ATTRIBUTE_W).toInt() / SCALE,
          element.attribute(Node::ATTRIBUTE_H).toInt() / SCALE));
    }

    readChildren(element, { { QString(Node::ELEMENT_TEXT), [&node](const QDomElement & e) {
                                 node->setText(readFirstTextNodeContent(e));
                             } },
                            { QString(Node::ATTRIBUTE_COLOR), [&node](const QDomElement & e) {
                                 node->setColor(readColorElement(e));
                             } },
                            { QString(Node::ATTRIBUTE_TEXT_COLOR), [&node](const QDomElement & e) {
                                 node->setTextColor(readColorElement(e));
                             } },
                            { QString(Node::ATTRIBUTE_IMAGE), [&node](const QDomElement & e) {
                                 node->setImageRef(readImageElement(e));
                             } } });

    return node;
}

static EdgeU readEdge(const QDomElement & element, MindMapData & data)
{
    using namespace DataKeywords::MindMap::Graph;

    const int arrowMode = element.attribute(Edge::ATTRIBUTE_ARROW_MODE, "0").toInt();
    const bool dashedLine = element.attribute(Edge::ATTRIBUTE_DASHED_LINE, "0").toInt();

    const auto noIndex = "-1";

    const int index0 = element.hasAttribute(Edge::V2::ATTRIBUTE_INDEX0) ? element.attribute(Edge::V2::ATTRIBUTE_INDEX0, noIndex).toInt()
                                                                        : element.attribute(Edge::ATTRIBUTE_INDEX0, noIndex).toInt();

    const int index1 = element.hasAttribute(Edge::V2::ATTRIBUTE_INDEX1) ? element.attribute(Edge::V2::ATTRIBUTE_INDEX1, noIndex).toInt()
                                                                        : element.attribute(Edge::ATTRIBUTE_INDEX1, noIndex).toInt();

    const bool reversed = element.attribute(Edge::ATTRIBUTE_REVERSED, "0").toInt();

    // Initialize a new edge. QGraphicsScene will take the ownership eventually.
    auto edge = std::make_unique<SceneItems::Edge>(data.graph().getNode(index0), data.graph().getNode(index1));
    edge->setArrowMode(static_cast<SceneItems::EdgeModel::ArrowMode>(arrowMode));
    edge->setDashedLine(dashedLine);
    edge->setReversed(reversed);

    readChildren(element, { { QString(DataKeywords::MindMap::Graph::Node::ELEMENT_TEXT), [&edge](const QDomElement & e) {
                                 edge->setText(readFirstTextNodeContent(e));
                             } } });

    return edge;
}

static void readLayoutOptimizer(const QDomElement & element, MindMapData & data)
{
    double aspectRatio = element.attribute(DataKeywords::MindMap::LayoutOptimizer::ATTRIBUTE_ASPECT_RATIO, "-1").toDouble() / SCALE;
    aspectRatio = std::min(aspectRatio, Constants::LayoutOptimizer::maxAspectRatio());
    aspectRatio = std::max(aspectRatio, Constants::LayoutOptimizer::minAspectRatio());
    data.setAspectRatio(aspectRatio);

    double minEdgeLength = element.attribute(DataKeywords::MindMap::LayoutOptimizer::ATTRIBUTE_MIN_EDGE_LENGTH, "-1").toDouble() / SCALE;
    minEdgeLength = std::min(minEdgeLength, Constants::LayoutOptimizer::maxEdgeLength());
    minEdgeLength = std::max(minEdgeLength, Constants::LayoutOptimizer::minEdgeLength());
    data.setMinEdgeLength(minEdgeLength);
}

static void readMetadata(const QDomElement & element, MindMapData & data)
{
    readChildren(element, { { QString(DataKeywords::MindMap::LayoutOptimizer::ELEMENT_LAYOUT_OPTIMIZER), [&](const QDomElement & e) {
                                 readLayoutOptimizer(e, data);
                             } } });
}

static void readStyle(const QDomElement & element, MindMapData & data)
{
    readChildren(element, { { QString(DataKeywords::MindMap::ELEMENT_ARROW_SIZE), [&data](const QDomElement & e) {
                                 data.setArrowSize(readFirstTextNodeContent(e).toDouble() / SCALE);
                             } },
                            { QString(DataKeywords::MindMap::ELEMENT_COLOR), [&data](const QDomElement & e) {
                                 data.setBackgroundColor(readColorElement(e));
                             } },
                            { QString(DataKeywords::MindMap::ELEMENT_EDGE_COLOR), [&data](const QDomElement & e) {
                                 data.setEdgeColor(readColorElement(e));
                             } },
                            { QString(DataKeywords::MindMap::ELEMENT_FONT_FAMILY), [&data](const QDomElement & e) {
                                 QFont font;
                                 font.setFamily(readFirstTextNodeContent(e));
                                 font.setBold(e.attribute(DataKeywords::MindMap::ATTRIBUTE_FONT_BOLD).toInt());
                                 font.setItalic(e.attribute(DataKeywords::MindMap::ATTRIBUTE_FONT_ITALIC).toUInt());
                                 font.setOverline(e.attribute(DataKeywords::MindMap::ATTRIBUTE_FONT_OVERLINE).toInt());
                                 font.setUnderline(e.attribute(DataKeywords::MindMap::ATTRIBUTE_FONT_UNDERLINE).toInt());
                                 font.setStrikeOut(e.attribute(DataKeywords::MindMap::ATTRIBUTE_FONT_STRIKE_OUT).toInt());
                                 font.setWeight(Utils::intToFontWeight(e.attribute(DataKeywords::MindMap::ATTRIBUTE_FONT_WEIGHT).toInt()));
                                 data.changeFont(font);
                             } },
                            { QString(DataKeywords::MindMap::ELEMENT_GRID_COLOR), [&data](const QDomElement & e) {
                                 data.setGridColor(readColorElement(e));
                             } },
                            { QString(DataKeywords::MindMap::ELEMENT_EDGE_THICKNESS), [&data](const QDomElement & e) {
                                 data.setEdgeWidth(readFirstTextNodeContent(e).toDouble() / SCALE);
                             } },
                            { QString(DataKeywords::MindMap::ELEMENT_TEXT_SIZE), [&data](const QDomElement & e) {
                                 data.setTextSize(static_cast<int>(readFirstTextNodeContent(e).toDouble() / SCALE));
                             } },
                            { QString(DataKeywords::MindMap::ELEMENT_CORNER_RADIUS), [&data](const QDomElement & e) {
                                 data.setCornerRadius(static_cast<int>(readFirstTextNodeContent(e).toDouble() / SCALE));
                             } } });
}

static void readGraph(const QDomElement & graph, MindMapData & data)
{
    readChildren(graph, {
                          { QString(DataKeywords::MindMap::Graph::ELEMENT_NODE), [&data](const QDomElement & e) {
                               data.graph().addNode(readNode(e));
                           } },
                          { QString(DataKeywords::MindMap::Graph::ELEMENT_EDGE), [&data](const QDomElement & e) {
                               data.graph().addEdge(readEdge(e, data));
                           } },
                        });
}

// Import always assumes the newest ALZ-format version, but it's backwards compatible
MindMapDataU fromXml(QDomDocument document)
{
    auto data = std::make_unique<MindMapData>();

    const auto root = document.documentElement();
    const auto undefinedVersion = "UNDEFINED";
    data->setApplicationVersion(root.hasAttribute(DataKeywords::MindMap::V2::ATTRIBUTE_APPLICATION_VERSION) ? //
                                  root.attribute(DataKeywords::MindMap::V2::ATTRIBUTE_APPLICATION_VERSION, undefinedVersion)
                                                                                                            : //
                                  root.attribute(DataKeywords::MindMap::ATTRIBUTE_APPLICATION_VERSION, undefinedVersion));

    data->setAlzFormatVersion(
      static_cast<IO::AlzFormatVersion>(
        root.attribute(DataKeywords::MindMap::V2::ATTRIBUTE_ALZ_FORMAT_VERSION, "1").toInt()));

    readChildren(root, { { QString(DataKeywords::MindMap::ELEMENT_GRAPH), [&data](const QDomElement & e) {
                              readGraph(e, *data);
                          } },
                         { QString(DataKeywords::MindMap::ELEMENT_ARROW_SIZE), [&data](const QDomElement & e) {
                              data->setArrowSize(readFirstTextNodeContent(e).toDouble() / SCALE);
                          } },
                         { QString(DataKeywords::MindMap::ELEMENT_COLOR), [&data](const QDomElement & e) {
                              data->setBackgroundColor(readColorElement(e));
                          } },
                         { QString(DataKeywords::MindMap::ELEMENT_EDGE_COLOR), [&data](const QDomElement & e) {
                              data->setEdgeColor(readColorElement(e));
                          } },
                         { QString(DataKeywords::MindMap::ELEMENT_FONT_FAMILY), [&data](const QDomElement & e) {
                              QFont font;
                              font.setFamily(readFirstTextNodeContent(e));
                              font.setBold(e.attribute(DataKeywords::MindMap::ATTRIBUTE_FONT_BOLD).toInt());
                              font.setItalic(e.attribute(DataKeywords::MindMap::ATTRIBUTE_FONT_ITALIC).toUInt());
                              font.setOverline(e.attribute(DataKeywords::MindMap::ATTRIBUTE_FONT_OVERLINE).toInt());
                              font.setUnderline(e.attribute(DataKeywords::MindMap::ATTRIBUTE_FONT_UNDERLINE).toInt());
                              font.setStrikeOut(e.attribute(DataKeywords::MindMap::ATTRIBUTE_FONT_STRIKE_OUT).toInt());
                              font.setWeight(Utils::intToFontWeight(e.attribute(DataKeywords::MindMap::ATTRIBUTE_FONT_WEIGHT).toInt()));
                              data->changeFont(font);
                          } },
                         { QString(DataKeywords::MindMap::ELEMENT_GRID_COLOR), [&data](const QDomElement & e) {
                              data->setGridColor(readColorElement(e));
                          } },
                         { QString(DataKeywords::MindMap::ELEMENT_EDGE_THICKNESS), [&data](const QDomElement & e) {
                              data->setEdgeWidth(readFirstTextNodeContent(e).toDouble() / SCALE);
                          } },
                         { QString(DataKeywords::MindMap::ELEMENT_IMAGE), [&data](const QDomElement & e) {
                              const auto id = e.attribute(DataKeywords::MindMap::Image::ATTRIBUTE_ID).toUInt();
                              const auto path = e.attribute(DataKeywords::MindMap::Image::ATTRIBUTE_PATH).toStdString();
                              Image image(base64ToQImage(readFirstTextNodeContent(e).toStdString(), id, path), path);
                              image.setId(id);
                              data->imageManager().setImage(image);
                          } },
                         { QString(DataKeywords::MindMap::ELEMENT_TEXT_SIZE), [&data](const QDomElement & e) {
                              data->setTextSize(static_cast<int>(readFirstTextNodeContent(e).toDouble() / SCALE));
                          } },
                         { QString(DataKeywords::MindMap::ELEMENT_CORNER_RADIUS), [&data](const QDomElement & e) {
                              data->setCornerRadius(static_cast<int>(readFirstTextNodeContent(e).toDouble() / SCALE));
                          } },
                         { QString(DataKeywords::MindMap::LayoutOptimizer::ELEMENT_LAYOUT_OPTIMIZER), [&data](const QDomElement & e) {
                              readLayoutOptimizer(e, *data);
                          } },
                         { QString(DataKeywords::MindMap::V2::Metadata::ELEMENT_METADATA), [&data](const QDomElement & e) {
                              readMetadata(e, *data);
                          } },
                         { QString(DataKeywords::MindMap::V2::Style::ELEMENT_STYLE), [&data](const QDomElement & e) {
                              readStyle(e, *data);
                          } } });

    return data;
}

QDomDocument toXml(MindMapDataS mindMapData, AlzFormatVersion outputVersion)
{
    QDomDocument doc;
    doc.appendChild(doc.createProcessingInstruction("xml", "version='1.0' encoding='UTF-8'"));
    auto root = doc.createElement(DataKeywords::MindMap::ELEMENT_HEIMER_MIND_MAP);

    writeVersion(root, doc, outputVersion);

    writeStyle(mindMapData, root, doc, outputVersion);

    writeGraph(mindMapData, root, doc, outputVersion);

    writeImages(mindMapData, root, doc);

    writeMetadata(mindMapData, root, doc, outputVersion);

    return doc;
}

AlzFileIOWorker::AlzFileIOWorker(AlzFormatVersion outputVersion)
  : m_outputVersion(outputVersion)
{
}

AlzFileIOWorker::~AlzFileIOWorker() = default;

MindMapDataU AlzFileIOWorker::fromFile(QString path) const
{
    return IO::fromXml(XmlReader::readFromFile(path));
}

bool AlzFileIOWorker::toFile(MindMapDataS mindMapData, QString path) const
{
    return XmlWriter::writeToFile(IO::toXml(mindMapData, m_outputVersion), path);
}

MindMapDataU AlzFileIOWorker::fromXml(QString xml) const
{
    QDomDocument document;
    document.setContent(xml, false);
    SC::instance().progressManager()->updateProgress();
    return IO::fromXml(document);
}

QString AlzFileIOWorker::toXml(MindMapDataS mindMapData) const
{
    return IO::toXml(mindMapData, m_outputVersion).toString();
}

} // namespace IO
