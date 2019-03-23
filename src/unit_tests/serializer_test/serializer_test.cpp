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

#include "serializer_test.hpp"

#include "serializer.hpp"
#include "mind_map_data.hpp"
#include "node_base.hpp"

SerializerTest::SerializerTest()
{
}

void SerializerTest::testEmptyDesign()
{
    MindMapData outData;

    // Serialize
    const auto document = Serializer::toXml(outData);

    // Deserialize
    const auto inData = Serializer::fromXml(document);
    QCOMPARE(QString(inData->version()), QString(VERSION));
}

void SerializerTest::testBackgroundColor()
{
    MindMapData outData;
    outData.setBackgroundColor(QColor(1, 2, 3));

    // Serialize
    const auto document = Serializer::toXml(outData);

    // Deserialize
    const auto inData = Serializer::fromXml(document);
    QCOMPARE(inData->backgroundColor(), outData.backgroundColor());
}

void SerializerTest::testCornerRadius()
{
    MindMapData outData;
    outData.setCornerRadius(Constants::Node::DEFAULT_CORNER_RADIUS + 1);

    // Serialize
    const auto document = Serializer::toXml(outData);

    // Deserialize
    const auto inData = Serializer::fromXml(document);
    QCOMPARE(inData->cornerRadius(), outData.cornerRadius());
}

void SerializerTest::testEdgeColor()
{
    MindMapData outData;
    outData.setEdgeColor(QColor(1, 2, 3));

    // Serialize
    const auto document = Serializer::toXml(outData);

    // Deserialize
    const auto inData = Serializer::fromXml(document);
    QCOMPARE(inData->edgeColor(), outData.edgeColor());
}

void SerializerTest::testEdgeWidth()
{
    MindMapData outData;
    outData.setEdgeWidth(666.42);

    // Serialize
    const auto document = Serializer::toXml(outData);

    // Deserialize
    const auto inData = Serializer::fromXml(document);
    QCOMPARE(inData->edgeWidth(), outData.edgeWidth());
}

void SerializerTest::testTextSize()
{
    MindMapData outData;
    outData.setTextSize(42);

    // Serialize
    const auto document = Serializer::toXml(outData);

    // Deserialize
    const auto inData = Serializer::fromXml(document);
    QCOMPARE(inData->textSize(), outData.textSize());
}

void SerializerTest::testNodeDeletion()
{
    MindMapData outData;

    auto outNode0 = std::make_shared<NodeBase>();
    outData.graph().addNode(outNode0);

    auto outNode1 = std::make_shared<NodeBase>();
    outData.graph().addNode(outNode1);

    auto outNode2 = std::make_shared<NodeBase>();
    outData.graph().addNode(outNode2);

    outData.graph().addEdge(std::make_shared<EdgeBase>(*outNode0, *outNode1));

    outData.graph().addEdge(std::make_shared<EdgeBase>(*outNode0, *outNode2));

    outData.graph().deleteNode(outNode1->index()); // Delete node in between

    // Serialize
    const auto document = Serializer::toXml(outData);

    // Deserialize
    const auto inData = Serializer::fromXml(document);

    auto edges = inData->graph().getEdgesFromNode(outNode0);
    QCOMPARE(edges.size(), static_cast<size_t>(1));
}

void SerializerTest::testSingleEdge()
{
    MindMapData outData;

    auto outNode0 = std::make_shared<NodeBase>();
    outData.graph().addNode(outNode0);

    auto outNode1 = std::make_shared<NodeBase>();
    outData.graph().addNode(outNode1);

    auto edge = std::make_shared<EdgeBase>(*outNode0, *outNode1);
    const QString text = "Lorem ipsum";
    edge->setText(text);
    outData.graph().addEdge(edge);

    // Serialize
    const auto document = Serializer::toXml(outData);

    // Deserialize
    const auto inData = Serializer::fromXml(document);

    auto edges = inData->graph().getEdgesFromNode(outNode0);
    QCOMPARE(edges.size(), static_cast<size_t>(1));
    QCOMPARE((*edges.begin())->text(), text);
}

void SerializerTest::testSingleNode()
{
    MindMapData outData;

    auto outNode = std::make_shared<NodeBase>();
    outNode->setColor(QColor(1, 2, 3));
    outNode->setLocation(QPointF(333.333, 666.666));
    outNode->setSize(QSize(123, 321));
    outNode->setText("Lorem ipsum");
    outNode->setTextColor(QColor(4, 5, 6));
    outData.graph().addNode(outNode);

    // Serialize
    const auto document = Serializer::toXml(outData);

    // Deserialize
    const auto inData = Serializer::fromXml(document);
    QVERIFY(inData->graph().numNodes() == 1);

    auto node = inData->graph().getNode(0);
    QVERIFY(node != nullptr);
    QCOMPARE(node->index(), outNode->index());
    QCOMPARE(node->color(), outNode->color());
    QCOMPARE(node->location(), outNode->location());
    QCOMPARE(node->size(), outNode->size());
    QCOMPARE(node->text(), outNode->text());
    QCOMPARE(node->textColor(), outNode->textColor());
}

QTEST_GUILESS_MAIN(SerializerTest)
