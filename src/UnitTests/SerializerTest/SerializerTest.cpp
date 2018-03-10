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

SerializerTest::SerializerTest()
{
}

void SerializerTest::testEmptyDesign()
{
    MindMapData outData;

    // Serialize
    Serializer dut(outData);
    const auto document = dut.toXml();

    // Deserialize
    const auto inData = dut.fromXml(document);
    QCOMPARE(QString(inData->version()), QString(VERSION));
}

void SerializerTest::testSingleNode()
{
    MindMapData outData;

    // Serialize
    Serializer dut(outData);
    NodeBasePtr outNode = NodeBasePtr(new NodeBase);
    outData.graph().addNode(outNode);

    outNode->setLocation(QPointF(333.333, 666.666));
    outNode->setText("Lorem ipsum");

    const auto document = dut.toXml();

    // Deserialize
    const auto inData = dut.fromXml(document);
    QVERIFY(inData->graph().numNodes() == 1);

    auto node = inData->graph().get(0);
    QVERIFY(node.get());
    QCOMPARE(node->index(), outNode->index());
    QCOMPARE(node->location(), outNode->location());
    QCOMPARE(node->text(), outNode->text());
}

QTEST_GUILESS_MAIN(SerializerTest)
