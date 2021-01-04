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

#include "graph_test.hpp"

#include "graph.hpp"
#include "node.hpp"
#include "test_mode.hpp"

#include <stdexcept>
#include <string>

using std::make_shared;

GraphTest::GraphTest()
{
    TestMode::setEnabled(true);
}

void GraphTest::testAddEdge()
{
    Graph dut;

    const auto node0 = make_shared<Node>();
    dut.addNode(node0);

    const auto node1 = make_shared<Node>();
    dut.addNode(node1);

    const auto edge = make_shared<Edge>(*node0, *node1);
    dut.addEdge(edge);
    dut.addEdge(edge); // Check that doubles are ignored

    const auto edgesFrom0 = dut.getEdgesFromNode(node0);
    QCOMPARE(edgesFrom0.size(), static_cast<size_t>(1));
    QCOMPARE(edgesFrom0.at(0)->sourceNode().index(), node0->index());

    QCOMPARE(dut.getEdgesToNode(node0).size(), static_cast<size_t>(0));

    const auto edgesTo1 = dut.getEdgesToNode(node1);
    QCOMPARE(edgesTo1.size(), static_cast<size_t>(1));
    QCOMPARE(edgesTo1.at(0)->sourceNode().index(), node0->index());

    QCOMPARE(dut.getEdgesToNode(node0).size(), static_cast<size_t>(0));
}

void GraphTest::testAddNode()
{
    Graph dut;
    auto node = make_shared<Node>();

    QCOMPARE(node->index(), -1);

    dut.addNode(node);

    QCOMPARE(dut.numNodes(), static_cast<size_t>(1));
    QCOMPARE(node->index(), 0); // Node index should be automatic

    node = make_shared<Node>();
    node->setIndex(666);

    dut.addNode(node);

    QCOMPARE(dut.numNodes(), static_cast<size_t>(2));
    QCOMPARE(node->index(), 666); // Node index should be forced to 666

    node = make_shared<Node>();

    dut.addNode(node);

    QCOMPARE(dut.numNodes(), static_cast<size_t>(3));
    QCOMPARE(node->index(), 667); // Node index should be automatically 667
}

void GraphTest::testAddTwoNodes()
{
    Graph dut;

    const auto node0 = make_shared<Node>();
    dut.addNode(node0);

    QCOMPARE(dut.numNodes(), static_cast<size_t>(1));

    const auto node1 = make_shared<Node>();
    dut.addNode(node1);

    QCOMPARE(dut.numNodes(), static_cast<size_t>(2));
    QCOMPARE(node0->index(), 0);
    QCOMPARE(node1->index(), 1);
}

void GraphTest::testAreNodesDirectlyConnected()
{
    Graph dut;

    const auto node0 = make_shared<Node>();
    dut.addNode(node0);

    const auto node1 = make_shared<Node>();
    dut.addNode(node1);

    const auto node2 = make_shared<Node>();
    dut.addNode(node2);

    dut.addEdge(std::make_shared<Edge>(*node0, *node1));
    dut.addEdge(std::make_shared<Edge>(*node1, *node2));

    QVERIFY(dut.areDirectlyConnected(node0, node1));
    QVERIFY(!dut.areDirectlyConnected(node0, node2));

    QVERIFY(dut.areDirectlyConnected(node0->index(), node1->index()));
    QVERIFY(!dut.areDirectlyConnected(node0->index(), node2->index()));
}

void GraphTest::testDeleteEdge()
{
    Graph dut;

    const auto node0 = make_shared<Node>();
    dut.addNode(node0);

    const auto node1 = make_shared<Node>();
    dut.addNode(node1);

    const auto edge = make_shared<Edge>(*node0, *node1);
    dut.addEdge(edge);

    QCOMPARE(dut.getEdges().size(), static_cast<size_t>(1));

    QCOMPARE(dut.areDirectlyConnected(node0, node1), true);

    dut.deleteEdge(node0->index(), node1->index());

    QCOMPARE(dut.getEdges().size(), static_cast<size_t>(0));

    QCOMPARE(dut.areDirectlyConnected(node0, node1), false);
}

void GraphTest::testDeleteNode()
{
    Graph dut;
    const auto node = make_shared<Node>();

    dut.addNode(node);

    QCOMPARE(dut.numNodes(), static_cast<size_t>(1));

    dut.deleteNode(node->index());

    QCOMPARE(dut.numNodes(), static_cast<size_t>(0));
}

void GraphTest::testDeleteNodeInvolvingEdge()
{
    Graph dut;

    const auto node0 = make_shared<Node>();
    dut.addNode(node0);

    const auto node1 = make_shared<Node>();
    dut.addNode(node1);

    QCOMPARE(dut.numNodes(), static_cast<size_t>(2));

    dut.addEdge(make_shared<Edge>(*node0, *node1));

    QCOMPARE(dut.getEdgesFromNode(node0).size(), static_cast<size_t>(1));

    QCOMPARE(dut.getEdgesToNode(node1).size(), static_cast<size_t>(1));

    dut.deleteNode(node0->index());

    QCOMPARE(dut.numNodes(), static_cast<size_t>(1));

    QCOMPARE(dut.getEdgesToNode(node1).size(), static_cast<size_t>(0));
}

void GraphTest::testGetEdges()
{
    Graph dut;

    const auto node0 = make_shared<Node>();
    dut.addNode(node0);

    const auto node1 = make_shared<Node>();
    dut.addNode(node1);

    dut.addEdge(std::make_shared<Edge>(*node0, *node1));
    dut.addEdge(std::make_shared<Edge>(*node1, *node0));

    const auto edges = dut.getEdges();
    QCOMPARE(edges.size(), static_cast<size_t>(2));

    QVERIFY(std::count_if(edges.begin(), edges.end(), [=](const EdgePtr & edge) {
                return edge->sourceNode().index() == node0->index() && edge->targetNode().index() == node1->index();
            })
            == 1);

    QVERIFY(std::count_if(edges.begin(), edges.end(), [=](const EdgePtr & edge) {
                return edge->sourceNode().index() == node1->index() && edge->targetNode().index() == node0->index();
            })
            == 1);
}

void GraphTest::testGetNodes()
{
    Graph dut;

    const auto node0 = make_shared<Node>();
    dut.addNode(node0);

    const auto node1 = make_shared<Node>();
    dut.addNode(node1);

    const auto nodes = dut.getNodes();
    QCOMPARE(nodes.size(), static_cast<size_t>(2));

    QVERIFY(std::count_if(nodes.begin(), nodes.end(), [](NodePtr node) {
                return node->index() == 0;
            })
            == 1);

    QVERIFY(std::count_if(nodes.begin(), nodes.end(), [](NodePtr node) {
                return node->index() == 1;
            })
            == 1);
}

void GraphTest::testGetNodesConnectedToNode()
{
    Graph dut;

    const auto node0 = make_shared<Node>();
    dut.addNode(node0);

    const auto node1 = make_shared<Node>();
    dut.addNode(node1);

    const auto node2 = make_shared<Node>();
    dut.addNode(node2);

    dut.addEdge(std::make_shared<Edge>(*node0, *node1));
    dut.addEdge(std::make_shared<Edge>(*node2, *node0));

    const auto nodes = dut.getNodesConnectedToNode(node0);

    QVERIFY(nodes.size() == 2);

    // The order matters. Should be "to" nodes first and "from" nodes then.
    // This affects the direction of edges e.g. on node deletion.
    QVERIFY(nodes.at(0)->index() == node2->index());
    QVERIFY(nodes.at(1)->index() == node1->index());
}

void GraphTest::testGetNodeByIndex()
{
    Graph dut;
    const auto node = make_shared<Node>();

    dut.addNode(node);

    QCOMPARE(node, dut.getNode(0));
}

void GraphTest::testGetNodeByIndex_NotFound()
{
    Graph dut;
    auto node = make_shared<Node>();

    dut.addNode(node);

    std::string message;
    try {
        node = dut.getNode(666);
    } catch (const std::runtime_error & e) {
        message = e.what();
    }
    QVERIFY(message == "Invalid node index: " + std::to_string(666));
}

QTEST_GUILESS_MAIN(GraphTest)
