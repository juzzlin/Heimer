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

#include "mind_map_data.hpp"
#include "serializer.hpp"

SerializerTest::SerializerTest()
{
}

void SerializerTest::testEmptyDesign()
{
    MindMapData outData;
    const auto inData = Serializer::fromXml(Serializer::toXml(outData));
    QCOMPARE(QString(inData->version()), QString(VERSION));
}

void SerializerTest::testBackgroundColor()
{
    MindMapData outData;
    outData.setBackgroundColor(QColor(1, 2, 3));
    const auto inData = Serializer::fromXml(Serializer::toXml(outData));
    QCOMPARE(inData->backgroundColor(), outData.backgroundColor());
}

void SerializerTest::testCornerRadius()
{
    MindMapData outData;
    outData.setCornerRadius(Constants::Node::DEFAULT_CORNER_RADIUS + 1);
    const auto inData = Serializer::fromXml(Serializer::toXml(outData));
    QCOMPARE(inData->cornerRadius(), outData.cornerRadius());
}

void SerializerTest::testEdgeColor()
{
    MindMapData outData;
    outData.setEdgeColor(QColor(1, 2, 3));
    const auto inData = Serializer::fromXml(Serializer::toXml(outData));
    QCOMPARE(inData->edgeColor(), outData.edgeColor());
}

void SerializerTest::testEdgeWidth()
{
    MindMapData outData;
    outData.setEdgeWidth(666.42);
    const auto inData = Serializer::fromXml(Serializer::toXml(outData));
    QCOMPARE(inData->edgeWidth(), outData.edgeWidth());
}

void SerializerTest::testNotUsedImages()
{
    MindMapData outData;
    outData.imageManager().addImage(Image {});
    outData.imageManager().addImage(Image {});
    const auto outXml = Serializer::toXml(outData);
    outData.imageManager().clear(); // ImageManager is a static class
    QCOMPARE(outData.imageManager().images().size(), size_t { 0 });
    const auto inData = Serializer::fromXml(outXml);
    // No nodes are using the added images, so nothing should have been serialized
    QCOMPARE(outData.imageManager().images().size(), size_t { 0 });
}

void SerializerTest::testUsedImages()
{
    MindMapData outData;
    const auto id1 = outData.imageManager().addImage(Image {});
    const auto id2 = outData.imageManager().addImage(Image {});
    auto node1 = std::make_shared<Node>();
    outData.graph().addNode(node1);
    node1->setImageRef(id1);
    auto node2 = std::make_shared<Node>();
    outData.graph().addNode(node2);
    node2->setImageRef(id2);

    const auto outXml = Serializer::toXml(outData);
    outData.imageManager().clear(); // ImageManager is a static class
    QCOMPARE(outData.imageManager().images().size(), size_t { 0 });
    const auto inData = Serializer::fromXml(outXml);
    QCOMPARE(outData.imageManager().images().size(), size_t { 2 });
}

void SerializerTest::testTextSize()
{
    MindMapData outData;
    outData.setTextSize(42);
    const auto inData = Serializer::fromXml(Serializer::toXml(outData));
    QCOMPARE(inData->textSize(), outData.textSize());
}

void SerializerTest::testNodeDeletion()
{
    MindMapData outData;

    auto outNode0 = std::make_shared<Node>();
    outData.graph().addNode(outNode0);

    auto outNode1 = std::make_shared<Node>();
    outData.graph().addNode(outNode1);

    auto outNode2 = std::make_shared<Node>();
    outData.graph().addNode(outNode2);

    outData.graph().addEdge(std::make_shared<Edge>(*outNode0, *outNode1));

    outData.graph().addEdge(std::make_shared<Edge>(*outNode0, *outNode2));

    outData.graph().deleteNode(outNode1->index()); // Delete node in between

    const auto inData = Serializer::fromXml(Serializer::toXml(outData));
    const auto edges = inData->graph().getEdgesFromNode(outNode0);
    QCOMPARE(edges.size(), static_cast<size_t>(1));
}

void SerializerTest::testSingleEdge()
{
    MindMapData outData;

    const auto outNode0 = std::make_shared<Node>();
    outData.graph().addNode(outNode0);

    const auto outNode1 = std::make_shared<Node>();
    outData.graph().addNode(outNode1);

    const auto edge = std::make_shared<Edge>(*outNode0, *outNode1);
    const QString text = "Lorem ipsum";
    edge->setText(text);
    edge->setReversed(true);
    edge->setArrowMode(Edge::ArrowMode::Double);
    outData.graph().addEdge(edge);

    const auto inData = Serializer::fromXml(Serializer::toXml(outData));
    const auto edges = inData->graph().getEdgesFromNode(outNode0);
    QCOMPARE(edges.size(), static_cast<size_t>(1));
    QCOMPARE((*edges.begin())->text(), edge->text());
    QCOMPARE((*edges.begin())->reversed(), edge->reversed());
    QCOMPARE((*edges.begin())->arrowMode(), edge->arrowMode());
}

void SerializerTest::testSingleNode()
{
    MindMapData outData;

    const auto outNode = std::make_shared<Node>();
    outNode->setColor(QColor(1, 2, 3));
    outNode->setImageRef(1);
    outNode->setLocation(QPointF(333.333, 666.666));
    outNode->setSize(QSize(123, 321));
    outNode->setText("Lorem ipsum");
    outNode->setTextColor(QColor(4, 5, 6));
    outData.graph().addNode(outNode);

    const auto inData = Serializer::fromXml(Serializer::toXml(outData));
    QVERIFY(inData->graph().numNodes() == 1);

    const auto node = inData->graph().getNode(0);
    QVERIFY(node != nullptr);
    QCOMPARE(node->index(), outNode->index());
    QCOMPARE(node->color(), outNode->color());
    QCOMPARE(node->imageRef(), outNode->imageRef());
    QCOMPARE(node->location(), outNode->location());
    QCOMPARE(node->size(), outNode->size());
    QCOMPARE(node->text(), outNode->text());
    QCOMPARE(node->textColor(), outNode->textColor());
}

QTEST_GUILESS_MAIN(SerializerTest)
