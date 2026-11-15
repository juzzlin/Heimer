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

#include "item_filter_test.hpp"

#include "../../application/service_container.hpp"
#include "../../application/settings_proxy.hpp"
#include "../../view/item_filter.hpp"

#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsView>
#include <QTest>

#include <memory>

using SceneItems::Edge;
using SceneItems::EdgeTextEdit;
using SceneItems::Node;
using SceneItems::NodeHandle;

namespace {

const double tolerance = 2.0;

//! Two nodes connected with a labeled edge. Test mode is not enabled, so that the
//! items have the same shapes as in the application.
struct TestScene
{
    TestScene()
    {
        node0->setIndex(0);
        node0->setPos({ 0, 0 });
        scene.addItem(node0.get());
        node1->setIndex(1);
        node1->setPos({ 600, 0 });
        scene.addItem(node1.get());
        edge->setText("Edge");
        scene.addItem(edge.get());
        edge->updateLine();
    }

    ~TestScene()
    {
        for (const auto & item : scene.items()) {
            if (!item->parentItem()) {
                scene.removeItem(item);
            }
        }
    }

    QGraphicsScene scene;
    const std::shared_ptr<Node> node0 = std::make_shared<Node>();
    const std::shared_ptr<Node> node1 = std::make_shared<Node>();
    const std::shared_ptr<Edge> edge = std::make_shared<Edge>(node0, node1);
};

} // namespace

void ItemFilterTest::test_getFirstItemAtPosition_shouldFindNothingOnBackground()
{
    TestScene testScene;

    const auto result = ItemFilter::getFirstItemAtPosition(testScene.scene, { 300, 500 }, tolerance);

    QVERIFY(!result.hasItem());
    QVERIFY(!result.is<Node>());
}

void ItemFilterTest::test_getFirstItemAtPosition_shouldFindNode()
{
    TestScene testScene;

    const auto result = ItemFilter::getFirstItemAtPosition(testScene.scene, testScene.node1->pos(), tolerance);

    QVERIFY(result.is<Node>());
    QVERIFY(!result.is<Edge>());
    QCOMPARE(result.get<Node>(), testScene.node1.get());
}

void ItemFilterTest::test_getFirstItemAtPosition_shouldFindNodeWithinTolerance()
{
    TestScene testScene;
    const auto body = testScene.node0->sceneBodyRect();
    const QPointF justBelow { body.center().x(), body.bottom() + tolerance / 2 };

    const auto result = ItemFilter::getFirstItemAtPosition(testScene.scene, justBelow, tolerance);

    QVERIFY(result.is<Node>());
    QCOMPARE(result.get<Node>(), testScene.node0.get());
    QVERIFY(!ItemFilter::getFirstItemAtPosition(testScene.scene, justBelow, 0.1).hasItem());
}

void ItemFilterTest::test_getFirstItemAtPosition_shouldFindEdgeText()
{
    TestScene testScene;

    // The label is in the middle of the edge, slightly below the line
    const QPointF onLabel { (testScene.node0->pos().x() + testScene.node1->pos().x()) / 2, 8 };

    // Wait for the label to fade in
    QTRY_VERIFY(ItemFilter::getFirstItemAtPosition(testScene.scene, onLabel, tolerance).is<EdgeTextEdit>());

    const auto result = ItemFilter::getFirstItemAtPosition(testScene.scene, onLabel, tolerance);
    QCOMPARE(result.get<EdgeTextEdit>()->edge(), testScene.edge.get());
}

void ItemFilterTest::test_getFirstItemAtPosition_shouldFindNodeHandle()
{
    SC::instance().settingsProxy()->setRaiseNodeOnMouseHover(false);
    TestScene testScene;
    QGraphicsView view { &testScene.scene };
    view.resize(800, 400);
    view.show();
    QGraphicsSceneHoverEvent hoverEvent { QEvent::GraphicsSceneHoverEnter };
    testScene.scene.sendEvent(testScene.node0.get(), &hoverEvent);
    view.grab(); // Adds the handles to the scene

    NodeHandle * handle = nullptr;
    for (const auto & item : testScene.scene.items()) {
        if (const auto nodeHandle = dynamic_cast<NodeHandle *>(item); nodeHandle && nodeHandle->role() == NodeHandle::Role::ConnectOrCreate) {
            handle = nodeHandle;
        }
    }
    QVERIFY(handle);
    QTRY_VERIFY(!handle->sceneBoundingRect().isEmpty()); // Wait for the appear animation

    const auto result = ItemFilter::getFirstItemAtPosition(testScene.scene, handle->pos(), tolerance);

    QVERIFY(result.is<NodeHandle>());
    QCOMPARE(result.get<NodeHandle>(), handle);
}

QTEST_MAIN(ItemFilterTest)
