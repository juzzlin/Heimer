// This file is part of Heimer.
// Copyright (C) 2022 Jussi Lind <jussi.lind@iki.fi>
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

#include "selection_group_test.hpp"

#include "../../common/test_mode.hpp"
#include "../../view/edge_selection_group.hpp"
#include "../../view/node_selection_group.hpp"
#include "../../view/scene_items/edge.hpp"
#include "../../view/scene_items/node.hpp"

using SceneItems::Edge;
using SceneItems::Node;

SelectionGroupTest::SelectionGroupTest()
{
    TestMode::setEnabled(true);
}

void SelectionGroupTest::testAddEdges_Explicit()
{
    const auto edge1 = std::make_unique<Edge>(nullptr, nullptr);
    const auto edge2 = std::make_unique<Edge>(nullptr, nullptr);

    EdgeSelectionGroup selectionGroup;
    selectionGroup.add(*edge1);
    selectionGroup.add(*edge2);

    QVERIFY(selectionGroup.contains(*edge1));
    QVERIFY(edge1->selected());
    QVERIFY(selectionGroup.contains(*edge2));
    QVERIFY(edge2->selected());

    selectionGroup.clear();

    QVERIFY(!selectionGroup.contains(*edge1));
    QVERIFY(!edge1->selected());
    QVERIFY(!selectionGroup.contains(*edge2));
    QVERIFY(!edge2->selected());
    QCOMPARE(selectionGroup.size(), size_t(0));
}

void SelectionGroupTest::testAddEdges_Implicit()
{
    const auto edge = std::make_unique<Edge>(nullptr, nullptr);

    EdgeSelectionGroup selectionGroup;
    selectionGroup.add(*edge, true);

    QVERIFY(selectionGroup.contains(*edge));
    QVERIFY(edge->selected());
    QCOMPARE(selectionGroup.size(), size_t(1));

    selectionGroup.clear(true);

    QVERIFY(!selectionGroup.contains(*edge));
    QVERIFY(!edge->selected());
    QCOMPARE(selectionGroup.size(), size_t(0));
}

void SelectionGroupTest::testAddEdges_ImplicitAndExplicit()
{
    const auto edge1 = std::make_unique<Edge>(nullptr, nullptr);
    const auto edge2 = std::make_unique<Edge>(nullptr, nullptr);

    EdgeSelectionGroup selectionGroup;
    selectionGroup.add(*edge1, true);
    selectionGroup.add(*edge2, false);

    QVERIFY(selectionGroup.contains(*edge1));
    QVERIFY(edge1->selected());
    QVERIFY(selectionGroup.contains(*edge2));
    QVERIFY(edge2->selected());
    QCOMPARE(selectionGroup.size(), size_t(2));

    selectionGroup.clear(true);

    QVERIFY(!selectionGroup.contains(*edge1));
    QVERIFY(!edge1->selected());
    QVERIFY(selectionGroup.contains(*edge2));
    QVERIFY(edge2->selected());
    QCOMPARE(selectionGroup.size(), size_t(1));

    selectionGroup.clear();

    QVERIFY(!selectionGroup.contains(*edge1));
    QVERIFY(!edge1->selected());
    QVERIFY(!selectionGroup.contains(*edge2));
    QVERIFY(!edge2->selected());
    QCOMPARE(selectionGroup.size(), size_t(0));
}

void SelectionGroupTest::testEdges()
{
    const auto edge1 = std::make_unique<Edge>(nullptr, nullptr);
    const auto edge2 = std::make_unique<Edge>(nullptr, nullptr);
    const auto edge3 = std::make_unique<Edge>(nullptr, nullptr);

    EdgeSelectionGroup selectionGroup;
    selectionGroup.add(*edge1);
    selectionGroup.add(*edge2);
    selectionGroup.add(*edge3);

    const auto edges = selectionGroup.edges();
    QCOMPARE(edges.size(), size_t(3));
    QCOMPARE(edges.at(0), edge1.get());
    QCOMPARE(edges.at(1), edge2.get());
    QCOMPARE(edges.at(2), edge3.get());
}

void SelectionGroupTest::testSelectedEdge()
{
    const auto edge1 = std::make_unique<Edge>(nullptr, nullptr);
    const auto edge2 = std::make_unique<Edge>(nullptr, nullptr);
    const auto edge3 = std::make_unique<Edge>(nullptr, nullptr);

    EdgeSelectionGroup selectionGroup;

    QVERIFY(!selectionGroup.selectedEdge().has_value());

    selectionGroup.add(*edge1);
    selectionGroup.add(*edge2);
    selectionGroup.add(*edge3);

    QVERIFY(selectionGroup.selectedEdge().has_value());
    QCOMPARE(selectionGroup.selectedEdge().value(), edge1.get());
}

void SelectionGroupTest::testToggleEdge()
{
    const auto edge = std::make_unique<Edge>(nullptr, nullptr);

    EdgeSelectionGroup selectionGroup;
    selectionGroup.toggle(*edge);

    QVERIFY(selectionGroup.contains(*edge));
    QVERIFY(edge->selected());

    selectionGroup.toggle(*edge);

    QVERIFY(!selectionGroup.contains(*edge));
    QVERIFY(!edge->selected());
}
void SelectionGroupTest::testAddNodes_Explicit()
{
    const auto node1 = std::make_unique<Node>();
    const auto node2 = std::make_unique<Node>();

    NodeSelectionGroup selectionGroup;
    selectionGroup.add(*node1);
    selectionGroup.add(*node2);
    
    QCOMPARE(selectionGroup.contains(*node1), true);
    QCOMPARE(node1->selected(), true);
    QCOMPARE(selectionGroup.contains(*node2), true);
    QCOMPARE(node2->selected(), true);

    selectionGroup.clear();
    
    QCOMPARE(selectionGroup.contains(*node1), false);
    QCOMPARE(node1->selected(), false);
    QCOMPARE(selectionGroup.contains(*node2), false);
    QCOMPARE(node2->selected(), false);
    QCOMPARE(selectionGroup.size(), size_t(0));
}

void SelectionGroupTest::testAddNodes_Implicit()
{
    const auto node = std::make_unique<Node>();

    NodeSelectionGroup selectionGroup;
    selectionGroup.add(*node, true);
    
    QCOMPARE(selectionGroup.contains(*node), true);
    QCOMPARE(node->selected(), true);
    QCOMPARE(selectionGroup.size(), size_t(1));

    selectionGroup.clear(true);
    
    QCOMPARE(selectionGroup.contains(*node), false);
    QCOMPARE(node->selected(), false);
    QCOMPARE(selectionGroup.size(), size_t(0));
}

void SelectionGroupTest::testAddNodes_ImplicitAndExplicit()
{
    const auto node1 = std::make_unique<Node>();
    const auto node2 = std::make_unique<Node>();

    NodeSelectionGroup selectionGroup;
    selectionGroup.add(*node1, true);
    selectionGroup.add(*node2, false);
    
    QCOMPARE(selectionGroup.contains(*node1), true);
    QCOMPARE(node1->selected(), true);
    QCOMPARE(selectionGroup.contains(*node2), true);
    QCOMPARE(node2->selected(), true);
    QCOMPARE(selectionGroup.size(), size_t(2));

    selectionGroup.clear(true);
    
    QCOMPARE(selectionGroup.contains(*node1), false);
    QCOMPARE(node1->selected(), false);
    QCOMPARE(selectionGroup.contains(*node2), true);
    QCOMPARE(node2->selected(), true);
    QCOMPARE(selectionGroup.size(), size_t(1));

    selectionGroup.clear();
    
    QCOMPARE(selectionGroup.contains(*node1), false);
    QCOMPARE(node1->selected(), false);
    QCOMPARE(selectionGroup.contains(*node2), false);
    QCOMPARE(node2->selected(), false);
    QCOMPARE(selectionGroup.size(), size_t(0));
}

void SelectionGroupTest::testNodes()
{
    const auto node1 = std::make_unique<Node>();
    const auto node2 = std::make_unique<Node>();
    const auto node3 = std::make_unique<Node>();

    NodeSelectionGroup selectionGroup;
    selectionGroup.add(*node1);
    selectionGroup.add(*node2);
    selectionGroup.add(*node3);

    const auto nodes = selectionGroup.nodes();
    QCOMPARE(nodes.size(), size_t(3));
    QCOMPARE(nodes.at(0), node1.get());
    QCOMPARE(nodes.at(1), node2.get());
    QCOMPARE(nodes.at(2), node3.get());
}

void SelectionGroupTest::testSelectedNode()
{
    const auto node1 = std::make_unique<Node>();
    const auto node2 = std::make_unique<Node>();
    const auto node3 = std::make_unique<Node>();

    NodeSelectionGroup selectionGroup;

    QVERIFY(!selectionGroup.selectedNode().has_value());

    selectionGroup.add(*node1);
    selectionGroup.add(*node2);
    selectionGroup.add(*node3);

    QVERIFY(selectionGroup.selectedNode().has_value());
    QCOMPARE(selectionGroup.selectedNode().value(), node1.get());
}

void SelectionGroupTest::testToggleNode()
{
    const auto node = std::make_unique<Node>();

    NodeSelectionGroup selectionGroup;
    selectionGroup.toggle(*node);
    
    QCOMPARE(selectionGroup.contains(*node), true);
    QCOMPARE(node->selected(), true);
    
    selectionGroup.toggle(*node);
    
    QCOMPARE(selectionGroup.contains(*node), false);
    QCOMPARE(node->selected(), false);
}

QTEST_GUILESS_MAIN(SelectionGroupTest)
