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

#include "node.hpp"
#include "selection_group.hpp"
#include "test_mode.hpp"

SelectionGroupTest::SelectionGroupTest()
{
    TestMode::setEnabled(true);
}

void SelectionGroupTest::testAddNodes_Explicit()
{
    const auto node1 = std::make_unique<Node>();
    const auto node2 = std::make_unique<Node>();

    SelectionGroup selectionGroup;
    selectionGroup.addNode(*node1);
    selectionGroup.addNode(*node2);

    QCOMPARE(selectionGroup.hasNode(*node1), true);
    QCOMPARE(node1->selected(), true);
    QCOMPARE(selectionGroup.hasNode(*node2), true);
    QCOMPARE(node2->selected(), true);

    selectionGroup.clear();

    QCOMPARE(selectionGroup.hasNode(*node1), false);
    QCOMPARE(node1->selected(), false);
    QCOMPARE(selectionGroup.hasNode(*node2), false);
    QCOMPARE(node2->selected(), false);
    QCOMPARE(selectionGroup.size(), size_t(0));
}

void SelectionGroupTest::testAddNodes_Implicit()
{
    const auto node = std::make_unique<Node>();

    SelectionGroup selectionGroup;
    selectionGroup.addNode(*node, true);

    QCOMPARE(selectionGroup.hasNode(*node), true);
    QCOMPARE(node->selected(), true);
    QCOMPARE(selectionGroup.size(), size_t(1));

    selectionGroup.clear(true);

    QCOMPARE(selectionGroup.hasNode(*node), false);
    QCOMPARE(node->selected(), false);
    QCOMPARE(selectionGroup.size(), size_t(0));
}

void SelectionGroupTest::testAddNodes_ImplicitAndExplicit()
{
    const auto node1 = std::make_unique<Node>();
    const auto node2 = std::make_unique<Node>();

    SelectionGroup selectionGroup;
    selectionGroup.addNode(*node1, true);
    selectionGroup.addNode(*node2, false);

    QCOMPARE(selectionGroup.hasNode(*node1), true);
    QCOMPARE(node1->selected(), true);
    QCOMPARE(selectionGroup.hasNode(*node2), true);
    QCOMPARE(node2->selected(), true);
    QCOMPARE(selectionGroup.size(), size_t(2));

    selectionGroup.clear(true);

    QCOMPARE(selectionGroup.hasNode(*node1), false);
    QCOMPARE(node1->selected(), false);
    QCOMPARE(selectionGroup.hasNode(*node2), true);
    QCOMPARE(node2->selected(), true);
    QCOMPARE(selectionGroup.size(), size_t(1));

    selectionGroup.clear();

    QCOMPARE(selectionGroup.hasNode(*node1), false);
    QCOMPARE(node1->selected(), false);
    QCOMPARE(selectionGroup.hasNode(*node2), false);
    QCOMPARE(node2->selected(), false);
    QCOMPARE(selectionGroup.size(), size_t(0));
}

void SelectionGroupTest::testNodes()
{
    const auto node1 = std::make_unique<Node>();
    const auto node2 = std::make_unique<Node>();
    const auto node3 = std::make_unique<Node>();

    SelectionGroup selectionGroup;
    selectionGroup.addNode(*node1);
    selectionGroup.addNode(*node2);
    selectionGroup.addNode(*node3);

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

    SelectionGroup selectionGroup;
    selectionGroup.addNode(*node1);
    selectionGroup.addNode(*node2);
    selectionGroup.addNode(*node3);

    QCOMPARE(selectionGroup.selectedNode(), node1.get());
}

void SelectionGroupTest::testToggleNode()
{
    const auto node = std::make_unique<Node>();

    SelectionGroup selectionGroup;
    selectionGroup.toggleNode(*node);

    QCOMPARE(selectionGroup.hasNode(*node), true);
    QCOMPARE(node->selected(), true);

    selectionGroup.toggleNode(*node);

    QCOMPARE(selectionGroup.hasNode(*node), false);
    QCOMPARE(node->selected(), false);
}

QTEST_GUILESS_MAIN(SelectionGroupTest)
