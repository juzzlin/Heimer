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

#include "graphtest.hpp"

#include "graph.hpp"
#include "nodebase.hpp"

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

    auto edgesFrom0 = dut.getEdgesFromNode(node0);
    QCOMPARE(edgesFrom0.size(), static_cast<size_t>(1));
    QCOMPARE(edgesFrom0.at(0)->sourceNodeBase().index(), node0->index());

    QCOMPARE(dut.getEdgesToNode(node0).size(), static_cast<size_t>(0));

    auto edgesTo1 = dut.getEdgesToNode(node1);
    QCOMPARE(edgesTo1.size(), static_cast<size_t>(1));
    QCOMPARE(edgesTo1.at(0)->sourceNodeBase().index(), node0->index());

    QCOMPARE(dut.getEdgesToNode(node0).size(), static_cast<size_t>(0));
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

    auto edgesFrom0 = dut.getEdgesFromNode(node0);
    QCOMPARE(edgesFrom0.size(), static_cast<size_t>(1));
    QCOMPARE(edgesFrom0.at(0)->sourceNodeBase().index(), node0->index());

    QCOMPARE(dut.getEdgesToNode(node0).size(), static_cast<size_t>(0));

    auto edgesTo1 = dut.getEdgesToNode(node1);
    QCOMPARE(edgesTo1.size(), static_cast<size_t>(1));
    QCOMPARE(edgesTo1.at(0)->sourceNodeBase().index(), node0->index());

    QCOMPARE(dut.getEdgesToNode(node0).size(), static_cast<size_t>(0));
}

void GraphTest::testAddNode()
{
    Graph dut;
    auto node = make_shared<NodeBase>();

    QCOMPARE(node->index(), -1);

    dut.addNode(node);

    QCOMPARE(dut.numNodes(), 1);
    QCOMPARE(node->index(), 0); // Node index should be automatic

    node = make_shared<NodeBase>();
    node->setIndex(666);

    dut.addNode(node);

    QCOMPARE(dut.numNodes(), 2);
    QCOMPARE(node->index(), 666); // Node index should be forced to 666

    node = make_shared<NodeBase>();

    dut.addNode(node);

    QCOMPARE(dut.numNodes(), 3);
    QCOMPARE(node->index(), 667); // Node index should be automatically 667
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

void GraphTest::testAreNodesDirectlyConnected()
{
    Graph dut;

    auto node0 = make_shared<NodeBase>();
    dut.addNode(node0);

    auto node1 = make_shared<NodeBase>();
    dut.addNode(node1);

    auto node2 = make_shared<NodeBase>();
    dut.addNode(node2);

    dut.addEdge(std::make_shared<EdgeBase>(*node0, *node1));
    dut.addEdge(std::make_shared<EdgeBase>(*node1, *node2));

    QVERIFY(dut.areDirectlyConnected(node0, node1));

    QVERIFY(!dut.areDirectlyConnected(node0, node2));
}

void GraphTest::testDeleteNode()
{
    Graph dut;
    auto node = make_shared<NodeBase>();

    dut.addNode(node);

    QCOMPARE(dut.numNodes(), 1);

    dut.deleteNode(node->index());

    QCOMPARE(dut.numNodes(), 0);
}

void GraphTest::testDeleteNodeInvolvingEdge()
{
    Graph dut;

    auto node0 = make_shared<NodeBase>();
    dut.addNode(node0);

    auto node1 = make_shared<NodeBase>();
    dut.addNode(node1);

    QCOMPARE(dut.numNodes(), 2);

    dut.addEdge(make_shared<EdgeBase>(*node0, *node1));

    QCOMPARE(dut.getEdgesFromNode(node0).size(), static_cast<size_t>(1));

    QCOMPARE(dut.getEdgesToNode(node1).size(), static_cast<size_t>(1));

    dut.deleteNode(node0->index());

    QCOMPARE(dut.numNodes(), 1);

    QCOMPARE(dut.getEdgesToNode(node1).size(), static_cast<size_t>(0));
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

    QVERIFY(std::count_if(edges.begin(), edges.end(), [=] (const EdgeBasePtr & edge) {
        return edge->sourceNodeBase().index() == node0->index() && edge->targetNodeBase().index() == node1->index();
    }) == 1);

    QVERIFY(std::count_if(edges.begin(), edges.end(), [=] (const EdgeBasePtr & edge) {
        return edge->sourceNodeBase().index() == node1->index() && edge->targetNodeBase().index() == node0->index();
    }) == 1);
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

    QVERIFY(std::count_if(nodes.begin(), nodes.end(), [] (NodeBasePtr node) {
        return node->index() == 0;
    }) == 1);

    QVERIFY(std::count_if(nodes.begin(), nodes.end(), [] (NodeBasePtr node) {
        return node->index() == 1;
    }) == 1);
}

void GraphTest::testGetNodesConnectedToNode()
{
    Graph dut;

    auto node0 = make_shared<NodeBase>();
    dut.addNode(node0);

    auto node1 = make_shared<NodeBase>();
    dut.addNode(node1);

    auto node2 = make_shared<NodeBase>();
    dut.addNode(node2);

    dut.addEdge(std::make_shared<EdgeBase>(*node0, *node1));
    dut.addEdge(std::make_shared<EdgeBase>(*node2, *node0));

    auto nodes = dut.getNodesConnectedToNode(node0);

    QVERIFY(nodes.size() == 2);

    QVERIFY(std::count_if(nodes.begin(), nodes.end(), [=] (NodeBasePtr node) {
        return node->index() == node1->index();
    }) == 1);

    QVERIFY(std::count_if(nodes.begin(), nodes.end(), [=] (NodeBasePtr node) {
        return node->index() == node2->index();
    }) == 1);
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
