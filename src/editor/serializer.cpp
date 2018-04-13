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

#include "serializer.hpp"
#include "config.hpp"
#include "graph.hpp"
#include "node.hpp"
#include "mclogger.hh"

#include <functional>
#include <map>

#include <QDomElement>

static const double SCALE = 1000; // https://bugreports.qt.io/browse/QTBUG-67129

using std::make_shared;

static void writeNodes(MindMapData & mindMapData, QDomElement & root, QDomDocument & doc)
{
    for (auto node : mindMapData.graph().getNodes())
    {
        auto nodeElement = doc.createElement(Serializer::DataKeywords::Design::Graph::NODE);
        nodeElement.setAttribute(Serializer::DataKeywords::Design::Graph::Node::INDEX, node->index());
        nodeElement.setAttribute(Serializer::DataKeywords::Design::Graph::Node::X, static_cast<int>(node->location().x() * SCALE));
        nodeElement.setAttribute(Serializer::DataKeywords::Design::Graph::Node::Y, static_cast<int>(node->location().y() * SCALE));
        root.appendChild(nodeElement);

        // Create a child node for the text content
        auto textElement = doc.createElement(Serializer::DataKeywords::Design::Graph::Node::TEXT);
        nodeElement.appendChild(textElement);
        auto textNode = doc.createTextNode(node->text());
        textElement.appendChild(textNode);

        // Create a child node for color
        auto colorElement = doc.createElement(Serializer::DataKeywords::Design::Graph::Node::COLOR);
        colorElement.setAttribute(Serializer::DataKeywords::Design::Color::R, node->color().red());
        colorElement.setAttribute(Serializer::DataKeywords::Design::Color::G, node->color().green());
        colorElement.setAttribute(Serializer::DataKeywords::Design::Color::B, node->color().blue());
        nodeElement.appendChild(colorElement);
    }
}

static void writeEdges(MindMapData & mindMapData, QDomElement & root, QDomDocument & doc)
{
    for (auto node : mindMapData.graph().getNodes())
    {
        auto edges = mindMapData.graph().getEdgesFromNode(node);
        for (auto index1 : edges)
        {
            auto nodeElement = doc.createElement(Serializer::DataKeywords::Design::Graph::EDGE);
            nodeElement.setAttribute(Serializer::DataKeywords::Design::Graph::Edge::INDEX0, node->index());
            nodeElement.setAttribute(Serializer::DataKeywords::Design::Graph::Edge::INDEX1, index1);
            root.appendChild(nodeElement);
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
    MCLogger().warning() << "Unknown element '" << element.nodeName().toStdString() << "'";
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
#ifdef DEMENTIA_UNIT_TEST
static NodeBasePtr readNode(const QDomElement & element)
#else
static NodePtr readNode(const QDomElement & element)
#endif
{
#ifdef DEMENTIA_UNIT_TEST
    auto node = make_shared<NodeBase>();
#else
    // Init a new node. QGraphicsScene will take the ownership eventually.
    auto node = make_shared<Node>();
#endif
    node->setIndex(element.attribute(Serializer::DataKeywords::Design::Graph::Node::INDEX, "-1").toInt());
    node->setLocation(QPointF(
        element.attribute(Serializer::DataKeywords::Design::Graph::Node::X, "0").toInt() / SCALE,
        element.attribute(Serializer::DataKeywords::Design::Graph::Node::Y, "0").toInt() / SCALE));

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
    });

    return node;
}

static std::pair<int, int> readEdge(const QDomElement & element)
{
    return {
        element.attribute(Serializer::DataKeywords::Design::Graph::Edge::INDEX0, "-1").toInt(),
        element.attribute(Serializer::DataKeywords::Design::Graph::Edge::INDEX1, "-1").toInt()
    };
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
                auto && edge = readEdge(e);
                data->graph().addEdge(edge.first, edge.second);
            }
        },
    });
}

MindMapDataPtr Serializer::fromXml(QDomDocument document)
{
    const auto design = document.documentElement();
    const auto version = design.attribute(DataKeywords::Design::APPLICATION_VERSION, "UNDEFINED");

    auto data = make_shared<MindMapData>();
    data->setVersion(version);

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
        }
    });

    return data;
}

QDomDocument Serializer::toXml(MindMapData & mindMapData)
{
    QDomDocument doc;

    auto processingInstruction = doc.createProcessingInstruction("xml", "version='1.0' encoding='UTF-8'");
    doc.appendChild(processingInstruction);

    auto design = doc.createElement(Serializer::DataKeywords::Design::DESIGN);
    design.setAttribute(Serializer::DataKeywords::Design::APPLICATION_VERSION, Config::APPLICATION_VERSION);
    doc.appendChild(design);

    auto colorElement = doc.createElement(Serializer::DataKeywords::Design::COLOR);
    colorElement.setAttribute(Serializer::DataKeywords::Design::Color::R, mindMapData.backgroundColor().red());
    colorElement.setAttribute(Serializer::DataKeywords::Design::Color::G, mindMapData.backgroundColor().green());
    colorElement.setAttribute(Serializer::DataKeywords::Design::Color::B, mindMapData.backgroundColor().blue());
    design.appendChild(colorElement);

    auto graph = doc.createElement(Serializer::DataKeywords::Design::GRAPH);
    design.appendChild(graph);

    writeNodes(mindMapData, graph, doc);
    writeEdges(mindMapData, graph, doc);

    return doc;
}
