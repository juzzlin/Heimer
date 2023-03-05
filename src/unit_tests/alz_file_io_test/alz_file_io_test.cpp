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

#include "alz_file_io_test.hpp"

#include "../../image_manager.hpp"

#include "../../core/mind_map_data.hpp"
#include "../../core/test_mode.hpp"

#include "../../io/alz_file_io.hpp"
#include "../../io/alz_file_io_version.hpp"
#include "../../io/xml_reader.hpp"
#include "../../io/xml_writer.hpp"

#include "../../core/graph.hpp"

using Core::MindMapData;

using SceneItems::Edge;
using SceneItems::EdgeModel;
using SceneItems::Node;
using SceneItems::NodeModel;

AlzFileIOTest::AlzFileIOTest()
{
    Core::TestMode::setEnabled(true);
}

void AlzFileIOTest::testEmptyDesign()
{
    const auto outData = std::make_shared<MindMapData>();
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO().toXml(outData));
    QCOMPARE(QString(inData->applicationVersion()), QString(VERSION));
}

void AlzFileIOTest::testStyle_ArrowSize()
{
    const auto outData = std::make_shared<MindMapData>();
    outData->setArrowSize(42.42);
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO().toXml(outData));
    QCOMPARE(inData->arrowSize(), outData->arrowSize());
}

void AlzFileIOTest::testStyle_BackgroundColor()
{
    const auto outData = std::make_shared<MindMapData>();
    outData->setBackgroundColor(QColor(1, 2, 3));
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO().toXml(outData));
    QCOMPARE(inData->backgroundColor(), outData->backgroundColor());
}

void AlzFileIOTest::testStyle_CornerRadius()
{
    const auto outData = std::make_shared<MindMapData>();
    outData->setCornerRadius(Constants::Node::Defaults::CORNER_RADIUS + 1);
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO().toXml(outData));
    QCOMPARE(inData->cornerRadius(), outData->cornerRadius());
}

void AlzFileIOTest::testStyle_EdgeColor()
{
    const auto outData = std::make_shared<MindMapData>();
    outData->setEdgeColor(QColor(1, 2, 3));
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO().toXml(outData));
    QCOMPARE(inData->edgeColor(), outData->edgeColor());
}

void AlzFileIOTest::testStyle_EdgeWidth()
{
    const auto outData = std::make_shared<MindMapData>();
    outData->setEdgeWidth(666.42);
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO().toXml(outData));
    QCOMPARE(inData->edgeWidth(), outData->edgeWidth());
}

void AlzFileIOTest::testStyle_FontItalic()
{
    const auto outData = std::make_shared<MindMapData>();
    QFont font("Foobar", 42, 666, true);
    outData->changeFont(font);
    outData->setTextSize(24);
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO().toXml(outData));
    QCOMPARE(inData->font().family(), outData->font().family());
    QCOMPARE(inData->font().italic(), true);
    QCOMPARE(inData->font().weight(), outData->font().weight());
    QCOMPARE(inData->textSize(), outData->textSize()); // Font shouldn't change text size
}

void AlzFileIOTest::testStyle_FontNonItalic()
{
    const auto outData = std::make_shared<MindMapData>();
    QFont font;
    outData->changeFont(font);
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO().toXml(outData));
    QCOMPARE(inData->font().italic(), false);
}

void AlzFileIOTest::testStyle_FontBold()
{
    const auto outData = std::make_shared<MindMapData>();
    QFont font;
    font.setBold(true);
    outData->changeFont(font);
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO().toXml(outData));
    QCOMPARE(inData->font().bold(), true);
}

void AlzFileIOTest::testStyle_FontNonBold()
{
    const auto outData = std::make_shared<MindMapData>();
    QFont font;
    outData->changeFont(font);
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO().toXml(outData));
    QCOMPARE(inData->font().bold(), false);
}

void AlzFileIOTest::testStyle_FontOverline()
{
    const auto outData = std::make_shared<MindMapData>();
    QFont font;
    font.setOverline(true);
    outData->changeFont(font);
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO().toXml(outData));
    QCOMPARE(inData->font().overline(), true);
}

void AlzFileIOTest::testStyle_FontNonOverline()
{
    const auto outData = std::make_shared<MindMapData>();
    QFont font;
    outData->changeFont(font);
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO().toXml(outData));
    QCOMPARE(inData->font().overline(), false);
}

void AlzFileIOTest::testStyle_FontStrikeOut()
{
    const auto outData = std::make_shared<MindMapData>();
    QFont font;
    font.setStrikeOut(true);
    outData->changeFont(font);
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO().toXml(outData));
    QCOMPARE(inData->font().strikeOut(), true);
}

void AlzFileIOTest::testStyle_FontNonStrikeOut()
{
    const auto outData = std::make_shared<MindMapData>();
    QFont font;
    outData->changeFont(font);
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO().toXml(outData));
    QCOMPARE(inData->font().strikeOut(), false);
}

void AlzFileIOTest::testStyle_FontUnderline()
{
    const auto outData = std::make_shared<MindMapData>();
    QFont font("Foobar");
    font.setUnderline(true);
    outData->changeFont(font);
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO().toXml(outData));
    QCOMPARE(inData->font().underline(), true);
}

void AlzFileIOTest::testStyle_FontNonUnderline()
{
    const auto outData = std::make_shared<MindMapData>();
    QFont font("Foobar");
    outData->changeFont(font);
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO().toXml(outData));
    QCOMPARE(inData->font().underline(), false);
}

void AlzFileIOTest::testStyle_GridColor()
{
    const auto outData = std::make_shared<MindMapData>();
    outData->setGridColor(QColor(1, 2, 3));
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO().toXml(outData));
    QCOMPARE(inData->gridColor(), outData->gridColor());
}

void AlzFileIOTest::testStyle_TextSize()
{
    const auto outData = std::make_shared<MindMapData>();
    outData->setTextSize(42);
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO().toXml(outData));
    QCOMPARE(inData->textSize(), outData->textSize());
}

void AlzFileIOTest::testMetadata_LayoutOptimizer()
{
    const auto outData = std::make_shared<MindMapData>();
    outData->setAspectRatio(3.14);
    outData->setMinEdgeLength(42.666);
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO().toXml(outData));
    QCOMPARE(inData->aspectRatio(), outData->aspectRatio());
    QCOMPARE(inData->minEdgeLength(), outData->minEdgeLength());
}

void AlzFileIOTest::testLoadJpg()
{
    const QString xml =
      "<?xml version='1.0' encoding='UTF-8'?>"
      "<heimer-mind-map version=\"3.6.1\">"
      " <graph>"
      "  <node w=\"200000\" x=\"0\" y=\"0\" h=\"75000\" index=\"0\">"
      "   <image ref=\"1\"/>"
      "  </node>"
      " </graph>"
      " <image path=\"test.jpg\" id=\"1\">/9j/4AAQSkZJRgABAQEASABIAAD//gAgRGVzY3JpcHRpb246IENyZWF0ZWQgd2l0aCBHSU1Q/9sAQwAIBgYHBgUIBwcHCQkICgwUDQwLCwwZEhMPFB0aHx4dGhwcICQuJyAiLCMcHCg3KSwwMTQ0NB8nOT04MjwuMzQy/9sAQwEJCQkMCwwYDQ0YMiEcITIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIy/8AAEQgABAAEAwEiAAIRAQMRAf/EAB8AAAEFAQEBAQEBAAAAAAAAAAABAgMEBQYHCAkKC//EALUQAAIBAwMCBAMFBQQEAAABfQECAwAEEQUSITFBBhNRYQcicRQygZGhCCNCscEVUtHwJDNicoIJChYXGBkaJSYnKCkqNDU2Nzg5OkNERUZHSElKU1RVVldYWVpjZGVmZ2hpanN0dXZ3eHl6g4SFhoeIiYqSk5SVlpeYmZqio6Slpqeoqaqys7S1tre4ubrCw8TFxsfIycrS09TV1tfY2drh4uPk5ebn6Onq8fLz9PX29/j5+v/EAB8BAAMBAQEBAQEBAQEAAAAAAAABAgMEBQYHCAkKC//EALURAAIBAgQEAwQHBQQEAAECdwABAgMRBAUhMQYSQVEHYXETIjKBCBRCkaGxwQkjM1LwFWJy0QoWJDThJfEXGBkaJicoKSo1Njc4OTpDREVGR0hJSlNUVVZXWFlaY2RlZmdoaWpzdHV2d3h5eoKDhIWGh4iJipKTlJWWl5iZmqKjpKWmp6ipqrKztLW2t7i5usLDxMXGx8jJytLT1NXW19jZ2uLj5OXm5+jp6vLz9PX29/j5+v/aAAwDAQACEQMRAD8A9/ooooA//9k=</image>"
      "</heimer-mind-map>";
    const auto inData = IO::AlzFileIO().fromXml(xml);
    QCOMPARE(inData->imageManager().images().size(), size_t { 1 });
    const auto image = inData->imageManager().getImage(1);
    QVERIFY(image.has_value());
    QCOMPARE(image->image().width(), 4);
    QCOMPARE(image->image().height(), 4);
    QCOMPARE(image->id(), size_t(1));
    QCOMPARE(image->path(), std::string("test.jpg"));
}

void AlzFileIOTest::testLoadPng()
{
    const QString xml =
      "<?xml version='1.0' encoding='UTF-8'?>"
      "<heimer-mind-map version=\"3.6.1\">"
      " <graph>"
      "  <node w=\"200000\" x=\"0\" y=\"0\" h=\"75000\" index=\"0\">"
      "   <image ref=\"1\"/>"
      "  </node>"
      " </graph>"
      " <image path=\"test.png\" id=\"1\">iVBORw0KGgoAAAANSUhEUgAAAAQAAAAECAIAAAAmkwkpAAAACXBIWXMAAAsTAAALEwEAmpwYAAAAGXRFWHRDb21tZW50AENyZWF0ZWQgd2l0aCBHSU1QV4EOFwAAABRJREFUCJlj/P//PwMMMDEgAdwcAJZuAwUDbWh7AAAAAElFTkSuQmCC</image>"
      "</heimer-mind-map>";
    const auto inData = IO::AlzFileIO().fromXml(xml);
    QCOMPARE(inData->imageManager().images().size(), size_t { 1 });
    const auto image = inData->imageManager().getImage(1);
    QVERIFY(image.has_value());
    QCOMPARE(image->image().width(), 4);
    QCOMPARE(image->image().height(), 4);
    QCOMPARE(image->id(), size_t(1));
    QCOMPARE(image->path(), std::string("test.png"));
}

void AlzFileIOTest::testNotUsedImages()
{
    const auto outData = std::make_shared<MindMapData>();
    outData->imageManager().addImage(Image {});
    outData->imageManager().addImage(Image {});
    const auto outXml = IO::AlzFileIO().toXml(outData);
    QCOMPARE(outData->imageManager().images().size(), size_t { 2 });
    const auto inData = IO::AlzFileIO().fromXml(outXml);
    // No nodes are using the added images, so nothing should have been serialized
    QCOMPARE(inData->imageManager().images().size(), size_t { 0 });
}

void AlzFileIOTest::testUsedImages()
{
    const auto outData = std::make_shared<MindMapData>();
    const auto id1 = outData->imageManager().addImage(Image {});
    const auto id2 = outData->imageManager().addImage(Image {});
    auto node1 = std::make_shared<Node>();
    outData->graph().addNode(node1);
    node1->setImageRef(id1);
    auto node2 = std::make_shared<Node>();
    outData->graph().addNode(node2);
    node2->setImageRef(id2);

    const auto outXml = IO::AlzFileIO().toXml(outData);
    QCOMPARE(outData->imageManager().images().size(), size_t { 2 });
    const auto inData = IO::AlzFileIO().fromXml(outXml);
    QCOMPARE(inData->imageManager().images().size(), size_t { 2 });
}

void AlzFileIOTest::testGraph_NodeDeletion()
{
    const auto outData = std::make_shared<MindMapData>();

    auto outNode0 = std::make_shared<Node>();
    outData->graph().addNode(outNode0);

    auto outNode1 = std::make_shared<Node>();
    outData->graph().addNode(outNode1);

    auto outNode2 = std::make_shared<Node>();
    outData->graph().addNode(outNode2);

    outData->graph().addEdge(std::make_shared<Edge>(outNode0, outNode1));

    outData->graph().addEdge(std::make_shared<Edge>(outNode0, outNode2));

    outData->graph().deleteNode(outNode1->index()); // Delete node in between

    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO().toXml(outData));
    const auto edges = inData->graph().getEdgesFromNode(outNode0);
    QCOMPARE(edges.size(), static_cast<size_t>(1));
}

void AlzFileIOTest::testGraph_SingleEdge()
{
    const auto outData = std::make_shared<MindMapData>();
    const auto outNode0 = std::make_shared<Node>();
    outData->graph().addNode(outNode0);
    const auto outNode1 = std::make_shared<Node>();
    outData->graph().addNode(outNode1);

    const auto edge = std::make_shared<Edge>(outNode0, outNode1);
    const QString text = "Lorem ipsum";
    edge->setArrowMode(EdgeModel::ArrowMode::Double);
    edge->setDashedLine(true);
    edge->setReversed(true);
    edge->setText(text);
    outData->graph().addEdge(edge);

    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO().toXml(outData));
    const auto edges = inData->graph().getEdgesFromNode(outNode0);
    QCOMPARE(edges.size(), static_cast<size_t>(1));
    QCOMPARE(edges.at(0)->arrowMode(), edge->arrowMode());
    QCOMPARE(edges.at(0)->dashedLine(), edge->dashedLine());
    QCOMPARE(edges.at(0)->reversed(), edge->reversed());
    QCOMPARE(edges.at(0)->text(), edge->text());
}

void AlzFileIOTest::testGraph_SingleNode()
{
    const auto outData = std::make_shared<MindMapData>();

    const auto outNode = std::make_shared<Node>();
    outNode->setColor(QColor(1, 2, 3));
    const auto imageRef = outData->imageManager().addImage(Image {});
    outNode->setImageRef(imageRef);
    outNode->setLocation(QPointF(333.333, 666.666));
    outNode->setSize(QSize(123, 321));
    outNode->setText("Lorem ipsum");
    outNode->setTextColor(QColor(4, 5, 6));
    outData->graph().addNode(outNode);

    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO().toXml(outData));
    QVERIFY(inData->graph().nodeCount() == 1);

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

void AlzFileIOTest::testV1_ArrowSize()
{
    const auto outData = std::make_shared<MindMapData>();
    outData->setArrowSize(42.42);
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO(IO::AlzFormatVersion::V1).toXml(outData));
    QVERIFY(inData->alzFormatVersion() == IO::AlzFormatVersion::V1);
    QCOMPARE(inData->arrowSize(), outData->arrowSize());
}

void AlzFileIOTest::testV1_BackgroundColor()
{
    const auto outData = std::make_shared<MindMapData>();
    outData->setBackgroundColor(QColor(1, 2, 3));
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO(IO::AlzFormatVersion::V1).toXml(outData));
    QVERIFY(inData->alzFormatVersion() == IO::AlzFormatVersion::V1);
    QCOMPARE(inData->backgroundColor(), outData->backgroundColor());
}

void AlzFileIOTest::testV1_CornerRadius()
{
    const auto outData = std::make_shared<MindMapData>();
    outData->setCornerRadius(Constants::Node::Defaults::CORNER_RADIUS + 1);
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO(IO::AlzFormatVersion::V1).toXml(outData));
    QVERIFY(inData->alzFormatVersion() == IO::AlzFormatVersion::V1);
    QCOMPARE(inData->cornerRadius(), outData->cornerRadius());
}

void AlzFileIOTest::testV1_EdgeColor()
{
    const auto outData = std::make_shared<MindMapData>();
    outData->setEdgeColor(QColor(1, 2, 3));
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO(IO::AlzFormatVersion::V1).toXml(outData));
    QVERIFY(inData->alzFormatVersion() == IO::AlzFormatVersion::V1);
    QCOMPARE(inData->edgeColor(), outData->edgeColor());
}

void AlzFileIOTest::testV1_EdgeWidth()
{
    const auto outData = std::make_shared<MindMapData>();
    outData->setEdgeWidth(666.42);
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO(IO::AlzFormatVersion::V1).toXml(outData));
    QVERIFY(inData->alzFormatVersion() == IO::AlzFormatVersion::V1);
    QCOMPARE(inData->edgeWidth(), outData->edgeWidth());
}

void AlzFileIOTest::testV1_FontItalic()
{
    const auto outData = std::make_shared<MindMapData>();
    QFont font("Foobar", 42, 666, true);
    outData->changeFont(font);
    outData->setTextSize(24);
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO(IO::AlzFormatVersion::V1).toXml(outData));
    QVERIFY(inData->alzFormatVersion() == IO::AlzFormatVersion::V1);
    QCOMPARE(inData->font().family(), outData->font().family());
    QCOMPARE(inData->font().italic(), true);
    QCOMPARE(inData->font().weight(), outData->font().weight());
    QCOMPARE(inData->textSize(), outData->textSize()); // Font shouldn't change text size
}

void AlzFileIOTest::testV1_FontNonItalic()
{
    const auto outData = std::make_shared<MindMapData>();
    QFont font;
    outData->changeFont(font);
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO(IO::AlzFormatVersion::V1).toXml(outData));
    QVERIFY(inData->alzFormatVersion() == IO::AlzFormatVersion::V1);
    QCOMPARE(inData->font().italic(), false);
}

void AlzFileIOTest::testV1_FontBold()
{
    const auto outData = std::make_shared<MindMapData>();
    QFont font;
    font.setBold(true);
    outData->changeFont(font);
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO(IO::AlzFormatVersion::V1).toXml(outData));
    QVERIFY(inData->alzFormatVersion() == IO::AlzFormatVersion::V1);
    QCOMPARE(inData->font().bold(), true);
}

void AlzFileIOTest::testV1_FontNonBold()
{
    const auto outData = std::make_shared<MindMapData>();
    QFont font;
    outData->changeFont(font);
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO(IO::AlzFormatVersion::V1).toXml(outData));
    QVERIFY(inData->alzFormatVersion() == IO::AlzFormatVersion::V1);
    QCOMPARE(inData->font().bold(), false);
}

void AlzFileIOTest::testV1_FontOverline()
{
    const auto outData = std::make_shared<MindMapData>();
    QFont font;
    font.setOverline(true);
    outData->changeFont(font);
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO(IO::AlzFormatVersion::V1).toXml(outData));
    QVERIFY(inData->alzFormatVersion() == IO::AlzFormatVersion::V1);
    QCOMPARE(inData->font().overline(), true);
}

void AlzFileIOTest::testV1_FontNonOverline()
{
    const auto outData = std::make_shared<MindMapData>();
    QFont font;
    outData->changeFont(font);
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO(IO::AlzFormatVersion::V1).toXml(outData));
    QVERIFY(inData->alzFormatVersion() == IO::AlzFormatVersion::V1);
    QCOMPARE(inData->font().overline(), false);
}

void AlzFileIOTest::testV1_FontStrikeOut()
{
    const auto outData = std::make_shared<MindMapData>();
    QFont font;
    font.setStrikeOut(true);
    outData->changeFont(font);
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO(IO::AlzFormatVersion::V1).toXml(outData));
    QVERIFY(inData->alzFormatVersion() == IO::AlzFormatVersion::V1);
    QCOMPARE(inData->font().strikeOut(), true);
}

void AlzFileIOTest::testV1_FontNonStrikeOut()
{
    const auto outData = std::make_shared<MindMapData>();
    QFont font;
    outData->changeFont(font);
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO(IO::AlzFormatVersion::V1).toXml(outData));
    QVERIFY(inData->alzFormatVersion() == IO::AlzFormatVersion::V1);
    QCOMPARE(inData->font().strikeOut(), false);
}

void AlzFileIOTest::testV1_FontUnderline()
{
    const auto outData = std::make_shared<MindMapData>();
    QFont font("Foobar");
    font.setUnderline(true);
    outData->changeFont(font);
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO(IO::AlzFormatVersion::V1).toXml(outData));
    QVERIFY(inData->alzFormatVersion() == IO::AlzFormatVersion::V1);
    QCOMPARE(inData->font().underline(), true);
}

void AlzFileIOTest::testV1_FontNonUnderline()
{
    const auto outData = std::make_shared<MindMapData>();
    QFont font("Foobar");
    outData->changeFont(font);
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO(IO::AlzFormatVersion::V1).toXml(outData));
    QVERIFY(inData->alzFormatVersion() == IO::AlzFormatVersion::V1);
    QCOMPARE(inData->font().underline(), false);
}

void AlzFileIOTest::testV1_GridColor()
{
    const auto outData = std::make_shared<MindMapData>();
    outData->setGridColor(QColor(1, 2, 3));
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO(IO::AlzFormatVersion::V1).toXml(outData));
    QVERIFY(inData->alzFormatVersion() == IO::AlzFormatVersion::V1);
    QCOMPARE(inData->gridColor(), outData->gridColor());
}

void AlzFileIOTest::testV1_TextSize()
{
    const auto outData = std::make_shared<MindMapData>();
    outData->setTextSize(42);
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO(IO::AlzFormatVersion::V1).toXml(outData));
    QVERIFY(inData->alzFormatVersion() == IO::AlzFormatVersion::V1);
    QCOMPARE(inData->textSize(), outData->textSize());
}

void AlzFileIOTest::testV1_LayoutOptimizer()
{
    const auto outData = std::make_shared<MindMapData>();
    outData->setAspectRatio(3.14);
    outData->setMinEdgeLength(42.666);
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO(IO::AlzFormatVersion::V1).toXml(outData));
    QVERIFY(inData->alzFormatVersion() == IO::AlzFormatVersion::V1);
    QCOMPARE(inData->aspectRatio(), outData->aspectRatio());
    QCOMPARE(inData->minEdgeLength(), outData->minEdgeLength());
}

void AlzFileIOTest::testV1_Graph_SingleEdge()
{
    const auto outData = std::make_shared<MindMapData>();
    const auto outNode0 = std::make_shared<Node>();
    outData->graph().addNode(outNode0);
    const auto outNode1 = std::make_shared<Node>();
    outData->graph().addNode(outNode1);

    const auto edge = std::make_shared<Edge>(outNode0, outNode1);
    outData->graph().addEdge(edge);

    // Only the index attributes have changed in V1 => V2
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO(IO::AlzFormatVersion::V1).toXml(outData));
    QVERIFY(inData->alzFormatVersion() == IO::AlzFormatVersion::V1);
    const auto edges = inData->graph().getEdgesFromNode(outNode0);
    QCOMPARE(edges.size(), static_cast<size_t>(1));
    QCOMPARE(edges.at(0)->sourceNode().index(), outNode0->index());
    QCOMPARE(edges.at(0)->targetNode().index(), outNode1->index());
}

void AlzFileIOTest::testV1_Graph_SingleNode()
{
    const auto outData = std::make_shared<MindMapData>();

    const auto outNode = std::make_shared<Node>();
    outData->graph().addNode(outNode);

    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO(IO::AlzFormatVersion::V1).toXml(outData));
    QVERIFY(inData->alzFormatVersion() == IO::AlzFormatVersion::V1);
    QVERIFY(inData->graph().nodeCount() == 1);

    // Only the index attribute has changed in V1 => V2
    const auto node = inData->graph().getNode(0);
    QVERIFY(node != nullptr);
    QCOMPARE(node->index(), outNode->index());
}

void AlzFileIOTest::testV1_Version()
{
    const auto outData = std::make_shared<MindMapData>();
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO(IO::AlzFormatVersion::V1).toXml(outData));

    QVERIFY(inData->applicationVersion() == Constants::Application::APPLICATION_VERSION);
    QVERIFY(inData->alzFormatVersion() == IO::AlzFormatVersion::V1);
}

void AlzFileIOTest::testV2_Version()
{
    const auto outData = std::make_shared<MindMapData>();
    const auto inData = IO::AlzFileIO().fromXml(IO::AlzFileIO().toXml(outData));

    QVERIFY(inData->applicationVersion() == Constants::Application::APPLICATION_VERSION);
    QVERIFY(inData->alzFormatVersion() == Constants::Application::ALZ_FORMAT_VERSION);
}

QTEST_GUILESS_MAIN(AlzFileIOTest)
