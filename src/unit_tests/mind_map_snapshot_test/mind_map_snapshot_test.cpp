// This file is part of Heimer.
// Copyright (C) 2026 Jussi Lind <jussi.lind@iki.fi>
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
#include "mind_map_snapshot_test.hpp"

#include "../../application/mind_map_snapshot.hpp"
#include "../../common/test_mode.hpp"
#include "../../domain/graph.hpp"
#include "../../domain/image.hpp"
#include "../../domain/mind_map_data.hpp"
#include "../../infra/io/alz_file_io.hpp"
#include "../../view/scene_items/edge.hpp"
#include "../../view/scene_items/node.hpp"

#include <QImage>
#include <QTest>

using SceneItems::Edge;
using SceneItems::Node;

MindMapSnapshotTest::MindMapSnapshotTest()
{
    TestMode::setEnabled(true);
}

void MindMapSnapshotTest::test_restore_shouldRestoreGraph()
{
    const auto mindMapData = std::make_shared<MindMapData>();
    const auto node0 = std::make_shared<Node>();
    node0->setText("Node 0");
    node0->setLocation({ 10, 20 });
    mindMapData->graph().addNode(node0);
    const auto node1 = std::make_shared<Node>();
    mindMapData->graph().addNode(node1);
    const auto edge = std::make_shared<Edge>(node0, node1);
    edge->setText("Edge");
    mindMapData->graph().addEdge(edge);
    const IO::AlzFileIO alzFileIO;

    const auto restored = MindMapSnapshot { alzFileIO, mindMapData }.restore(alzFileIO);

    QCOMPARE(restored->graph().nodeCount(), size_t { 2 });
    const auto restoredNode0 = restored->graph().getNode(node0->index());
    QVERIFY(restoredNode0);
    QCOMPARE(restoredNode0->text(), QString { "Node 0" });
    QCOMPARE(restoredNode0->location(), (QPointF { 10, 20 }));
    const auto edges = restored->graph().getEdgesFromNode(restoredNode0);
    QCOMPARE(edges.size(), size_t { 1 });
    QCOMPARE(edges.at(0)->text(), QString { "Edge" });
}

void MindMapSnapshotTest::test_restore_shouldRestoreImages()
{
    const auto mindMapData = std::make_shared<MindMapData>();
    QImage qImage { 2, 2, QImage::Format_ARGB32 };
    qImage.fill(Qt::red);
    const auto imageId = mindMapData->imageManager().addImage(Image { qImage, "red.png" });
    const auto node = std::make_shared<Node>();
    node->setImageRef(imageId);
    mindMapData->graph().addNode(node);
    const IO::AlzFileIO alzFileIO;

    const auto restored = MindMapSnapshot { alzFileIO, mindMapData }.restore(alzFileIO);

    QCOMPARE(restored->graph().getNode(node->index())->imageRef(), imageId);
    const auto image = restored->imageManager().getImage(imageId);
    QVERIFY(image.has_value());
    QCOMPARE(image->path(), std::string { "red.png" });
    QCOMPARE(image->image(), qImage);
}

void MindMapSnapshotTest::test_restore_shouldRestoreFileName()
{
    const auto mindMapData = std::make_shared<MindMapData>();
    mindMapData->setFileName("/tmp/foo.alz");
    const IO::AlzFileIO alzFileIO;

    const auto restored = MindMapSnapshot { alzFileIO, mindMapData }.restore(alzFileIO);

    QCOMPARE(restored->fileName(), QString { "/tmp/foo.alz" });
}

void MindMapSnapshotTest::test_restore_shouldRestoreVersions()
{
    // E.g. a mind map loaded from an old file
    const auto mindMapData = std::make_shared<MindMapData>();
    mindMapData->setApplicationVersion("1.2.3");
    mindMapData->setAlzFormatVersion(IO::AlzFormatVersion::V1);
    const IO::AlzFileIO alzFileIO;

    const auto restored = MindMapSnapshot { alzFileIO, mindMapData }.restore(alzFileIO);

    QCOMPARE(restored->applicationVersion(), QString { "1.2.3" });
    QVERIFY(restored->alzFormatVersion() == IO::AlzFormatVersion::V1);
}

void MindMapSnapshotTest::test_restore_shouldBeIndependentOfLaterChanges()
{
    const auto mindMapData = std::make_shared<MindMapData>();
    const auto node = std::make_shared<Node>();
    node->setText("Before");
    mindMapData->graph().addNode(node);
    const IO::AlzFileIO alzFileIO;
    const MindMapSnapshot snapshot { alzFileIO, mindMapData };

    node->setText("After");
    mindMapData->graph().addNode(std::make_shared<Node>());

    const auto restored = snapshot.restore(alzFileIO);
    QCOMPARE(restored->graph().nodeCount(), size_t { 1 });
    QCOMPARE(restored->graph().getNode(node->index())->text(), QString { "Before" });
}

void MindMapSnapshotTest::test_snapshot_shouldNotContainImageData()
{
    const IO::AlzFileIO alzFileIO;
    const auto emptyMindMapData = std::make_shared<MindMapData>();
    const auto emptySnapshotSize = MindMapSnapshot { alzFileIO, emptyMindMapData }.xmlSizeInBytes();
    const auto mindMapData = std::make_shared<MindMapData>();
    QImage qImage { 512, 512, QImage::Format_ARGB32 };
    qImage.fill(Qt::blue);
    const auto imageId = mindMapData->imageManager().addImage(Image { qImage, "blue.png" });
    const auto node = std::make_shared<Node>();
    node->setImageRef(imageId);
    mindMapData->graph().addNode(node);

    // Image data is never written in test mode, so disable it for the serialization
    TestMode::setEnabled(false);
    const auto snapshotSize = MindMapSnapshot { alzFileIO, mindMapData }.xmlSizeInBytes();
    TestMode::setEnabled(true);

    // Only the node itself should increase the size, not the image data
    QVERIFY(snapshotSize < emptySnapshotSize + 1024);
}

QTEST_GUILESS_MAIN(MindMapSnapshotTest)
