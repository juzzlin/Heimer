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

#include "magic_zoom_test.hpp"

#include "../../common/test_mode.hpp"
#include "../../view/magic_zoom.hpp"
#include "../../view/scene_items/edge.hpp"
#include "../../view/scene_items/node.hpp"

#include <QGraphicsRectItem>
#include <QTest>

#include <memory>

using std::make_shared;

using SceneItems::Edge;
using SceneItems::Node;

namespace {

const int margin = 60;

QRectF nodeRect(const Node & node)
{
    return node.placementBoundingRect().translated(node.pos());
}

std::shared_ptr<Node> createNode(QPointF pos)
{
    const auto node = make_shared<Node>();
    node->setPos(pos);
    return node;
}

} // namespace

MagicZoomTest::MagicZoomTest()
{
    TestMode::setEnabled(true);
}

void MagicZoomTest::test_calculateRectangle_shouldBeEmptyWithoutItems()
{
    QVERIFY(MagicZoom::calculateRectangleByNodes({}).isNull());
    QVERIFY(MagicZoom::calculateRectangleByNodes({}, true).isNull());
    QVERIFY(MagicZoom::calculateRectangleByEdges({}).isNull());
    QVERIFY(MagicZoom::calculateRectangleByItems({}).isNull());
}

void MagicZoomTest::test_calculateRectangleByNodes_shouldContainNodesWithMargin()
{
    const auto node0 = createNode({ -200, -100 });
    const auto node1 = createNode({ 300, 400 });

    const auto rect = MagicZoom::calculateRectangleByNodes({ node0.get(), node1.get() });

    const auto withMargin = nodeRect(*node0).united(nodeRect(*node1)).adjusted(-margin, -margin, margin, margin);
    QVERIFY(rect.contains(withMargin));
    // The zoom area is expanded symmetrically around the nodes
    QCOMPARE(rect.center(), withMargin.center());
}

void MagicZoomTest::test_calculateRectangleByNodes_exportShouldHaveFixedMargin()
{
    const auto node0 = createNode({ -200, -100 });
    const auto node1 = createNode({ 300, 400 });

    const auto rect = MagicZoom::calculateRectangleByNodes({ node0.get(), node1.get() }, true);

    QCOMPARE(rect, nodeRect(*node0).united(nodeRect(*node1)).adjusted(-margin, -margin, margin, margin));
}

void MagicZoomTest::test_calculateRectangleByItems_shouldIgnoreOtherItems()
{
    const auto node0 = createNode({ 0, 0 });
    const auto node1 = createNode({ 100, 200 });
    QGraphicsRectItem otherItem { -5000, -5000, 10, 10 };

    const auto rect = MagicZoom::calculateRectangleByItems({ node0.get(), &otherItem, node1.get() });

    QCOMPARE(rect, MagicZoom::calculateRectangleByNodes({ node0.get(), node1.get() }));
    QCOMPARE(MagicZoom::calculateRectangleByItems({ node0.get(), &otherItem, node1.get() }, true),
             MagicZoom::calculateRectangleByNodes({ node0.get(), node1.get() }, true));
}

void MagicZoomTest::test_calculateRectangleByEdges_shouldContainLabels()
{
    const auto node0 = createNode({ 0, 0 });
    const auto node1 = createNode({ 400, 0 });
    const auto node2 = createNode({ 0, 400 });
    const auto edge0 = make_shared<Edge>(node0, node1);
    edge0->setText("Edge 0");
    const auto edge1 = make_shared<Edge>(node0, node2);
    edge1->setText("Edge 1");
    edge0->updateLine();
    edge1->updateLine();

    QVERIFY(!edge0->translatedLabelBoundingRect().isEmpty());
    QVERIFY(edge0->translatedLabelBoundingRect().center() != edge1->translatedLabelBoundingRect().center());

    const auto rect = MagicZoom::calculateRectangleByEdges({ edge0.get(), edge1.get() });

    QVERIFY(rect.contains(edge0->translatedLabelBoundingRect().adjusted(-margin, -margin, margin, margin)));
    QVERIFY(rect.contains(edge1->translatedLabelBoundingRect().adjusted(-margin, -margin, margin, margin)));
}

QTEST_MAIN(MagicZoomTest)
