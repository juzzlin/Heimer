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

#include "editor_data_test.hpp"

#include "../../editor_data.hpp"
#include "../../mind_map_data.hpp"
#include "../../test_mode.hpp"

#include "../../core/graph.hpp"

using SceneItems::Edge;
using SceneItems::EdgeModel;
using SceneItems::Node;
using SceneItems::NodeModel;

EditorDataTest::EditorDataTest()
{
    TestMode::setEnabled(true);
}

void EditorDataTest::testAddAndDeleteEdge()
{
    EditorData editorData;
    editorData.setMindMapData(std::make_shared<MindMapData>());

    auto node0 = std::make_shared<Node>();
    editorData.mindMapData()->graph().addNode(node0);
    auto node1 = std::make_shared<Node>();
    editorData.mindMapData()->graph().addNode(node1);
    auto edge0 = std::make_shared<Edge>(node0, node1);
    editorData.addEdge(edge0);

    QCOMPARE(editorData.mindMapData()->graph().areDirectlyConnected(node0, node1), true);

    editorData.deleteEdge(*edge0);

    edge0.reset();

    QCOMPARE(editorData.mindMapData()->graph().areDirectlyConnected(node0, node1), false);

    editorData.addEdge(std::make_shared<Edge>(node0, node1));

    QCOMPARE(editorData.mindMapData()->graph().areDirectlyConnected(node0, node1), true);

    editorData.deleteEdge(node0->index(), node1->index());

    QCOMPARE(editorData.mindMapData()->graph().areDirectlyConnected(node0, node1), false);

    node0.reset();
    node1.reset();
}

void EditorDataTest::testGroupConnection()
{
    EditorData editorData;
    editorData.setMindMapData(std::make_shared<MindMapData>());
    QCOMPARE(editorData.areSelectedNodesConnectable(), false);

    const auto node0 = editorData.addNodeAt(QPointF(0, 0));
    const auto node1 = editorData.addNodeAt(QPointF(1, 1));
    const auto node2 = editorData.addNodeAt(QPointF(2, 2));

    QCOMPARE(editorData.selectionGroupSize(), size_t(0));
    editorData.toggleNodeInSelectionGroup(*node0);
    QCOMPARE(editorData.areSelectedNodesConnectable(), false);
    editorData.toggleNodeInSelectionGroup(*node2);
    QCOMPARE(editorData.areSelectedNodesConnectable(), true);
    editorData.toggleNodeInSelectionGroup(*node1);
    QCOMPARE(editorData.areSelectedNodesConnectable(), true);

    const auto edges = editorData.connectSelectedNodes();
    QCOMPARE(edges.at(0)->sourceNode().index(), node0->index());
    QCOMPARE(edges.at(0)->targetNode().index(), node2->index());
    QCOMPARE(edges.at(1)->sourceNode().index(), node2->index());
    QCOMPARE(edges.at(1)->targetNode().index(), node1->index());
    QCOMPARE(editorData.areSelectedNodesConnectable(), false);
}

void EditorDataTest::testGroupDisconnection()
{
    EditorData editorData;
    editorData.setMindMapData(std::make_shared<MindMapData>());
    QCOMPARE(editorData.areSelectedNodesDisconnectable(), false);

    const auto node0 = editorData.addNodeAt(QPointF(0, 0));
    const auto node1 = editorData.addNodeAt(QPointF(1, 1));
    const auto node2 = editorData.addNodeAt(QPointF(2, 2));

    QCOMPARE(editorData.selectionGroupSize(), size_t(0));
    editorData.toggleNodeInSelectionGroup(*node0);
    QCOMPARE(editorData.areSelectedNodesDisconnectable(), false);
    editorData.toggleNodeInSelectionGroup(*node2);
    QCOMPARE(editorData.areSelectedNodesDisconnectable(), false);
    editorData.toggleNodeInSelectionGroup(*node1);
    QCOMPARE(editorData.areSelectedNodesDisconnectable(), false);

    editorData.connectSelectedNodes();
    QCOMPARE(editorData.areSelectedNodesDisconnectable(), true);
    editorData.disconnectSelectedNodes();
    QCOMPARE(editorData.areSelectedNodesDisconnectable(), false);
    QCOMPARE(editorData.mindMapData()->graph().areDirectlyConnected(node0, node2), false);
    QCOMPARE(editorData.mindMapData()->graph().areDirectlyConnected(node2, node1), false);
}

void EditorDataTest::testGroupDelete()
{
    EditorData editorData;
    editorData.setMindMapData(std::make_shared<MindMapData>());
    auto node0 = editorData.addNodeAt(QPointF(0, 0));
    auto node1 = editorData.addNodeAt(QPointF(1, 1));

    editorData.toggleNodeInSelectionGroup(*node0);
    editorData.toggleNodeInSelectionGroup(*node1);

    editorData.deleteSelectedNodes();

    QCOMPARE(editorData.selectionGroupSize(), size_t(0));
    QCOMPARE(editorData.mindMapData()->graph().getNodes().empty(), true);
}

void EditorDataTest::testGroupMove()
{
    EditorData editorData;
    editorData.setMindMapData(std::make_shared<MindMapData>());
    auto node0 = editorData.addNodeAt(QPointF(0, 0));
    auto node1 = editorData.addNodeAt(QPointF(1, 1));

    editorData.toggleNodeInSelectionGroup(*node0);
    editorData.toggleNodeInSelectionGroup(*node1);

    editorData.moveSelectionGroup(*node0, { 1, 1 });

    QCOMPARE(qFuzzyCompare(node0->location().x(), 1), true);
    QCOMPARE(qFuzzyCompare(node0->location().y(), 1), true);
    QCOMPARE(qFuzzyCompare(node1->location().x(), 2), true);
    QCOMPARE(qFuzzyCompare(node1->location().y(), 2), true);
}

void EditorDataTest::testGroupSelection()
{
    EditorData editorData;
    editorData.setMindMapData(std::make_shared<MindMapData>());

    const auto node0 = editorData.addNodeAt(QPointF(0, 0));
    const auto node1 = editorData.addNodeAt(QPointF(1, 1));

    QCOMPARE(editorData.selectionGroupSize(), size_t(0));

    editorData.toggleNodeInSelectionGroup(*node0);
    editorData.toggleNodeInSelectionGroup(*node1);

    QCOMPARE(editorData.selectionGroupSize(), size_t(2));
    QCOMPARE(node0->selected(), true);
    QCOMPARE(node1->selected(), true);

    editorData.clearSelectionGroup();

    QCOMPARE(editorData.selectionGroupSize(), size_t(0));
    QCOMPARE(node0->selected(), false);
    QCOMPARE(node1->selected(), false);

    editorData.addNodeToSelectionGroup(*node0);
    editorData.addNodeToSelectionGroup(*node1);

    QCOMPARE(editorData.selectionGroupSize(), size_t(2));
    QCOMPARE(node0->selected(), true);
    QCOMPARE(node1->selected(), true);

    editorData.loadMindMapData(""); // Doesn't really load anything if in unit tests

    QCOMPARE(editorData.selectionGroupSize(), size_t(0));
}

void EditorDataTest::testLoadState()
{
    EditorData editorData;
    editorData.setMindMapData(std::make_shared<MindMapData>());

    const auto node0 = editorData.addNodeAt(QPointF(0, 0));
    const auto node1 = editorData.addNodeAt(QPointF(100, 0));
    const auto edge01 = editorData.addEdge(std::make_shared<Edge>(node0, node1));

    editorData.toggleNodeInSelectionGroup(*node0);
    editorData.setSelectedEdge(edge01.get());
    editorData.saveUndoPoint();

    QCOMPARE(editorData.isUndoable(), true);

    editorData.loadMindMapData(""); // Doesn't really load anything if in unit tests

    QCOMPARE(editorData.isUndoable(), false);
    QCOMPARE(editorData.selectionGroupSize(), size_t(0));
    QCOMPARE(editorData.selectedEdge(), nullptr);
    QCOMPARE(editorData.selectedNode(), nullptr);
}

void EditorDataTest::testUndoAddNodes()
{
    EditorData editorData;

    editorData.setMindMapData(std::make_shared<MindMapData>());

    QCOMPARE(editorData.isUndoable(), false);
    QCOMPARE(editorData.isModified(), false);

    editorData.saveUndoPoint();
    editorData.addNodeAt(QPointF(0, 0));
    QCOMPARE(editorData.isUndoable(), true);
    QCOMPARE(editorData.isModified(), true);

    editorData.saveUndoPoint();
    QCOMPARE(editorData.isUndoable(), true);
    QCOMPARE(editorData.isModified(), true); // Currently saveUndoPoint() triggers isModified

    editorData.addNodeAt(QPointF(1, 1));
    QCOMPARE(editorData.mindMapData()->graph().numNodes(), static_cast<size_t>(2));

    editorData.undo();
    QCOMPARE(editorData.isUndoable(), true);
    QCOMPARE(editorData.mindMapData()->graph().numNodes(), static_cast<size_t>(1));

    editorData.undo();
    QCOMPARE(editorData.isUndoable(), false);
    QCOMPARE(editorData.mindMapData()->graph().numNodes(), static_cast<size_t>(0));

    editorData.undo();
    QCOMPARE(editorData.isUndoable(), false);
    QCOMPARE(editorData.mindMapData()->graph().numNodes(), static_cast<size_t>(0));
}

void EditorDataTest::testRedoAddNodes()
{
    EditorData editorData;

    editorData.setMindMapData(std::make_shared<MindMapData>());
    editorData.addNodeAt(QPointF(0, 0));
    editorData.saveUndoPoint();
    editorData.addNodeAt(QPointF(1, 1));
    QCOMPARE(editorData.mindMapData()->graph().numNodes(), static_cast<size_t>(2));

    editorData.undo();
    QCOMPARE(editorData.isUndoable(), false);
    QCOMPARE(editorData.mindMapData()->graph().numNodes(), static_cast<size_t>(1));

    editorData.redo();
    QCOMPARE(editorData.isUndoable(), true);
    QCOMPARE(editorData.mindMapData()->graph().numNodes(), static_cast<size_t>(2));
}

void EditorDataTest::testTextSearch()
{
    const auto data = std::make_shared<MindMapData>();
    EditorData editorData;
    editorData.setMindMapData(data);

    const auto node0 = std::make_shared<Node>();
    data->graph().addNode(node0);
    const auto node1 = std::make_shared<Node>();
    data->graph().addNode(node1);
    const auto node2 = std::make_shared<Node>();
    data->graph().addNode(node2);

    QCOMPARE(node0->selected(), false);
    QCOMPARE(node1->selected(), false);
    QCOMPARE(node2->selected(), false);

    node0->setText("Foo");
    node1->setText("Bar");
    node2->setText("FooBar");

    editorData.selectNodesByText("foo");

    QCOMPARE(node0->selected(), true);
    QCOMPARE(node1->selected(), false);
    QCOMPARE(node2->selected(), true);

    editorData.selectNodesByText("");

    QCOMPARE(node0->selected(), false);
    QCOMPARE(node1->selected(), false);
    QCOMPARE(node2->selected(), false);
}

void EditorDataTest::testUndoAddEdge()
{
    const auto data = std::make_shared<MindMapData>();
    EditorData editorData;
    editorData.setMindMapData(data);

    const auto node0 = std::make_shared<Node>();
    data->graph().addNode(node0);
    const auto node1 = std::make_shared<Node>();
    data->graph().addNode(node1);
    const auto node2 = std::make_shared<Node>();
    data->graph().addNode(node2);

    const auto edge01 = std::make_shared<Edge>(node0, node1);

    editorData.saveUndoPoint();

    editorData.addEdge(edge01);

    const auto edge21 = std::make_shared<Edge>(node2, node1);

    editorData.saveUndoPoint();

    editorData.addEdge(edge21);

    QCOMPARE(editorData.mindMapData()->graph().areDirectlyConnected(node0, node1), true);
    QCOMPARE(editorData.mindMapData()->graph().areDirectlyConnected(node2, node1), true);

    editorData.undo();

    auto undoneNode0 = editorData.getNodeByIndex(node0->index());
    auto undoneNode1 = editorData.getNodeByIndex(node1->index());
    auto undoneNode2 = editorData.getNodeByIndex(node2->index());

    QCOMPARE(editorData.mindMapData()->graph().areDirectlyConnected(undoneNode0, undoneNode1), true);
    QCOMPARE(editorData.mindMapData()->graph().areDirectlyConnected(undoneNode2, undoneNode1), false);

    editorData.undo();

    undoneNode0 = editorData.getNodeByIndex(node0->index());
    undoneNode1 = editorData.getNodeByIndex(node1->index());
    undoneNode2 = editorData.getNodeByIndex(node2->index());

    QCOMPARE(editorData.mindMapData()->graph().areDirectlyConnected(undoneNode0, undoneNode1), false);
    QCOMPARE(editorData.mindMapData()->graph().areDirectlyConnected(undoneNode2, undoneNode1), false);

    editorData.redo();

    undoneNode0 = editorData.getNodeByIndex(node0->index());
    undoneNode1 = editorData.getNodeByIndex(node1->index());
    undoneNode2 = editorData.getNodeByIndex(node2->index());

    QCOMPARE(editorData.mindMapData()->graph().areDirectlyConnected(undoneNode0, undoneNode1), true);
    QCOMPARE(editorData.mindMapData()->graph().areDirectlyConnected(undoneNode2, undoneNode1), false);
}

void EditorDataTest::testUndoArrowMode()
{
    EditorData editorData;
    editorData.setMindMapData(std::make_shared<MindMapData>());

    const auto node0 = std::make_shared<Node>();
    editorData.mindMapData()->graph().addNode(node0);
    const auto node1 = std::make_shared<Node>();
    editorData.mindMapData()->graph().addNode(node1);
    const auto edge01 = std::make_shared<Edge>(node0, node1);
    editorData.addEdge(edge01);

    QCOMPARE(edge01->arrowMode(), EdgeModel::ArrowMode::Single);

    edge01->setArrowMode(EdgeModel::ArrowMode::Double);

    editorData.saveUndoPoint();

    edge01->setArrowMode(EdgeModel::ArrowMode::Hidden);

    editorData.undo();

    QCOMPARE(editorData.mindMapData()->graph().getEdges().at(0)->arrowMode(), EdgeModel::ArrowMode::Double);

    editorData.redo();

    QCOMPARE(editorData.mindMapData()->graph().getEdges().at(0)->arrowMode(), EdgeModel::ArrowMode::Hidden);
}

void EditorDataTest::testUndoArrowSize()
{
    EditorData editorData;

    editorData.setMindMapData(std::make_shared<MindMapData>());
    editorData.mindMapData()->setArrowSize(11.0);

    editorData.saveUndoPoint();
    editorData.mindMapData()->setArrowSize(12.5);

    editorData.saveUndoPoint();
    editorData.mindMapData()->setArrowSize(13.0);

    editorData.undo();
    QCOMPARE(editorData.mindMapData()->arrowSize(), 12.5);

    editorData.undo();
    QCOMPARE(editorData.mindMapData()->arrowSize(), 11.0);

    editorData.redo();
    QCOMPARE(editorData.mindMapData()->arrowSize(), 12.5);
}

void EditorDataTest::testUndoDeleteEdge()
{
    const auto data = std::make_shared<MindMapData>();
    EditorData editorData;
    editorData.setMindMapData(data);

    const auto node0 = std::make_shared<Node>();
    data->graph().addNode(node0);
    const auto node1 = std::make_shared<Node>();
    data->graph().addNode(node1);

    const auto edge01 = std::make_shared<Edge>(node0, node1);

    editorData.saveUndoPoint();

    editorData.addEdge(edge01);

    QCOMPARE(editorData.mindMapData()->graph().areDirectlyConnected(node0, node1), true);

    editorData.saveUndoPoint();

    editorData.deleteEdge(*edge01);

    QCOMPARE(editorData.mindMapData()->graph().areDirectlyConnected(node0, node1), false);

    editorData.undo();

    auto undoneNode0 = editorData.getNodeByIndex(node0->index());
    auto undoneNode1 = editorData.getNodeByIndex(node1->index());

    QCOMPARE(editorData.mindMapData()->graph().areDirectlyConnected(undoneNode0, undoneNode1), true);

    editorData.redo();

    undoneNode0 = editorData.getNodeByIndex(node0->index());
    undoneNode1 = editorData.getNodeByIndex(node1->index());

    QCOMPARE(editorData.mindMapData()->graph().areDirectlyConnected(undoneNode0, undoneNode1), false);
}

void EditorDataTest::testUndoBackgroundColor()
{
    EditorData editorData;

    editorData.setMindMapData(std::make_shared<MindMapData>());
    editorData.mindMapData()->setBackgroundColor(QColor(0, 0, 0));

    editorData.saveUndoPoint();
    editorData.mindMapData()->setBackgroundColor(QColor(1, 1, 1));

    editorData.saveUndoPoint();
    editorData.mindMapData()->setBackgroundColor(QColor(2, 2, 2));

    editorData.undo();
    QCOMPARE(editorData.mindMapData()->backgroundColor(), QColor(1, 1, 1));

    editorData.undo();
    QCOMPARE(editorData.mindMapData()->backgroundColor(), QColor(0, 0, 0));

    editorData.redo();
    QCOMPARE(editorData.mindMapData()->backgroundColor(), QColor(1, 1, 1));
}

void EditorDataTest::testUndoCornerRadius()
{
    EditorData editorData;

    editorData.setMindMapData(std::make_shared<MindMapData>());
    editorData.mindMapData()->setCornerRadius(12);

    editorData.saveUndoPoint();
    editorData.mindMapData()->setCornerRadius(11);

    editorData.saveUndoPoint();
    editorData.mindMapData()->setCornerRadius(10);

    editorData.undo();
    QCOMPARE(editorData.mindMapData()->cornerRadius(), 11);

    editorData.undo();
    QCOMPARE(editorData.mindMapData()->cornerRadius(), 12);

    editorData.redo();
    QCOMPARE(editorData.mindMapData()->cornerRadius(), 11);
}

void EditorDataTest::testUndoEdgeColor()
{
    EditorData editorData;

    editorData.setMindMapData(std::make_shared<MindMapData>());
    editorData.mindMapData()->setEdgeColor({ 1, 2, 3 });

    editorData.saveUndoPoint();
    editorData.mindMapData()->setEdgeColor({ 2, 3, 4 });

    editorData.saveUndoPoint();
    editorData.mindMapData()->setEdgeColor({ 4, 5, 6 });

    editorData.undo();
    QCOMPARE(editorData.mindMapData()->edgeColor(), QColor(2, 3, 4));

    editorData.undo();
    QCOMPARE(editorData.mindMapData()->edgeColor(), QColor(1, 2, 3));

    editorData.redo();
    QCOMPARE(editorData.mindMapData()->edgeColor(), QColor(2, 3, 4));
}

void EditorDataTest::testUndoEdgeDashedLine()
{
    EditorData editorData;
    editorData.setMindMapData(std::make_shared<MindMapData>());

    const auto node0 = std::make_shared<Node>();
    editorData.mindMapData()->graph().addNode(node0);
    const auto node1 = std::make_shared<Node>();
    editorData.mindMapData()->graph().addNode(node1);
    const auto edge01 = std::make_shared<Edge>(node0, node1);
    editorData.addEdge(edge01);

    edge01->setDashedLine(false);

    editorData.saveUndoPoint();

    edge01->setDashedLine(true);

    editorData.undo();

    QCOMPARE(editorData.mindMapData()->graph().getEdges().at(0)->dashedLine(), false);

    editorData.redo();

    QCOMPARE(editorData.mindMapData()->graph().getEdges().at(0)->dashedLine(), true);
}

void EditorDataTest::testUndoEdgeWidth()
{
    EditorData editorData;

    editorData.setMindMapData(std::make_shared<MindMapData>());
    editorData.mindMapData()->setEdgeWidth(1.0);

    editorData.saveUndoPoint();
    editorData.mindMapData()->setEdgeWidth(2.0);

    editorData.saveUndoPoint();
    editorData.mindMapData()->setEdgeWidth(3.0);

    editorData.undo();
    QCOMPARE(editorData.mindMapData()->edgeWidth(), 2.0);

    editorData.undo();
    QCOMPARE(editorData.mindMapData()->edgeWidth(), 1.0);

    editorData.redo();
    QCOMPARE(editorData.mindMapData()->edgeWidth(), 2.0);
}

void EditorDataTest::testUndoEdgeText()
{
    EditorData editorData;
    editorData.setMindMapData(std::make_shared<MindMapData>());

    const auto node0 = std::make_shared<Node>();
    editorData.mindMapData()->graph().addNode(node0);
    const auto node1 = std::make_shared<Node>();
    editorData.mindMapData()->graph().addNode(node1);

    const auto edge01 = std::make_shared<Edge>(node0, node1);
    editorData.addEdge(edge01);

    const auto text0 = "First";
    edge01->setText(text0);

    editorData.saveUndoPoint();

    const auto text1 = "Second";
    edge01->setText(text1);

    editorData.undo();

    QCOMPARE(editorData.mindMapData()->graph().getEdges().at(0)->text(), QString(text0));

    editorData.redo();

    QCOMPARE(editorData.mindMapData()->graph().getEdges().at(0)->text(), QString(text1));
}

void EditorDataTest::testUndoEdgeReversed()
{
    EditorData editorData;
    editorData.setMindMapData(std::make_shared<MindMapData>());

    const auto node0 = std::make_shared<Node>();
    editorData.mindMapData()->graph().addNode(node0);
    const auto node1 = std::make_shared<Node>();
    editorData.mindMapData()->graph().addNode(node1);

    const auto edge01 = std::make_shared<Edge>(node0, node1);

    editorData.addEdge(edge01);

    edge01->setReversed(false);

    editorData.saveUndoPoint();

    edge01->setReversed(true);

    editorData.undo();

    QCOMPARE(editorData.mindMapData()->graph().getEdges().at(0)->reversed(), false);

    editorData.redo();

    QCOMPARE(editorData.mindMapData()->graph().getEdges().at(0)->reversed(), true);
}

void EditorDataTest::testUndoFontChange()
{
    EditorData editorData;

    editorData.setMindMapData(std::make_shared<MindMapData>());
    const auto font = editorData.mindMapData()->font();
    editorData.saveUndoPoint();
    QFont newFont("Foo");
    editorData.mindMapData()->changeFont(newFont);

    QCOMPARE(editorData.mindMapData()->font().family(), newFont.family());

    editorData.undo();

    QCOMPARE(editorData.mindMapData()->font().family(), font.family());

    editorData.redo();

    QCOMPARE(editorData.mindMapData()->font().family(), newFont.family());
}

void EditorDataTest::testUndoGroupConnection()
{
    EditorData editorData;
    editorData.setMindMapData(std::make_shared<MindMapData>());

    const auto node0 = editorData.addNodeAt(QPointF(0, 0));
    const auto node1 = editorData.addNodeAt(QPointF(1, 1));
    const auto node2 = editorData.addNodeAt(QPointF(2, 2));

    editorData.toggleNodeInSelectionGroup(*node0);
    editorData.toggleNodeInSelectionGroup(*node2);
    editorData.toggleNodeInSelectionGroup(*node1);

    editorData.saveUndoPoint();

    const auto edges = editorData.connectSelectedNodes();

    QCOMPARE(editorData.areSelectedNodesConnectable(), false);

    editorData.undo();

    const auto undoneNode0 = editorData.getNodeByIndex(node0->index());
    const auto undoneNode1 = editorData.getNodeByIndex(node1->index());
    const auto undoneNode2 = editorData.getNodeByIndex(node2->index());

    editorData.toggleNodeInSelectionGroup(*undoneNode0);
    editorData.toggleNodeInSelectionGroup(*undoneNode2);
    editorData.toggleNodeInSelectionGroup(*undoneNode1);

    QCOMPARE(editorData.areSelectedNodesConnectable(), true);
}

void EditorDataTest::testUndoGroupDisconnection()
{
    EditorData editorData;
    editorData.setMindMapData(std::make_shared<MindMapData>());

    const auto node0 = editorData.addNodeAt(QPointF(0, 0));
    const auto node1 = editorData.addNodeAt(QPointF(1, 1));
    const auto node2 = editorData.addNodeAt(QPointF(2, 2));

    editorData.toggleNodeInSelectionGroup(*node0);
    editorData.toggleNodeInSelectionGroup(*node2);
    editorData.toggleNodeInSelectionGroup(*node1);

    editorData.connectSelectedNodes();

    editorData.saveUndoPoint();

    editorData.disconnectSelectedNodes();

    QCOMPARE(editorData.areSelectedNodesDisconnectable(), false);

    editorData.undo();

    const auto undoneNode0 = editorData.getNodeByIndex(node0->index());
    const auto undoneNode1 = editorData.getNodeByIndex(node1->index());
    const auto undoneNode2 = editorData.getNodeByIndex(node2->index());

    editorData.toggleNodeInSelectionGroup(*undoneNode0);
    editorData.toggleNodeInSelectionGroup(*undoneNode2);
    editorData.toggleNodeInSelectionGroup(*undoneNode1);

    QCOMPARE(editorData.areSelectedNodesDisconnectable(), true);
}

void EditorDataTest::testUndoNodeColor()
{
    const auto data = std::make_shared<MindMapData>();
    const auto node = std::make_shared<Node>();
    data->graph().addNode(node);

    EditorData editorData;
    editorData.setMindMapData(data);

    const QColor color0(1, 1, 1);
    node->setColor(color0);

    QCOMPARE(node->color(), color0);

    editorData.saveUndoPoint();

    const QColor color(6, 6, 6);
    node->setColor(color);

    QCOMPARE(node->color(), color);

    editorData.undo();

    const auto undoneNode = editorData.getNodeByIndex(node->index());

    QCOMPARE(undoneNode->color(), color0);

    editorData.redo();

    const auto redoneNode = editorData.getNodeByIndex(node->index());

    QCOMPARE(redoneNode->color(), color);
}

void EditorDataTest::testUndoNodeImage()
{
    const auto data = std::make_shared<MindMapData>();
    const auto node = std::make_shared<Node>();
    data->graph().addNode(node);

    EditorData editorData;
    editorData.setMindMapData(data);

    node->setImageRef(1);

    QCOMPARE(node->imageRef(), size_t { 1 });

    editorData.saveUndoPoint();

    node->setImageRef(2);

    QCOMPARE(node->imageRef(), size_t { 2 });

    editorData.undo();

    const auto undoneNode = editorData.getNodeByIndex(node->index());

    QCOMPARE(undoneNode->imageRef(), size_t { 1 });

    editorData.redo();

    const auto redoneNode = editorData.getNodeByIndex(node->index());

    QCOMPARE(redoneNode->imageRef(), size_t { 2 });
}

void EditorDataTest::testUndoNodeLocation()
{
    const auto data = std::make_shared<MindMapData>();
    const auto node = std::make_shared<Node>();
    data->graph().addNode(node);

    EditorData editorData;
    editorData.setMindMapData(data);

    const QPointF location0(1, 1);
    node->setLocation(location0);

    QCOMPARE(node->location(), location0);

    editorData.saveUndoPoint();

    const QPointF location(666, 666);
    node->setLocation(location);

    QCOMPARE(node->location(), location);

    editorData.undo();

    const auto undoneNode = editorData.getNodeByIndex(node->index());

    QCOMPARE(undoneNode->location(), location0);

    editorData.redo();

    const auto redoneNode = editorData.getNodeByIndex(node->index());

    QCOMPARE(redoneNode->location(), location);
}

void EditorDataTest::testUndoNodeTextColor()
{
    const auto data = std::make_shared<MindMapData>();
    const auto node = std::make_shared<Node>();
    data->graph().addNode(node);

    EditorData editorData;
    editorData.setMindMapData(data);

    const QColor color0(1, 1, 1);
    node->setTextColor(color0);

    editorData.saveUndoPoint();
    const QColor color(6, 6, 6);
    node->setTextColor(color);

    editorData.undo();

    const auto undoneNode = editorData.getNodeByIndex(node->index());

    QCOMPARE(undoneNode->textColor(), color0);

    editorData.redo();

    const auto redoneNode = editorData.getNodeByIndex(node->index());

    QCOMPARE(redoneNode->textColor(), color);
}

void EditorDataTest::testUndoTextSize()
{
    EditorData editorData;

    editorData.setMindMapData(std::make_shared<MindMapData>());
    editorData.mindMapData()->setTextSize(42);

    editorData.saveUndoPoint();
    editorData.mindMapData()->setTextSize(41);

    editorData.saveUndoPoint();
    editorData.mindMapData()->setTextSize(40);

    editorData.undo();
    QCOMPARE(editorData.mindMapData()->textSize(), 41);

    editorData.undo();
    QCOMPARE(editorData.mindMapData()->textSize(), 42);

    editorData.redo();
    QCOMPARE(editorData.mindMapData()->textSize(), 41);
}

void EditorDataTest::testUndoState()
{
    EditorData editorData;
    editorData.setMindMapData(std::make_shared<MindMapData>());

    const auto node0 = editorData.addNodeAt(QPointF(0, 0));
    const auto node1 = editorData.addNodeAt(QPointF(100, 0));
    const auto edge01 = editorData.addEdge(std::make_shared<Edge>(node0, node1));

    editorData.toggleNodeInSelectionGroup(*node0);
    editorData.setSelectedEdge(edge01.get());
    editorData.saveUndoPoint();

    QCOMPARE(editorData.isUndoable(), true);

    editorData.undo();

    QCOMPARE(editorData.isUndoable(), false);
    QCOMPARE(editorData.selectionGroupSize(), size_t(0));
    QCOMPARE(editorData.selectedEdge(), nullptr);
    QCOMPARE(editorData.selectedNode(), nullptr);
}

void EditorDataTest::testRedoState()
{
    EditorData editorData;
    editorData.setMindMapData(std::make_shared<MindMapData>());

    const auto node0 = editorData.addNodeAt(QPointF(0, 0));
    const auto node1 = editorData.addNodeAt(QPointF(100, 0));
    const auto edge01 = editorData.addEdge(std::make_shared<Edge>(node0, node1));

    editorData.toggleNodeInSelectionGroup(*node0);
    editorData.setSelectedEdge(edge01.get());
    editorData.saveUndoPoint();

    QCOMPARE(editorData.isUndoable(), true);

    editorData.undo();

    editorData.toggleNodeInSelectionGroup(*node0);
    editorData.setSelectedEdge(edge01.get());

    editorData.redo();

    QCOMPARE(editorData.isUndoable(), true);
    QCOMPARE(editorData.selectionGroupSize(), size_t(0));
    QCOMPARE(editorData.selectedEdge(), nullptr);
    QCOMPARE(editorData.selectedNode(), nullptr);

    editorData.undo();
    editorData.saveUndoPoint();

    QCOMPARE(editorData.isRedoable(), false);
}

void EditorDataTest::testUndoStackResetOnNewDesign()
{
    EditorData editorData;

    editorData.setMindMapData(std::make_shared<MindMapData>());

    QCOMPARE(editorData.isUndoable(), false);

    editorData.saveUndoPoint();
    editorData.addNodeAt(QPointF(0, 0));
    QCOMPARE(editorData.isUndoable(), true);

    editorData.setMindMapData(std::make_shared<MindMapData>());

    QCOMPARE(editorData.isUndoable(), false);
}

void EditorDataTest::testUndoStackResetOnLoadDesign()
{
    EditorData editorData;

    editorData.setMindMapData(std::make_shared<MindMapData>());

    QCOMPARE(editorData.isUndoable(), false);

    editorData.saveUndoPoint();
    editorData.addNodeAt(QPointF(0, 0));
    QCOMPARE(editorData.isUndoable(), true);

    editorData.loadMindMapData("dummy");

    QCOMPARE(editorData.isUndoable(), false);
}

void EditorDataTest::testUndoModificationFlagOnNewDesign()
{
    EditorData editorData;

    editorData.setMindMapData(std::make_shared<MindMapData>());

    QCOMPARE(editorData.isModified(), false);

    editorData.saveUndoPoint();
    editorData.addNodeAt(QPointF(0, 0));
    QCOMPARE(editorData.isModified(), true);

    editorData.setMindMapData(std::make_shared<MindMapData>());

    QCOMPARE(editorData.isModified(), false);
}

void EditorDataTest::testUndoModificationFlagOnLoadDesign()
{
    EditorData editorData;

    editorData.setMindMapData(std::make_shared<MindMapData>());

    QCOMPARE(editorData.isModified(), false);

    editorData.saveUndoPoint();
    editorData.addNodeAt(QPointF(0, 0));
    QCOMPARE(editorData.isModified(), true);

    editorData.loadMindMapData("dummy");

    QCOMPARE(editorData.isModified(), false);
}

QTEST_GUILESS_MAIN(EditorDataTest)
