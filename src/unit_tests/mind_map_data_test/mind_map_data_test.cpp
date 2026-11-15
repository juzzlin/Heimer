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
#include "mind_map_data_test.hpp"

#include "../../common/test_mode.hpp"
#include "../../domain/graph.hpp"
#include "../../domain/mind_map_data.hpp"
#include "../../domain/shadow_effect_params.hpp"
#include "../../domain/theme.hpp"
#include "../../view/scene_items/edge.hpp"
#include "../../view/scene_items/node.hpp"

#include <QTest>

using SceneItems::Edge;
using SceneItems::Node;

namespace {

Theme createTestTheme()
{
    Theme theme;
    theme.name = "Test";
    theme.backgroundColor = { 1, 1, 1 };
    theme.gridColor = { 2, 2, 2 };
    theme.nodeColor = { 3, 3, 3 };
    theme.nodeTextColor = { 4, 4, 4 };
    theme.cornerRadius = 11;
    theme.textSize = 13;
    theme.edgeColor = { 5, 5, 5 };
    theme.edgeWidth = 3.5;
    theme.arrowSize = 17;
    theme.edgeLabelColor = { 6, 6, 6 };
    theme.edgeLabelTextColor = { 7, 7, 7 };
    theme.shadowEffect = { 1, 2, 3, QColor { 8, 8, 8, 8 }, QColor { 9, 9, 9, 9 } };
    return theme;
}

struct TestMindMap
{
    TestMindMap()
    {
        node0->setColor({ 100, 0, 0 });
        node0->setTextColor({ 0, 100, 0 });
        data.graph().addNode(node0);
        data.graph().addNode(node1);
        data.graph().addEdge(edge);
    }

    MindMapData data;

    std::shared_ptr<Node> node0 = std::make_shared<Node>();

    std::shared_ptr<Node> node1 = std::make_shared<Node>();

    std::shared_ptr<Edge> edge = std::make_shared<Edge>(node0, node1);
};

} // namespace

MindMapDataTest::MindMapDataTest()
{
    TestMode::setEnabled(true);
}

void MindMapDataTest::test_defaultTheme_shouldHaveClassicEdgeLabelColors()
{
    const MindMapData data;

    QCOMPARE(data.edgeLabelColor(), Constants::Edge::defaultLabelColor());
    QCOMPARE(data.edgeLabelTextColor(), Constants::Edge::defaultLabelTextColor());
}

void MindMapDataTest::test_setTheme_shouldApplyToEdges()
{
    TestMindMap testMindMap;
    const auto theme = createTestTheme();

    testMindMap.data.setTheme(theme, false);

    QVERIFY(testMindMap.data.theme() == theme);
    QCOMPARE(testMindMap.edge->color(), theme.edgeColor);
    QCOMPARE(testMindMap.edge->labelColor(), theme.edgeLabelColor);
    QCOMPARE(testMindMap.edge->labelTextColor(), theme.edgeLabelTextColor);
    QVERIFY(testMindMap.edge->shadowEffect() == theme.shadowEffect);
}

void MindMapDataTest::test_setTheme_shouldApplyToNodesExceptNodeColors()
{
    TestMindMap testMindMap;
    const auto theme = createTestTheme();

    testMindMap.data.setTheme(theme, false);

    QCOMPARE(testMindMap.node0->cornerRadius(), theme.cornerRadius);
    QVERIFY(testMindMap.node0->shadowEffect() == theme.shadowEffect);
    // Individual node colors are kept
    QCOMPARE(testMindMap.node0->color(), (QColor { 100, 0, 0 }));
    QCOMPARE(testMindMap.node0->textColor(), (QColor { 0, 100, 0 }));
}

void MindMapDataTest::test_setTheme_withApplyNodeColorsShouldApplyNodeColors()
{
    TestMindMap testMindMap;
    const auto theme = createTestTheme();

    testMindMap.data.setTheme(theme, true);

    for (auto && node : { testMindMap.node0, testMindMap.node1 }) {
        QCOMPARE(node->color(), theme.nodeColor);
        QCOMPARE(node->textColor(), theme.nodeTextColor);
    }
}

void MindMapDataTest::test_setEdgeLabelColors_shouldApplyToEdges()
{
    TestMindMap testMindMap;

    testMindMap.data.setEdgeLabelColors({ 1, 2, 3 }, { 4, 5, 6 });

    QCOMPARE(testMindMap.data.theme().edgeLabelColor, (QColor { 1, 2, 3 }));
    QCOMPARE(testMindMap.data.theme().edgeLabelTextColor, (QColor { 4, 5, 6 }));
    QCOMPARE(testMindMap.edge->labelColor(), (QColor { 1, 2, 3 }));
    QCOMPARE(testMindMap.edge->labelTextColor(), (QColor { 4, 5, 6 }));
}

void MindMapDataTest::test_setShadowEffect_shouldApplyToNodesAndEdges()
{
    TestMindMap testMindMap;
    const ShadowEffectParams shadowEffect { 1, 2, 3, QColor { 1, 2, 3, 4 }, QColor { 5, 6, 7, 8 } };

    testMindMap.data.setShadowEffect(shadowEffect);

    QVERIFY(testMindMap.data.theme().shadowEffect == shadowEffect);
    QVERIFY(testMindMap.node0->shadowEffect() == shadowEffect);
    QVERIFY(testMindMap.node1->shadowEffect() == shadowEffect);
    QVERIFY(testMindMap.edge->shadowEffect() == shadowEffect);
}

void MindMapDataTest::test_copy_shouldCopyTheme()
{
    MindMapData data;
    const auto theme = createTestTheme();
    data.setTheme(theme, false);

    const MindMapData copy { data };

    QVERIFY(copy.theme() == theme);
}

QTEST_MAIN(MindMapDataTest)
