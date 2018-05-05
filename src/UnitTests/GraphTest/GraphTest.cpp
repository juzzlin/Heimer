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

    auto edge = make_shared<EdgeBase>(*node0, *node1);
    dut.addEdge(edge);
    dut.addEdge(edge); // Check that doubles are ignored

    auto edges = dut.getEdgesFromNode(node0);

    QCOMPARE(edges.size(), static_cast<size_t>(1));

    auto iter = std::find_if(edges.begin(), edges.end(), [=](const EdgeBasePtr & edge){
        return edge->sourceNodeBase().index() == node0->index() && edge->targetNodeBase().index() == node1->index();
    });

    QVERIFY(iter != edges.end());
}

void GraphTest::testAddEdgeByIndices()
{
    Graph dut;

    auto node0 = make_shared<NodeBase>();
    dut.addNode(node0);

    auto node1 = make_shared<NodeBase>();
    dut.addNode(node1);

    dut.addEdge(node0->index(), node1->index());
    dut.addEdge(node0->index(), node1->index()); // Check that doubles are ignored

    auto edges = dut.getEdgesFromNode(node0);

    QCOMPARE(edges.size(), static_cast<size_t>(1));

    auto iter = std::find_if(edges.begin(), edges.end(), [=](const EdgeBasePtr & edge){
        return edge->sourceNodeBase().index() == node0->index() && edge->targetNodeBase().index() == node1->index();
    });

    QVERIFY(iter != edges.end());
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

    dut.addEdge(std::make_shared<EdgeBase>(*node0, *node1));
    dut.addEdge(std::make_shared<EdgeBase>(*node1, *node0));

    auto edges = dut.getEdges();
    QCOMPARE(edges.size(), static_cast<size_t>(2));

    auto iter = std::find_if(edges.begin(), edges.end(), [=](const EdgeBasePtr & edge){
        return edge->sourceNodeBase().index() == node0->index() && edge->targetNodeBase().index() == node1->index();
    });

    QVERIFY(iter != edges.end());

    iter = std::find_if(edges.begin(), edges.end(), [=](const EdgeBasePtr & edge){
        return edge->sourceNodeBase().index() == node1->index() && edge->targetNodeBase().index() == node0->index();
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
    QCOMPARE(nodes.size(), static_cast<size_t>(2));

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

    QVERIFY(dut.getNode(1) == nullptr);
}

QTEST_GUILESS_MAIN(GraphTest)
