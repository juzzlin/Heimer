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

#include "SerializerTest.hpp"

#include "../editor/serializer.hpp"
#include "../editor/mindmapdata.hpp"
#include "../editor/nodebase.hpp"

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

void SerializerTest::testSingleEdge()
{
    MindMapData outData;

    auto outNode0 = std::make_shared<NodeBase>();
    outData.graph().addNode(outNode0);

    auto outNode1 = std::make_shared<NodeBase>();
    outData.graph().addNode(outNode1);

    outData.graph().addEdge(outNode0, outNode1);

    // Serialize
    const auto document = Serializer::toXml(outData);

    // Deserialize
    const auto inData = Serializer::fromXml(document);

    auto edges = inData->graph().getEdgesFromNode(outNode0);
    QCOMPARE(edges.size(), 1);
    QCOMPARE(edges.count(outNode1->index()), 1);
}

void SerializerTest::testSingleNode()
{
    MindMapData outData;

    auto outNode = std::make_shared<NodeBase>();
    outNode->setColor(QColor(1, 2, 3));
    outNode->setLocation(QPointF(333.333, 666.666));
    outNode->setText("Lorem ipsum");
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
    QCOMPARE(node->text(), outNode->text());
}

QTEST_GUILESS_MAIN(SerializerTest)
