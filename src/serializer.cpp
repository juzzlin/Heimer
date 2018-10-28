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

#include "serializer.hpp"
#include "config.hpp"
#include "graph.hpp"
#include "node.hpp"
#include "simple_logger.hpp"

#include <cassert>
#include <functional>
#include <map>

#include <QDomElement>

static const double SCALE = 1000; // https://bugreports.qt.io/browse/QTBUG-67129

using std::make_shared;

static void writeColor(QDomElement & parent, QDomDocument & doc, QColor color, QString elementName)
{
    auto colorElement = doc.createElement(elementName);
    colorElement.setAttribute(Serializer::DataKeywords::Design::Color::R, color.red());
    colorElement.setAttribute(Serializer::DataKeywords::Design::Color::G, color.green());
    colorElement.setAttribute(Serializer::DataKeywords::Design::Color::B, color.blue());
    parent.appendChild(colorElement);
}

static void writeNodes(MindMapData & mindMapData, QDomElement & root, QDomDocument & doc)
{
    for (auto node : mindMapData.graph().getNodes())
    {
        auto nodeElement = doc.createElement(Serializer::DataKeywords::Design::Graph::NODE);
        nodeElement.setAttribute(Serializer::DataKeywords::Design::Graph::Node::INDEX, node->index());
        nodeElement.setAttribute(Serializer::DataKeywords::Design::Graph::Node::X, static_cast<int>(node->location().x() * SCALE));
        nodeElement.setAttribute(Serializer::DataKeywords::Design::Graph::Node::Y, static_cast<int>(node->location().y() * SCALE));
        nodeElement.setAttribute(Serializer::DataKeywords::Design::Graph::Node::W, static_cast<int>(node->size().width() * SCALE));
        nodeElement.setAttribute(Serializer::DataKeywords::Design::Graph::Node::H, static_cast<int>(node->size().height() * SCALE));
        root.appendChild(nodeElement);

        // Create a child node for the text content
        auto textElement = doc.createElement(Serializer::DataKeywords::Design::Graph::Node::TEXT);
        textElement.appendChild(doc.createTextNode(node->text()));
        nodeElement.appendChild(textElement);

        // Create a child node for color
        writeColor(nodeElement, doc, node->color(), Serializer::DataKeywords::Design::Graph::Node::COLOR);

        // Create a child node for text color
        writeColor(nodeElement, doc, node->textColor(), Serializer::DataKeywords::Design::Graph::Node::TEXT_COLOR);
    }
}

static void writeEdges(MindMapData & mindMapData, QDomElement & root, QDomDocument & doc)
{
    for (auto node : mindMapData.graph().getNodes())
    {
        auto edges = mindMapData.graph().getEdgesFromNode(node);
        for (auto && edge : edges)
        {
            auto edgeElement = doc.createElement(Serializer::DataKeywords::Design::Graph::EDGE);
            edgeElement.setAttribute(Serializer::DataKeywords::Design::Graph::Edge::INDEX0, edge->sourceNodeBase().index());
            edgeElement.setAttribute(Serializer::DataKeywords::Design::Graph::Edge::INDEX1, edge->targetNodeBase().index());
            root.appendChild(edgeElement);

            // Create a child node for the text content
            auto textElement = doc.createElement(Serializer::DataKeywords::Design::Graph::Node::TEXT);
            edgeElement.appendChild(textElement);
            auto textNode = doc.createTextNode(edge->text());
            textElement.appendChild(textNode);
        }
    }
}

static QColor readColorElement(const QDomElement & element)
{
    return {
        element.attribute(Serializer::DataKeywords::Design::Color::R, "255").toInt(),
        element.attribute(Serializer::DataKeywords::Design::Color::G, "255").toInt(),
        element.attribute(Serializer::DataKeywords::Design::Color::B, "255").toInt()
    };
}

static QString readFirstTextNodeContent(const QDomElement & element)
{
    for (int i = 0; i < element.childNodes().count(); i++)
    {
        const auto child = element.childNodes().at(i);
        if (child.isText())
        {
            return child.toText().nodeValue();
        }
    }
    return "";
}

static void elementWarning(const QDomElement & element)
{
    juzzlin::L().warning() << "Unknown element '" << element.nodeName().toStdString() << "'";
}

// Generic helper that loops through element's children
static void readChildren(const QDomElement & root, std::map<QString, std::function<void (const QDomElement &)> > handlerMap)
{
    auto domNode = root.firstChild();
    while (!domNode.isNull())
    {
        auto element = domNode.toElement();
        if (!element.isNull())
        {
            if (handlerMap.count(element.nodeName()))
            {
                handlerMap[element.nodeName()](element);
            }
            else
            {
                elementWarning(element);
            }
        }

        domNode = domNode.nextSibling();
    }
}

// The purpose of this #ifdef is to build GUILESS unit tests so that QTEST_GUILESS_MAIN can be used
#ifdef HEIMER_UNIT_TEST
static NodeBasePtr readNode(const QDomElement & element)
#else
static NodePtr readNode(const QDomElement & element)
#endif
{
#ifdef HEIMER_UNIT_TEST
    auto node = make_shared<NodeBase>();
#else
    // Init a new node. QGraphicsScene will take the ownership eventually.
    auto node = make_shared<Node>();
#endif
    node->setIndex(element.attribute(Serializer::DataKeywords::Design::Graph::Node::INDEX, "-1").toInt());
    node->setLocation(QPointF(
        element.attribute(Serializer::DataKeywords::Design::Graph::Node::X, "0").toInt() / SCALE,
        element.attribute(Serializer::DataKeywords::Design::Graph::Node::Y, "0").toInt() / SCALE));

    if (element.hasAttribute(Serializer::DataKeywords::Design::Graph::Node::W) &&
        element.hasAttribute(Serializer::DataKeywords::Design::Graph::Node::H))
    {
       node->setSize(QSizeF(
           element.attribute(Serializer::DataKeywords::Design::Graph::Node::W).toInt() / SCALE,
           element.attribute(Serializer::DataKeywords::Design::Graph::Node::H).toInt() / SCALE));
    }

    readChildren(element, {
        {
            QString(Serializer::DataKeywords::Design::Graph::Node::TEXT), [=] (const QDomElement & e) {
                node->setText(readFirstTextNodeContent(e));
            }
        },
        {
            QString(Serializer::DataKeywords::Design::Graph::Node::COLOR), [=] (const QDomElement & e) {
                node->setColor(readColorElement(e));
            }
        },
        {
            QString(Serializer::DataKeywords::Design::Graph::Node::TEXT_COLOR), [=] (const QDomElement & e) {
                node->setTextColor(readColorElement(e));
            }
        }
    });

    return node;
}

// The purpose of this #ifdef is to build GUILESS unit tests so that QTEST_GUILESS_MAIN can be used
#ifdef HEIMER_UNIT_TEST
static EdgeBasePtr readEdge(const QDomElement & element, MindMapDataPtr data)
#else
static EdgePtr readEdge(const QDomElement & element, MindMapDataPtr data)
#endif
{
    const int index0 = element.attribute(Serializer::DataKeywords::Design::Graph::Edge::INDEX0, "-1").toInt();
    const int index1 = element.attribute(Serializer::DataKeywords::Design::Graph::Edge::INDEX1, "-1").toInt();

#ifdef HEIMER_UNIT_TEST
    auto node0 = data->graph().getNode(index0);
    assert(node0);
    auto node1 = data->graph().getNode(index1);
    assert(node1);
    auto edge = make_shared<EdgeBase>(*node0, *node1);
#else
    // Init a new edge. QGraphicsScene will take the ownership eventually.
    auto node0 = std::dynamic_pointer_cast<Node>(data->graph().getNode(index0));
    assert(node0);
    auto node1 = std::dynamic_pointer_cast<Node>(data->graph().getNode(index1));
    assert(node1);
    auto edge = make_shared<Edge>(*node0, *node1);
#endif

    readChildren(element, {
        {
            QString(Serializer::DataKeywords::Design::Graph::Node::TEXT), [=] (const QDomElement & e) {
                edge->setText(readFirstTextNodeContent(e));
            }
        }
    });

    return edge;
}

static void readGraph(const QDomElement & graph, MindMapDataPtr data)
{
    readChildren(graph, {
        {
            QString(Serializer::DataKeywords::Design::Graph::NODE), [=] (const QDomElement & e) {
                data->graph().addNode(readNode(e));
            }
        },
        {
            QString(Serializer::DataKeywords::Design::Graph::EDGE), [=] (const QDomElement & e) {
                data->graph().addEdge(readEdge(e, data));
            }
        },
    });
}

MindMapDataPtr Serializer::fromXml(QDomDocument document)
{
    const auto design = document.documentElement();

    auto data = make_shared<MindMapData>();
    data->setVersion(design.attribute(DataKeywords::Design::APPLICATION_VERSION, "UNDEFINED"));

    readChildren(design, {
        {
            QString(Serializer::DataKeywords::Design::GRAPH), [=] (const QDomElement & e) {
                readGraph(e, data);
            }
        },
        {
            QString(Serializer::DataKeywords::Design::COLOR), [=] (const QDomElement & e) {
                data->setBackgroundColor(readColorElement(e));
            }
        },
        {
            QString(Serializer::DataKeywords::Design::EDGE_THICKNESS), [=] (const QDomElement & e) {
                data->setEdgeWidth(readFirstTextNodeContent(e).toDouble() / SCALE);
            }
        },
        {
            QString(Serializer::DataKeywords::Design::TEXT_SIZE), [=] (const QDomElement & e) {
                data->setTextSize(static_cast<int>(readFirstTextNodeContent(e).toDouble() / SCALE));
            }
        }
    });

    return data;
}

QDomDocument Serializer::toXml(MindMapData & mindMapData)
{
    QDomDocument doc;

    doc.appendChild(doc.createProcessingInstruction("xml", "version='1.0' encoding='UTF-8'"));

    auto design = doc.createElement(Serializer::DataKeywords::Design::DESIGN);
    design.setAttribute(Serializer::DataKeywords::Design::APPLICATION_VERSION, Config::APPLICATION_VERSION);
    doc.appendChild(design);

    writeColor(design, doc, mindMapData.backgroundColor(), Serializer::DataKeywords::Design::COLOR);

    auto edgeWidthElement = doc.createElement(Serializer::DataKeywords::Design::EDGE_THICKNESS);
    edgeWidthElement.appendChild(doc.createTextNode(QString::number(static_cast<int>(mindMapData.edgeWidth() * SCALE))));
    design.appendChild(edgeWidthElement);

    auto textSizeElement = doc.createElement(Serializer::DataKeywords::Design::TEXT_SIZE);
    textSizeElement.appendChild(doc.createTextNode(QString::number(static_cast<int>(mindMapData.textSize() * SCALE))));
    design.appendChild(textSizeElement);

    auto graph = doc.createElement(Serializer::DataKeywords::Design::GRAPH);
    design.appendChild(graph);

    writeNodes(mindMapData, graph, doc);
    writeEdges(mindMapData, graph, doc);

    return doc;
}
