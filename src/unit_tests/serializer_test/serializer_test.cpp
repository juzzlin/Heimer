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

#include "alz_serializer.hpp"
#include "mind_map_data.hpp"
#include "test_mode.hpp"

SerializerTest::SerializerTest()
{
    TestMode::setEnabled(true);
}

void SerializerTest::testEmptyDesign()
{
    MindMapData outData;
    const auto inData = AlzSerializer::fromXml(AlzSerializer::toXml(outData));
    QCOMPARE(QString(inData->version()), QString(VERSION));
}

void SerializerTest::testArrowSize()
{
    MindMapData outData;
    outData.setArrowSize(42.42);
    const auto inData = AlzSerializer::fromXml(AlzSerializer::toXml(outData));
    QCOMPARE(inData->arrowSize(), outData.arrowSize());
}

void SerializerTest::testBackgroundColor()
{
    MindMapData outData;
    outData.setBackgroundColor(QColor(1, 2, 3));
    const auto inData = AlzSerializer::fromXml(AlzSerializer::toXml(outData));
    QCOMPARE(inData->backgroundColor(), outData.backgroundColor());
}

void SerializerTest::testCornerRadius()
{
    MindMapData outData;
    outData.setCornerRadius(Constants::Node::DEFAULT_CORNER_RADIUS + 1);
    const auto inData = AlzSerializer::fromXml(AlzSerializer::toXml(outData));
    QCOMPARE(inData->cornerRadius(), outData.cornerRadius());
}

void SerializerTest::testEdgeColor()
{
    MindMapData outData;
    outData.setEdgeColor(QColor(1, 2, 3));
    const auto inData = AlzSerializer::fromXml(AlzSerializer::toXml(outData));
    QCOMPARE(inData->edgeColor(), outData.edgeColor());
}

void SerializerTest::testEdgeWidth()
{
    MindMapData outData;
    outData.setEdgeWidth(666.42);
    const auto inData = AlzSerializer::fromXml(AlzSerializer::toXml(outData));
    QCOMPARE(inData->edgeWidth(), outData.edgeWidth());
}

void SerializerTest::testFontItalic()
{
    MindMapData outData;
    QFont font("Foobar", 42, 666, true);
    outData.changeFont(font);
    outData.setTextSize(24);
    const auto inData = AlzSerializer::fromXml(AlzSerializer::toXml(outData));
    QCOMPARE(inData->font().family(), outData.font().family());
    QCOMPARE(inData->font().italic(), true);
    QCOMPARE(inData->font().weight(), outData.font().weight());
    QCOMPARE(inData->textSize(), outData.textSize()); // Font shouldn't change text size
}

void SerializerTest::testFontNonItalic()
{
    MindMapData outData;
    QFont font;
    outData.changeFont(font);
    const auto inData = AlzSerializer::fromXml(AlzSerializer::toXml(outData));
    QCOMPARE(inData->font().italic(), false);
}

void SerializerTest::testFontBold()
{
    MindMapData outData;
    QFont font;
    font.setBold(true);
    outData.changeFont(font);
    const auto inData = AlzSerializer::fromXml(AlzSerializer::toXml(outData));
    QCOMPARE(inData->font().bold(), true);
}

void SerializerTest::testFontNonBold()
{
    MindMapData outData;
    QFont font;
    outData.changeFont(font);
    const auto inData = AlzSerializer::fromXml(AlzSerializer::toXml(outData));
    QCOMPARE(inData->font().bold(), false);
}

void SerializerTest::testFontOverline()
{
    MindMapData outData;
    QFont font;
    font.setOverline(true);
    outData.changeFont(font);
    const auto inData = AlzSerializer::fromXml(AlzSerializer::toXml(outData));
    QCOMPARE(inData->font().overline(), true);
}

void SerializerTest::testFontNonOverline()
{
    MindMapData outData;
    QFont font;
    outData.changeFont(font);
    const auto inData = AlzSerializer::fromXml(AlzSerializer::toXml(outData));
    QCOMPARE(inData->font().overline(), false);
}

void SerializerTest::testFontStrikeOut()
{
    MindMapData outData;
    QFont font;
    font.setStrikeOut(true);
    outData.changeFont(font);
    const auto inData = AlzSerializer::fromXml(AlzSerializer::toXml(outData));
    QCOMPARE(inData->font().strikeOut(), true);
}

void SerializerTest::testFontNonStrikeOut()
{
    MindMapData outData;
    QFont font;
    outData.changeFont(font);
    const auto inData = AlzSerializer::fromXml(AlzSerializer::toXml(outData));
    QCOMPARE(inData->font().strikeOut(), false);
}

void SerializerTest::testFontUnderline()
{
    MindMapData outData;
    QFont font("Foobar");
    font.setUnderline(true);
    outData.changeFont(font);
    const auto inData = AlzSerializer::fromXml(AlzSerializer::toXml(outData));
    QCOMPARE(inData->font().underline(), true);
}

void SerializerTest::testFontNonUnderline()
{
    MindMapData outData;
    QFont font("Foobar");
    outData.changeFont(font);
    const auto inData = AlzSerializer::fromXml(AlzSerializer::toXml(outData));
    QCOMPARE(inData->font().underline(), false);
}

void SerializerTest::testGridColor()
{
    MindMapData outData;
    outData.setGridColor(QColor(1, 2, 3));
    const auto inData = AlzSerializer::fromXml(AlzSerializer::toXml(outData));
    QCOMPARE(inData->gridColor(), outData.gridColor());
}

void SerializerTest::testLayoutOptimizer()
{
    MindMapData outData;
    outData.setAspectRatio(3.14);
    outData.setMinEdgeLength(42.666);
    const auto inData = AlzSerializer::fromXml(AlzSerializer::toXml(outData));
    QCOMPARE(inData->aspectRatio(), outData.aspectRatio());
    QCOMPARE(inData->minEdgeLength(), outData.minEdgeLength());
}

void SerializerTest::testNotUsedImages()
{
    MindMapData outData;
    outData.imageManager().addImage(Image {});
    outData.imageManager().addImage(Image {});
    const auto outXml = AlzSerializer::toXml(outData);
    outData.imageManager().clear(); // ImageManager is a static class
    QCOMPARE(outData.imageManager().images().size(), size_t { 0 });
    const auto inData = AlzSerializer::fromXml(outXml);
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

    const auto outXml = AlzSerializer::toXml(outData);
    outData.imageManager().clear(); // ImageManager is a static class
    QCOMPARE(outData.imageManager().images().size(), size_t { 0 });
    const auto inData = AlzSerializer::fromXml(outXml);
    QCOMPARE(outData.imageManager().images().size(), size_t { 2 });
}

void SerializerTest::testTextSize()
{
    MindMapData outData;
    outData.setTextSize(42);
    const auto inData = AlzSerializer::fromXml(AlzSerializer::toXml(outData));
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

    outData.graph().addEdge(std::make_shared<Edge>(outNode0, outNode1));

    outData.graph().addEdge(std::make_shared<Edge>(outNode0, outNode2));

    outData.graph().deleteNode(outNode1->index()); // Delete node in between

    const auto inData = AlzSerializer::fromXml(AlzSerializer::toXml(outData));
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

    const auto edge = std::make_shared<Edge>(outNode0, outNode1);
    const QString text = "Lorem ipsum";
    edge->setArrowMode(Edge::ArrowMode::Double);
    edge->setDashedLine(true);
    edge->setReversed(true);
    edge->setText(text);
    outData.graph().addEdge(edge);

    const auto inData = AlzSerializer::fromXml(AlzSerializer::toXml(outData));
    const auto edges = inData->graph().getEdgesFromNode(outNode0);
    QCOMPARE(edges.size(), static_cast<size_t>(1));
    QCOMPARE((*edges.begin())->arrowMode(), edge->arrowMode());
    QCOMPARE((*edges.begin())->dashedLine(), edge->dashedLine());
    QCOMPARE((*edges.begin())->reversed(), edge->reversed());
    QCOMPARE((*edges.begin())->text(), edge->text());
}

void SerializerTest::testSingleNode()
{
    MindMapData outData;

    const auto outNode = std::make_shared<Node>();
    outNode->setColor(QColor(1, 2, 3));
    const auto imageRef = outData.imageManager().addImage(Image {});
    outNode->setImageRef(imageRef);
    outNode->setLocation(QPointF(333.333, 666.666));
    outNode->setSize(QSize(123, 321));
    outNode->setText("Lorem ipsum");
    outNode->setTextColor(QColor(4, 5, 6));
    outData.graph().addNode(outNode);

    const auto inData = AlzSerializer::fromXml(AlzSerializer::toXml(outData));
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
