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

using std::make_shared;

GraphTest::GraphTest()
{
}

void GraphTest::testAddEdge()
{
    Graph dut;

    auto node0 = make_shared<NodeBase>();
    dut.addNode(node0);

    auto node1 = make_shared<NodeBase>();
    dut.addNode(node1);

    dut.addEdge(node0, node1);
    dut.addEdge(node0, node1); // Check that doubles are ignored

    auto edges = dut.getEdgesFromNode(node0);

    QCOMPARE(edges.size(), 1);
    QCOMPARE(edges.count(node1->index()), 1);
}

void GraphTest::testAddNode()
{
    Graph dut;
    auto node = make_shared<NodeBase>();

    QCOMPARE(node->index(), -1);

    dut.addNode(node);

    QCOMPARE(dut.numNodes(), 1);
    QCOMPARE(node->index(), 0);
}

void GraphTest::testAddTwoNodes()
{
    Graph dut;

    auto node0 = make_shared<NodeBase>();
    dut.addNode(node0);

    QCOMPARE(dut.numNodes(), 1);

    auto node1 = make_shared<NodeBase>();
    dut.addNode(node1);

    QCOMPARE(dut.numNodes(), 2);
    QCOMPARE(node0->index(), 0);
    QCOMPARE(node1->index(), 1);
}

void GraphTest::testGetEdges()
{
    Graph dut;

    auto node0 = make_shared<NodeBase>();
    dut.addNode(node0);

    auto node1 = make_shared<NodeBase>();
    dut.addNode(node1);

    dut.addEdge(node0, node1);
    dut.addEdge(node1, node0);

    auto edges = dut.getEdges();
    QCOMPARE(edges.size(), 2);

    auto iter = std::find_if(edges.begin(), edges.end(), [=](const Graph::Edge & edge){
        return edge.first == node0->index() && edge.second == node1->index();
    });

    QVERIFY(iter != edges.end());

    iter = std::find_if(edges.begin(), edges.end(), [=](const Graph::Edge & edge){
        return edge.first == node1->index() && edge.second == node0->index();
    });

    QVERIFY(iter != edges.end());
}

void GraphTest::testGetNodes()
{
    Graph dut;

    auto node0 = make_shared<NodeBase>();
    dut.addNode(node0);

    auto node1 = make_shared<NodeBase>();
    dut.addNode(node1);

    auto nodes = dut.getNodes();
    QCOMPARE(nodes.size(), 2);

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
    auto node = make_shared<NodeBase>();

    dut.addNode(node);

    QCOMPARE(node, dut.getNode(0));
}

void GraphTest::testGetNodeByIndex_NotFound()
{
    Graph dut;
    auto node = make_shared<NodeBase>();

    dut.addNode(node);

    QCOMPARE(dut.getNode(1), nullptr);
}

QTEST_GUILESS_MAIN(GraphTest)
