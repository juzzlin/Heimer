// This file is part of Heimer.
// Copyright (C) 2021 Jussi Lind <jussi.lind@iki.fi>
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

#include "node_test.hpp"

#include "../../common/test_mode.hpp"
#include "../../domain/shadow_effect_params.hpp"
#include "../../view/scene_items/node.hpp"

#include <QGraphicsScene>
#include <QImage>
#include <QPainter>
#include <QTest>

using SceneItems::Node;

NodeTest::NodeTest()
{
    TestMode::setEnabled(true);
}

void NodeTest::test_containsText()
{
    Node node;
    node.setText("Foobar");
    QVERIFY(node.containsText("Foobar"));
    QVERIFY(node.containsText("foobar"));
    QVERIFY(node.containsText("foo"));
    QVERIFY(node.containsText("bar"));
}

void NodeTest::test_bodyRect_shouldExcludeSelectionRing()
{
    const Node node;

    const auto bodyRect = node.bodyRect();
    const auto boundingRect = node.boundingRect();

    QVERIFY(boundingRect.contains(bodyRect));
    QVERIFY(boundingRect.width() > bodyRect.width());
    QVERIFY(boundingRect.height() > bodyRect.height());
    QCOMPARE(boundingRect.center(), bodyRect.center());
    QCOMPARE(node.shape().boundingRect(), bodyRect);
}

namespace {
QColor renderPixelAtSelectionRing(Node & node)
{
    QGraphicsScene scene;
    scene.setBackgroundBrush(Qt::white);
    scene.addItem(&node);
    const auto rect = node.boundingRect();
    QImage image { rect.size().toSize(), QImage::Format_ARGB32 };
    image.fill(Qt::white);
    {
        QPainter painter { &image };
        scene.render(&painter, QRectF { image.rect() }, rect);
    }
    // Painting adds the handles to the scene, but they are owned by the node
    for (auto && item : scene.items()) {
        scene.removeItem(item);
    }
    // The middle of the top edge of the ring
    return image.pixelColor(image.width() / 2, 1);
}
} // namespace

void NodeTest::test_paint_shouldDrawSelectionRingOnlyWhenSelected()
{
    Node node;
    node.setColor(Qt::white);
    node.setShadowEffect({ 0, 0, 0, QColor { 0, 0, 0, 0 }, QColor { 255, 0, 0 } });

    QCOMPARE(renderPixelAtSelectionRing(node), QColor { Qt::white });

    node.setSelected(true);
    const auto ringColor = renderPixelAtSelectionRing(node);
    QVERIFY2(ringColor.red() > 200 && ringColor.green() < 80 && ringColor.blue() < 80, qPrintable(ringColor.name()));
}

QTEST_MAIN(NodeTest)
