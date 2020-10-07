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

#include "alz_serializer.hpp"

#include "constants.hpp"
#include "graph.hpp"
#include "node.hpp"
#include "simple_logger.hpp"
#include "test_mode.hpp"

#include <cassert>
#include <functional>
#include <map>

#include <QDebug>
#include <QDomElement>
#include <QFile>
#include <QTemporaryDir>

namespace AlzSerializer {
namespace DataKeywords {
namespace Design {

static constexpr auto DESIGN = "design";

static constexpr auto APPLICATION_VERSION = "version";

static constexpr auto COLOR = "color";

static constexpr auto CORNER_RADIUS = "corner-radius";

static constexpr auto EDGE_COLOR = "edge-color";

static constexpr auto EDGE_THICKNESS = "edge-width";

static constexpr auto GRAPH = "graph";

static constexpr auto GRID_COLOR = "grid-color";

static constexpr auto IMAGE = "image";

static constexpr auto TEXT_SIZE = "text-size";

// Used for Design and Node
namespace Color {

static constexpr auto R = "r";

static constexpr auto G = "g";

static constexpr auto B = "b";
} // namespace Color

namespace Graph {

static constexpr auto NODE = "node";

namespace Node {

static constexpr auto COLOR = "color";

static constexpr auto IMAGE = "image";

static constexpr auto INDEX = "index";

static constexpr auto TEXT = "text";

static constexpr auto TEXT_COLOR = "text-color";

static constexpr auto X = "x";

static constexpr auto Y = "y";

static constexpr auto W = "w";

static constexpr auto H = "h";

namespace Image {

static constexpr auto REF = "ref";
} // namespace Image
} // namespace Node

static constexpr auto EDGE = "edge";

namespace Edge {

static constexpr auto INDEX0 = "index0";

static constexpr auto INDEX1 = "index1";

static constexpr auto ARROW_MODE = "arrow-mode";

static constexpr auto REVERSED = "reversed";

} // namespace Edge
} // namespace Graph

namespace Image {

static constexpr auto ID = "id";

static constexpr auto PATH = "path";

} // namespace Image

namespace LayoutOptimizer {

static constexpr auto ASPECT_RATIO = "aspect-ratio";

static constexpr auto LAYOUT_OPTIMIZER = "layout-optimizer";

static constexpr auto MIN_EDGE_LENGTH = "min-edge-length";

} // namespace LayoutOptimizer

} // namespace Design

} // namespace DataKeywords

static const double SCALE = 1000; // https://bugreports.qt.io/browse/QTBUG-67129

using std::make_shared;

static void writeColor(QDomElement & parent, QDomDocument & doc, QColor color, QString elementName)
{
    auto colorElement = doc.createElement(elementName);
    colorElement.setAttribute(DataKeywords::Design::Color::R, color.red());
    colorElement.setAttribute(DataKeywords::Design::Color::G, color.green());
    colorElement.setAttribute(DataKeywords::Design::Color::B, color.blue());
    parent.appendChild(colorElement);
}

static void writeImageRef(QDomElement & parent, QDomDocument & doc, size_t imageRef, QString elementName)
{
    auto colorElement = doc.createElement(elementName);
    colorElement.setAttribute(DataKeywords::Design::Graph::Node::Image::REF, static_cast<int>(imageRef));
    parent.appendChild(colorElement);
}

static void writeNodes(MindMapData & mindMapData, QDomElement & root, QDomDocument & doc)
{
    for (auto && node : mindMapData.graph().getNodes()) {
        auto nodeElement = doc.createElement(DataKeywords::Design::Graph::NODE);
        nodeElement.setAttribute(DataKeywords::Design::Graph::Node::INDEX, node->index());
        nodeElement.setAttribute(DataKeywords::Design::Graph::Node::X, static_cast<int>(node->location().x() * SCALE));
        nodeElement.setAttribute(DataKeywords::Design::Graph::Node::Y, static_cast<int>(node->location().y() * SCALE));
        nodeElement.setAttribute(DataKeywords::Design::Graph::Node::W, static_cast<int>(node->size().width() * SCALE));
        nodeElement.setAttribute(DataKeywords::Design::Graph::Node::H, static_cast<int>(node->size().height() * SCALE));
        root.appendChild(nodeElement);

        // Create a child node for the text content
        auto textElement = doc.createElement(DataKeywords::Design::Graph::Node::TEXT);
        textElement.appendChild(doc.createTextNode(node->text()));
        nodeElement.appendChild(textElement);

        // Create a child node for color
        writeColor(nodeElement, doc, node->color(), DataKeywords::Design::Graph::Node::COLOR);

        // Create a child node for text color
        writeColor(nodeElement, doc, node->textColor(), DataKeywords::Design::Graph::Node::TEXT_COLOR);

        // Create a child node for image ref
        if (node->imageRef()) {
            writeImageRef(nodeElement, doc, node->imageRef(), DataKeywords::Design::Graph::Node::IMAGE);
        }
    }
}

static void writeEdges(MindMapData & mindMapData, QDomElement & root, QDomDocument & doc)
{
    for (auto && node : mindMapData.graph().getNodes()) {
        for (auto && edge : mindMapData.graph().getEdgesFromNode(node)) {
            auto edgeElement = doc.createElement(DataKeywords::Design::Graph::EDGE);
            edgeElement.setAttribute(DataKeywords::Design::Graph::Edge::INDEX0, edge->sourceNode().index());
            edgeElement.setAttribute(DataKeywords::Design::Graph::Edge::INDEX1, edge->targetNode().index());
            edgeElement.setAttribute(DataKeywords::Design::Graph::Edge::ARROW_MODE, static_cast<int>(edge->arrowMode()));
            edgeElement.setAttribute(DataKeywords::Design::Graph::Edge::REVERSED, edge->reversed());
            root.appendChild(edgeElement);

            // Create a child node for the text content
            auto textElement = doc.createElement(DataKeywords::Design::Graph::Node::TEXT);
            edgeElement.appendChild(textElement);
            const auto textNode = doc.createTextNode(edge->text());
            textElement.appendChild(textNode);
        }
    }
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
    if (!TestMode::enabled()) {
        QTemporaryDir dir;
        if (dir.isValid()) {
            QFileInfo info(imagePath.c_str());
            const auto extractedFilePath = (dir.path() + QDir::separator() + info.fileName()).toStdString();
            juzzlin::L().info() << "Extracting embedded image id=" << imageId << " to '" << extractedFilePath << "'";
            QByteArray bytes = QByteArray::fromBase64(base64.c_str(), QByteArray::Base64Encoding);
            QFile out(extractedFilePath.c_str());
            if (!out.open(QIODevice::WriteOnly)) {
                throw std::runtime_error("Cannot open file: '" + extractedFilePath + "' for write!");
            }
            auto bytesWritten = out.write(bytes);
            juzzlin::L().info() << "Bytes written: " << bytesWritten << ", " << (bytesWritten == bytes.length() ? "OK" : "SIZE MISMATCH");
            return QImage { extractedFilePath.c_str() };
        } else {
            throw std::runtime_error("Temp dir not valid: '" + dir.path().toStdString() + "'");
        }
    } else {
        TestMode::logDisabledCode("base64ToQImage");
    }
    return {};
}

static void writeImages(MindMapData & mindMapData, QDomElement & root, QDomDocument & doc)
{
    for (auto && node : mindMapData.graph().getNodes()) {
        if (node->imageRef()) {
            Image image;
            bool exists;
            std::tie(image, exists) = mindMapData.imageManager().getImage(node->imageRef());
            if (exists) {
                auto imageElement = doc.createElement(DataKeywords::Design::IMAGE);
                imageElement.setAttribute(DataKeywords::Design::Image::ID, static_cast<int>(image.id()));
                imageElement.setAttribute(DataKeywords::Design::Image::PATH, image.path().c_str());
                root.appendChild(imageElement);

                // Create a child node for the image content
                imageElement.appendChild(doc.createTextNode(getBase64Data(image.path())));
            }
        }
    }
}

static void writeLayoutOptimizer(MindMapData & mindMapData, QDomElement & root, QDomDocument & doc)
{
    auto layoutOptimizerElement = doc.createElement(DataKeywords::Design::LayoutOptimizer::LAYOUT_OPTIMIZER);
    layoutOptimizerElement.setAttribute(DataKeywords::Design::LayoutOptimizer::ASPECT_RATIO, mindMapData.aspectRatio() * SCALE);
    layoutOptimizerElement.setAttribute(DataKeywords::Design::LayoutOptimizer::MIN_EDGE_LENGTH, mindMapData.minEdgeLength() * SCALE);
    root.appendChild(layoutOptimizerElement);
}

static QColor readColorElement(const QDomElement & element)
{
    return {
        element.attribute(DataKeywords::Design::Color::R, "255").toInt(),
        element.attribute(DataKeywords::Design::Color::G, "255").toInt(),
        element.attribute(DataKeywords::Design::Color::B, "255").toInt()
    };
}

static size_t readImageElement(const QDomElement & element)
{
    return static_cast<size_t>(element.attribute(DataKeywords::Design::Graph::Node::Image::REF, "0").toInt());
}

static QString readFirstTextNodeContent(const QDomElement & element)
{
    for (int i = 0; i < element.childNodes().count(); i++) {
        const auto child = element.childNodes().at(i);
        if (child.isText()) {
            // See: https://github.com/juzzlin/Heimer/issues/73
            return child.toText().nodeValue().replace(13, "");
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

// The purpose of this #ifdef is to build GUILESS unit tests so that QTEST_GUILESS_MAIN can be used
static NodePtr readNode(const QDomElement & element)
{
    // Init a new node. QGraphicsScene will take the ownership eventually.
    const auto node = make_shared<Node>();

    node->setIndex(element.attribute(DataKeywords::Design::Graph::Node::INDEX, "-1").toInt());
    node->setLocation(QPointF(
      element.attribute(DataKeywords::Design::Graph::Node::X, "0").toInt() / SCALE,
      element.attribute(DataKeywords::Design::Graph::Node::Y, "0").toInt() / SCALE));

    if (element.hasAttribute(DataKeywords::Design::Graph::Node::W) && element.hasAttribute(DataKeywords::Design::Graph::Node::H)) {
        node->setSize(QSizeF(
          element.attribute(DataKeywords::Design::Graph::Node::W).toInt() / SCALE,
          element.attribute(DataKeywords::Design::Graph::Node::H).toInt() / SCALE));
    }

    readChildren(element, { { QString(DataKeywords::Design::Graph::Node::TEXT), [=](const QDomElement & e) {
                                 node->setText(readFirstTextNodeContent(e));
                             } },
                            { QString(DataKeywords::Design::Graph::Node::COLOR), [=](const QDomElement & e) {
                                 node->setColor(readColorElement(e));
                             } },
                            { QString(DataKeywords::Design::Graph::Node::TEXT_COLOR), [=](const QDomElement & e) {
                                 node->setTextColor(readColorElement(e));
                             } },
                            { QString(DataKeywords::Design::Graph::Node::IMAGE), [=](const QDomElement & e) {
                                 node->setImageRef(readImageElement(e));
                             } } });

    return node;
}

static EdgePtr readEdge(const QDomElement & element, MindMapDataPtr data)
{
    const int index0 = element.attribute(DataKeywords::Design::Graph::Edge::INDEX0, "-1").toInt();
    const int index1 = element.attribute(DataKeywords::Design::Graph::Edge::INDEX1, "-1").toInt();
    const int reversed = element.attribute(DataKeywords::Design::Graph::Edge::REVERSED, "0").toInt();
    const int arrowMode = element.attribute(DataKeywords::Design::Graph::Edge::ARROW_MODE, "0").toInt();

    // Initialize a new edge. QGraphicsScene will take the ownership eventually.
    const auto node0 = data->graph().getNode(index0);
    const auto node1 = data->graph().getNode(index1);
    const auto edge = make_shared<Edge>(*node0, *node1);
    edge->setArrowMode(static_cast<Edge::ArrowMode>(arrowMode));
    edge->setReversed(reversed);

    readChildren(element, { { QString(DataKeywords::Design::Graph::Node::TEXT), [=](const QDomElement & e) {
                                 edge->setText(readFirstTextNodeContent(e));
                             } } });

    return edge;
}

static void readLayoutOptimizer(const QDomElement & element, MindMapDataPtr data)
{
    double aspectRatio = element.attribute(DataKeywords::Design::LayoutOptimizer::ASPECT_RATIO, "-1").toDouble() / SCALE;
    aspectRatio = std::min(aspectRatio, Constants::LayoutOptimizer::MAX_ASPECT_RATIO);
    aspectRatio = std::max(aspectRatio, Constants::LayoutOptimizer::MIN_ASPECT_RATIO);
    data->setAspectRatio(aspectRatio);

    double minEdgeLength = element.attribute(DataKeywords::Design::LayoutOptimizer::MIN_EDGE_LENGTH, "-1").toDouble() / SCALE;
    minEdgeLength = std::min(minEdgeLength, Constants::LayoutOptimizer::MAX_EDGE_LENGTH);
    minEdgeLength = std::max(minEdgeLength, Constants::LayoutOptimizer::MIN_EDGE_LENGTH);
    data->setMinEdgeLength(minEdgeLength);
}

static void readGraph(const QDomElement & graph, MindMapDataPtr data)
{
    readChildren(graph, {
                          { QString(DataKeywords::Design::Graph::NODE), [=](const QDomElement & e) {
                               data->graph().addNode(readNode(e));
                           } },
                          { QString(DataKeywords::Design::Graph::EDGE), [=](const QDomElement & e) {
                               data->graph().addEdge(readEdge(e, data));
                           } },
                        });
}

MindMapDataPtr fromXml(QDomDocument document)
{
    const auto design = document.documentElement();
    const auto data = make_shared<MindMapData>();
    data->setVersion(design.attribute(DataKeywords::Design::APPLICATION_VERSION, "UNDEFINED"));

    readChildren(design, { { QString(DataKeywords::Design::GRAPH), [=](const QDomElement & e) {
                                readGraph(e, data);
                            } },
                           { QString(DataKeywords::Design::COLOR), [=](const QDomElement & e) {
                                data->setBackgroundColor(readColorElement(e));
                            } },
                           { QString(DataKeywords::Design::EDGE_COLOR), [=](const QDomElement & e) {
                                data->setEdgeColor(readColorElement(e));
                            } },
                           { QString(DataKeywords::Design::GRID_COLOR), [=](const QDomElement & e) {
                                data->setGridColor(readColorElement(e));
                            } },
                           { QString(DataKeywords::Design::EDGE_THICKNESS), [=](const QDomElement & e) {
                                data->setEdgeWidth(readFirstTextNodeContent(e).toDouble() / SCALE);
                            } },
                           { QString(DataKeywords::Design::IMAGE), [=](const QDomElement & e) {
                                const auto id = e.attribute(DataKeywords::Design::Image::ID).toUInt();
                                const auto path = e.attribute(DataKeywords::Design::Image::PATH).toStdString();
                                Image image(base64ToQImage(readFirstTextNodeContent(e).toStdString(), id, path), path);
                                image.setId(id);
                                data->imageManager().setImage(image);
                            } },
                           { QString(DataKeywords::Design::TEXT_SIZE), [=](const QDomElement & e) {
                                data->setTextSize(static_cast<int>(readFirstTextNodeContent(e).toDouble() / SCALE));
                            } },
                           { QString(DataKeywords::Design::CORNER_RADIUS), [=](const QDomElement & e) {
                                data->setCornerRadius(static_cast<int>(readFirstTextNodeContent(e).toDouble() / SCALE));
                            } },
                           { QString(DataKeywords::Design::LayoutOptimizer::LAYOUT_OPTIMIZER), [=](const QDomElement & e) {
                                readLayoutOptimizer(e, data);
                            } } });

    return data;
}

QDomDocument toXml(MindMapData & mindMapData)
{
    QDomDocument doc;

    doc.appendChild(doc.createProcessingInstruction("xml", "version='1.0' encoding='UTF-8'"));

    auto design = doc.createElement(DataKeywords::Design::DESIGN);
    design.setAttribute(DataKeywords::Design::APPLICATION_VERSION, Constants::Application::APPLICATION_VERSION);
    doc.appendChild(design);

    writeColor(design, doc, mindMapData.backgroundColor(), DataKeywords::Design::COLOR);

    writeColor(design, doc, mindMapData.edgeColor(), DataKeywords::Design::EDGE_COLOR);

    writeColor(design, doc, mindMapData.gridColor(), DataKeywords::Design::GRID_COLOR);

    auto edgeWidthElement = doc.createElement(DataKeywords::Design::EDGE_THICKNESS);
    edgeWidthElement.appendChild(doc.createTextNode(QString::number(static_cast<int>(mindMapData.edgeWidth() * SCALE))));
    design.appendChild(edgeWidthElement);

    auto textSizeElement = doc.createElement(DataKeywords::Design::TEXT_SIZE);
    textSizeElement.appendChild(doc.createTextNode(QString::number(static_cast<int>(mindMapData.textSize() * SCALE))));
    design.appendChild(textSizeElement);

    auto cornerRadiusElement = doc.createElement(DataKeywords::Design::CORNER_RADIUS);
    cornerRadiusElement.appendChild(doc.createTextNode(QString::number(static_cast<int>(mindMapData.cornerRadius() * SCALE))));
    design.appendChild(cornerRadiusElement);

    auto graph = doc.createElement(DataKeywords::Design::GRAPH);
    design.appendChild(graph);

    writeNodes(mindMapData, graph, doc);

    writeEdges(mindMapData, graph, doc);

    writeImages(mindMapData, design, doc);

    writeLayoutOptimizer(mindMapData, design, doc);

    return doc;
}

} // namespace AlzSerializer
