// This file is part of Dementia.
// Copyright (C) 2018 Jussi Lind <jussi.lind@iki.fi>
//
// Dementia is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dementia is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dementia. If not, see <http://www.gnu.org/licenses/>.

#include "GraphTest.hpp"

#include "../editor/graph.hpp"
#include "../editor/nodebase.hpp"

GraphTest::GraphTest()
{
}

void GraphTest::testAddNode()
{
    Graph dut;
    NodeBasePtr node(new NodeBase);

    QVERIFY(node->index() == -1);

    dut.addNode(node);

    QVERIFY(dut.numNodes() == 1);
    QVERIFY(node->index() == 0);
}

void GraphTest::testAddTwoNodes()
{
    Graph dut;

    NodeBasePtr node0(new NodeBase);
    dut.addNode(node0);

    QVERIFY(dut.numNodes() == 1);

    NodeBasePtr node1(new NodeBase);
    dut.addNode(node1);

    QVERIFY(dut.numNodes() == 2);

    QVERIFY(node0->index() == 0);
    QVERIFY(node1->index() == 1);
}

void GraphTest::testGetAll()
{
    Graph dut;

    NodeBasePtr node0(new NodeBase);
    dut.addNode(node0);

    NodeBasePtr node1(new NodeBase);
    dut.addNode(node1);

    auto && nodes = dut.getAll();
    QVERIFY(nodes.size() == 2);

    auto iter = std::find_if(nodes.begin(), nodes.end(), [](NodeBasePtr node){
        return node->index() == 0;
    });

    QVERIFY(iter != nodes.end());

    iter = std::find_if(nodes.begin(), nodes.end(), [](NodeBasePtr node){
        return node->index() == 1;
    });

    QVERIFY(iter != nodes.end());
}

void GraphTest::testGetNodeByIndex()
{
    Graph dut;
    NodeBasePtr node(new NodeBase);

    dut.addNode(node);

    auto nodeByIndex = dut.get(0);
    QVERIFY(node == nodeByIndex);
}

void GraphTest::testGetNodeByIndex_NotFound()
{
    Graph dut;
    NodeBasePtr node(new NodeBase);

    dut.addNode(node);

    auto nodeByIndex = dut.get(1);
    QVERIFY(nodeByIndex == nullptr);
}

QTEST_GUILESS_MAIN(GraphTest)
