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

#include "copy_context_test.hpp"

#include "../../common/test_mode.hpp"
#include "../../domain/copy_context.hpp"
#include "../../domain/graph.hpp"
#include "../../view/scene_items/edge.hpp"
#include "../../view/scene_items/node.hpp"

#include <QTest>

#include <memory>

using std::make_shared;

using SceneItems::Edge;
using SceneItems::Node;

CopyContextTest::CopyContextTest()
{
    TestMode::setEnabled(true);
}

void CopyContextTest::test_push_shouldCopyNodes()
{
    Graph graph;
    const auto node0 = make_shared<Node>();
    node0->setText("Node 0");
    graph.addNode(node0);
    const auto node1 = make_shared<Node>();
    node1->setText("Node 1");
    graph.addNode(node1);

    CopyContext dut;
    dut.push({ node0.get(), node1.get() }, graph);

    QCOMPARE(dut.copyStackSize(), size_t { 2 });
    const auto copiedData = dut.copiedData();
    QCOMPARE(copiedData.nodes.size(), size_t { 2 });
    QCOMPARE(copiedData.nodes.at(0)->text(), QString { "Node 0" });
    QCOMPARE(copiedData.nodes.at(1)->text(), QString { "Node 1" });
    // The copies must be separate instances
    QVERIFY(copiedData.nodes.at(0).get() != node0.get());
    QVERIFY(copiedData.edges.empty());
}

void CopyContextTest::test_push_shouldCopyOnlyEdgesBetweenCopiedNodes()
{
    Graph graph;
    const auto node0 = make_shared<Node>();
    graph.addNode(node0);
    const auto node1 = make_shared<Node>();
    graph.addNode(node1);
    const auto node2 = make_shared<Node>();
    graph.addNode(node2);
    const auto edge01 = make_shared<Edge>(node0, node1);
    edge01->setText("0 -> 1");
    graph.addEdge(edge01);
    graph.addEdge(make_shared<Edge>(node1, node2));

    CopyContext dut;
    dut.push({ node0.get(), node1.get() }, graph);

    const auto copiedData = dut.copiedData();
    QCOMPARE(copiedData.edges.size(), size_t { 1 });
    const auto & edgeMetadata = copiedData.edges.at(0);
    QCOMPARE(edgeMetadata.sourceNodeIndex, node0->index());
    QCOMPARE(edgeMetadata.targetNodeIndex, node1->index());
    QCOMPARE(edgeMetadata.edge->text(), QString { "0 -> 1" });
    QVERIFY(edgeMetadata.edge.get() != edge01.get());
}

void CopyContextTest::test_push_shouldCopyEdgesToBothDirections()
{
    Graph graph;
    const auto node0 = make_shared<Node>();
    graph.addNode(node0);
    const auto node1 = make_shared<Node>();
    graph.addNode(node1);
    graph.addEdge(make_shared<Edge>(node0, node1));
    graph.addEdge(make_shared<Edge>(node1, node0));

    CopyContext dut;
    // Push in reverse order to make sure the order doesn't matter
    dut.push({ node1.get(), node0.get() }, graph);

    const auto copiedData = dut.copiedData();
    QCOMPARE(copiedData.edges.size(), size_t { 2 });
    const auto & first = copiedData.edges.at(0);
    const auto & second = copiedData.edges.at(1);
    QCOMPARE(first.sourceNodeIndex, second.targetNodeIndex);
    QCOMPARE(first.targetNodeIndex, second.sourceNodeIndex);
    QVERIFY(first.sourceNodeIndex != first.targetNodeIndex);
}

void CopyContextTest::test_push_shouldCalculateReferencePointAsAverage()
{
    Graph graph;
    const auto node0 = make_shared<Node>();
    node0->setPos({ 0, 0 });
    graph.addNode(node0);
    const auto node1 = make_shared<Node>();
    node1->setPos({ 100, 50 });
    graph.addNode(node1);
    const auto node2 = make_shared<Node>();
    node2->setPos({ 200, -50 });
    graph.addNode(node2);

    CopyContext dut;
    dut.push({ node0.get(), node1.get(), node2.get() }, graph);

    QCOMPARE(dut.copiedData().copyReferencePoint, (QPointF { 100, 0 }));
}

void CopyContextTest::test_clear_shouldEmptyCopiedData()
{
    Graph graph;
    const auto node0 = make_shared<Node>();
    node0->setPos({ 10, 10 });
    graph.addNode(node0);
    const auto node1 = make_shared<Node>();
    graph.addNode(node1);
    graph.addEdge(make_shared<Edge>(node0, node1));

    CopyContext dut;
    dut.push({ node0.get(), node1.get() }, graph);
    dut.clear();

    QCOMPARE(dut.copyStackSize(), size_t { 0 });
    QVERIFY(dut.copiedData().nodes.empty());
    QVERIFY(dut.copiedData().edges.empty());
    QCOMPARE(dut.copiedData().copyReferencePoint, QPointF {});
}

QTEST_GUILESS_MAIN(CopyContextTest)
