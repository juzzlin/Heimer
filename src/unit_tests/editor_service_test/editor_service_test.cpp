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

#include "editor_service_test.hpp"

#include "../../application/editor_service.hpp"
#include "../../common/test_mode.hpp"
#include "../../domain/graph.hpp"
#include "../../domain/mind_map_data.hpp"

using SceneItems::Edge;
using SceneItems::EdgeModel;
using SceneItems::Node;
using SceneItems::NodeModel;

EditorServiceTest::EditorServiceTest()
{
    TestMode::setEnabled(true);
}

void EditorServiceTest::testAddAndDeleteEdge()
{
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());

    auto node0 = std::make_shared<Node>();
    editorService.mindMapData()->graph().addNode(node0);
    auto node1 = std::make_shared<Node>();
    editorService.mindMapData()->graph().addNode(node1);
    auto edge0 = std::make_shared<Edge>(node0, node1);
    editorService.addEdge(edge0);

    QCOMPARE(editorService.mindMapData()->graph().areDirectlyConnected(node0, node1), true);

    editorService.deleteEdge(*edge0);

    edge0.reset();

    QCOMPARE(editorService.mindMapData()->graph().areDirectlyConnected(node0, node1), false);

    editorService.addEdge(std::make_shared<Edge>(node0, node1));

    QCOMPARE(editorService.mindMapData()->graph().areDirectlyConnected(node0, node1), true);

    editorService.deleteEdge(node0->index(), node1->index());

    QCOMPARE(editorService.mindMapData()->graph().areDirectlyConnected(node0, node1), false);

    node0.reset();
    node1.reset();
}

void EditorServiceTest::testGroupConnection()
{
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());
    QCOMPARE(editorService.areSelectedNodesConnectable(), false);

    const auto node0 = editorService.addNodeAt(QPointF(0, 0));
    const auto node1 = editorService.addNodeAt(QPointF(1, 1));
    const auto node2 = editorService.addNodeAt(QPointF(2, 2));

    QCOMPARE(editorService.selectionGroupSize(), size_t(0));
    editorService.toggleNodeInSelectionGroup(*node0);
    QCOMPARE(editorService.areSelectedNodesConnectable(), false);
    editorService.toggleNodeInSelectionGroup(*node2);
    QCOMPARE(editorService.areSelectedNodesConnectable(), true);
    editorService.toggleNodeInSelectionGroup(*node1);
    QCOMPARE(editorService.areSelectedNodesConnectable(), true);

    const auto edges = editorService.connectSelectedNodes();
    QCOMPARE(edges.at(0)->sourceNode().index(), node0->index());
    QCOMPARE(edges.at(0)->targetNode().index(), node2->index());
    QCOMPARE(edges.at(1)->sourceNode().index(), node2->index());
    QCOMPARE(edges.at(1)->targetNode().index(), node1->index());
    QCOMPARE(editorService.areSelectedNodesConnectable(), false);
}

void EditorServiceTest::testGroupDisconnection()
{
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());
    QCOMPARE(editorService.areSelectedNodesDisconnectable(), false);

    const auto node0 = editorService.addNodeAt(QPointF(0, 0));
    const auto node1 = editorService.addNodeAt(QPointF(1, 1));
    const auto node2 = editorService.addNodeAt(QPointF(2, 2));

    QCOMPARE(editorService.selectionGroupSize(), size_t(0));
    editorService.toggleNodeInSelectionGroup(*node0);
    QCOMPARE(editorService.areSelectedNodesDisconnectable(), false);
    editorService.toggleNodeInSelectionGroup(*node2);
    QCOMPARE(editorService.areSelectedNodesDisconnectable(), false);
    editorService.toggleNodeInSelectionGroup(*node1);
    QCOMPARE(editorService.areSelectedNodesDisconnectable(), false);

    editorService.connectSelectedNodes();
    QCOMPARE(editorService.areSelectedNodesDisconnectable(), true);
    editorService.disconnectSelectedNodes();
    QCOMPARE(editorService.areSelectedNodesDisconnectable(), false);
    QCOMPARE(editorService.mindMapData()->graph().areDirectlyConnected(node0, node2), false);
    QCOMPARE(editorService.mindMapData()->graph().areDirectlyConnected(node2, node1), false);
}

void EditorServiceTest::testGroupDelete()
{
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());
    auto node0 = editorService.addNodeAt(QPointF(0, 0));
    auto node1 = editorService.addNodeAt(QPointF(1, 1));

    editorService.toggleNodeInSelectionGroup(*node0);
    editorService.toggleNodeInSelectionGroup(*node1);

    editorService.deleteSelectedNodes();

    QCOMPARE(editorService.selectionGroupSize(), size_t(0));
    QCOMPARE(editorService.mindMapData()->graph().getNodes().empty(), true);
}

void EditorServiceTest::testGroupMove()
{
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());
    auto node0 = editorService.addNodeAt(QPointF(0, 0));
    auto node1 = editorService.addNodeAt(QPointF(1, 1));

    editorService.toggleNodeInSelectionGroup(*node0);
    editorService.toggleNodeInSelectionGroup(*node1);

    editorService.moveSelectionGroup(*node0, { 1, 1 });

    QCOMPARE(qFuzzyCompare(node0->location().x(), 1), true);
    QCOMPARE(qFuzzyCompare(node0->location().y(), 1), true);
    QCOMPARE(qFuzzyCompare(node1->location().x(), 2), true);
    QCOMPARE(qFuzzyCompare(node1->location().y(), 2), true);
}

void EditorServiceTest::testInitializationResetsFileName()
{
    EditorService editorService;
    const QString fileName = "dummy";
    editorService.loadMindMapData(fileName); // Doesn't really load anything if in unit tests
    QCOMPARE(editorService.fileName(), fileName);
    editorService.initializeNewMindMap();
    QCOMPARE(editorService.fileName(), QString { "" });
}

void EditorServiceTest::testGroupSelection()
{
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());

    const auto node0 = editorService.addNodeAt(QPointF(0, 0));
    const auto node1 = editorService.addNodeAt(QPointF(1, 1));

    QCOMPARE(editorService.selectionGroupSize(), size_t(0));

    editorService.toggleNodeInSelectionGroup(*node0);
    editorService.toggleNodeInSelectionGroup(*node1);

    QCOMPARE(editorService.selectionGroupSize(), size_t(2));
    QCOMPARE(node0->selected(), true);
    QCOMPARE(node1->selected(), true);

    editorService.clearSelectionGroup();

    QCOMPARE(editorService.selectionGroupSize(), size_t(0));
    QCOMPARE(node0->selected(), false);
    QCOMPARE(node1->selected(), false);

    editorService.addNodeToSelectionGroup(*node0);
    editorService.addNodeToSelectionGroup(*node1);

    QCOMPARE(editorService.selectionGroupSize(), size_t(2));
    QCOMPARE(node0->selected(), true);
    QCOMPARE(node1->selected(), true);

    editorService.loadMindMapData(""); // Doesn't really load anything if in unit tests

    QCOMPARE(editorService.selectionGroupSize(), size_t(0));
}

void EditorServiceTest::testLoadState()
{
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());

    const auto node0 = editorService.addNodeAt(QPointF(0, 0));
    const auto node1 = editorService.addNodeAt(QPointF(100, 0));
    const auto edge01 = editorService.addEdge(std::make_shared<Edge>(node0, node1));

    editorService.toggleNodeInSelectionGroup(*node0);
    editorService.setSelectedEdge(edge01.get());
    editorService.saveUndoPoint();

    QCOMPARE(editorService.isUndoable(), true);

    const QString fileName = "dummy";
    editorService.loadMindMapData(fileName); // Doesn't really load anything if in unit tests

    QCOMPARE(editorService.fileName(), fileName);
    QCOMPARE(editorService.isUndoable(), false);
    QCOMPARE(editorService.selectionGroupSize(), size_t(0));
    QCOMPARE(editorService.selectedEdge(), nullptr);
    QVERIFY(!editorService.selectedNode().has_value());
}

void EditorServiceTest::testUndoAddNodes()
{
    EditorService editorService;

    editorService.setMindMapData(std::make_shared<MindMapData>());

    QCOMPARE(editorService.isUndoable(), false);
    QCOMPARE(editorService.isModified(), false);

    editorService.saveUndoPoint();
    editorService.addNodeAt(QPointF(0, 0));
    QCOMPARE(editorService.isUndoable(), true);
    QCOMPARE(editorService.isModified(), true);

    editorService.saveUndoPoint();
    QCOMPARE(editorService.isUndoable(), true);
    QCOMPARE(editorService.isModified(), true); // Currently saveUndoPoint() triggers isModified

    editorService.addNodeAt(QPointF(1, 1));
    QCOMPARE(editorService.mindMapData()->graph().nodeCount(), static_cast<size_t>(2));

    editorService.undo();
    QCOMPARE(editorService.isUndoable(), true);
    QCOMPARE(editorService.mindMapData()->graph().nodeCount(), static_cast<size_t>(1));

    editorService.undo();
    QCOMPARE(editorService.isUndoable(), false);
    QCOMPARE(editorService.mindMapData()->graph().nodeCount(), static_cast<size_t>(0));

    editorService.undo();
    QCOMPARE(editorService.isUndoable(), false);
    QCOMPARE(editorService.mindMapData()->graph().nodeCount(), static_cast<size_t>(0));
}

void EditorServiceTest::testRedoAddNodes()
{
    EditorService editorService;

    editorService.setMindMapData(std::make_shared<MindMapData>());
    editorService.addNodeAt(QPointF(0, 0));
    editorService.saveUndoPoint();
    editorService.addNodeAt(QPointF(1, 1));
    QCOMPARE(editorService.mindMapData()->graph().nodeCount(), static_cast<size_t>(2));

    editorService.undo();
    QCOMPARE(editorService.isUndoable(), false);
    QCOMPARE(editorService.mindMapData()->graph().nodeCount(), static_cast<size_t>(1));

    editorService.redo();
    QCOMPARE(editorService.isUndoable(), true);
    QCOMPARE(editorService.mindMapData()->graph().nodeCount(), static_cast<size_t>(2));
}

void EditorServiceTest::testTextSearch()
{
    const auto data = std::make_shared<MindMapData>();
    EditorService editorService;
    editorService.setMindMapData(data);

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

    editorService.selectNodesByText("foo");

    QCOMPARE(node0->selected(), true);
    QCOMPARE(node1->selected(), false);
    QCOMPARE(node2->selected(), true);

    editorService.selectNodesByText("");

    QCOMPARE(node0->selected(), false);
    QCOMPARE(node1->selected(), false);
    QCOMPARE(node2->selected(), false);
}

void EditorServiceTest::testUndoAddEdge()
{
    const auto data = std::make_shared<MindMapData>();
    EditorService editorService;
    editorService.setMindMapData(data);

    const auto node0 = std::make_shared<Node>();
    data->graph().addNode(node0);
    const auto node1 = std::make_shared<Node>();
    data->graph().addNode(node1);
    const auto node2 = std::make_shared<Node>();
    data->graph().addNode(node2);

    const auto edge01 = std::make_shared<Edge>(node0, node1);

    editorService.saveUndoPoint();

    editorService.addEdge(edge01);

    const auto edge21 = std::make_shared<Edge>(node2, node1);

    editorService.saveUndoPoint();

    editorService.addEdge(edge21);

    QCOMPARE(editorService.mindMapData()->graph().areDirectlyConnected(node0, node1), true);
    QCOMPARE(editorService.mindMapData()->graph().areDirectlyConnected(node2, node1), true);

    editorService.undo();

    auto undoneNode0 = editorService.getNodeByIndex(node0->index());
    auto undoneNode1 = editorService.getNodeByIndex(node1->index());
    auto undoneNode2 = editorService.getNodeByIndex(node2->index());

    QCOMPARE(editorService.mindMapData()->graph().areDirectlyConnected(undoneNode0, undoneNode1), true);
    QCOMPARE(editorService.mindMapData()->graph().areDirectlyConnected(undoneNode2, undoneNode1), false);

    editorService.undo();

    undoneNode0 = editorService.getNodeByIndex(node0->index());
    undoneNode1 = editorService.getNodeByIndex(node1->index());
    undoneNode2 = editorService.getNodeByIndex(node2->index());

    QCOMPARE(editorService.mindMapData()->graph().areDirectlyConnected(undoneNode0, undoneNode1), false);
    QCOMPARE(editorService.mindMapData()->graph().areDirectlyConnected(undoneNode2, undoneNode1), false);

    editorService.redo();

    undoneNode0 = editorService.getNodeByIndex(node0->index());
    undoneNode1 = editorService.getNodeByIndex(node1->index());
    undoneNode2 = editorService.getNodeByIndex(node2->index());

    QCOMPARE(editorService.mindMapData()->graph().areDirectlyConnected(undoneNode0, undoneNode1), true);
    QCOMPARE(editorService.mindMapData()->graph().areDirectlyConnected(undoneNode2, undoneNode1), false);
}

void EditorServiceTest::testUndoArrowMode()
{
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());

    const auto node0 = std::make_shared<Node>();
    editorService.mindMapData()->graph().addNode(node0);
    const auto node1 = std::make_shared<Node>();
    editorService.mindMapData()->graph().addNode(node1);
    const auto edge01 = std::make_shared<Edge>(node0, node1);
    editorService.addEdge(edge01);

    QCOMPARE(edge01->arrowMode(), EdgeModel::ArrowMode::Single);

    edge01->setArrowMode(EdgeModel::ArrowMode::Double);

    editorService.saveUndoPoint();

    edge01->setArrowMode(EdgeModel::ArrowMode::Hidden);

    editorService.undo();

    QCOMPARE(editorService.mindMapData()->graph().getEdges().at(0)->arrowMode(), EdgeModel::ArrowMode::Double);

    editorService.redo();

    QCOMPARE(editorService.mindMapData()->graph().getEdges().at(0)->arrowMode(), EdgeModel::ArrowMode::Hidden);
}

void EditorServiceTest::testUndoArrowSize()
{
    EditorService editorService;

    editorService.setMindMapData(std::make_shared<MindMapData>());
    editorService.mindMapData()->setArrowSize(11.0);

    editorService.saveUndoPoint();
    editorService.mindMapData()->setArrowSize(12.5);

    editorService.saveUndoPoint();
    editorService.mindMapData()->setArrowSize(13.0);

    editorService.undo();
    QCOMPARE(editorService.mindMapData()->arrowSize(), 12.5);

    editorService.undo();
    QCOMPARE(editorService.mindMapData()->arrowSize(), 11.0);

    editorService.redo();
    QCOMPARE(editorService.mindMapData()->arrowSize(), 12.5);
}

void EditorServiceTest::testUndoDeleteEdge()
{
    const auto data = std::make_shared<MindMapData>();
    EditorService editorService;
    editorService.setMindMapData(data);

    const auto node0 = std::make_shared<Node>();
    data->graph().addNode(node0);
    const auto node1 = std::make_shared<Node>();
    data->graph().addNode(node1);

    const auto edge01 = std::make_shared<Edge>(node0, node1);

    editorService.saveUndoPoint();

    editorService.addEdge(edge01);

    QCOMPARE(editorService.mindMapData()->graph().areDirectlyConnected(node0, node1), true);

    editorService.saveUndoPoint();

    editorService.deleteEdge(*edge01);

    QCOMPARE(editorService.mindMapData()->graph().areDirectlyConnected(node0, node1), false);

    editorService.undo();

    auto undoneNode0 = editorService.getNodeByIndex(node0->index());
    auto undoneNode1 = editorService.getNodeByIndex(node1->index());

    QCOMPARE(editorService.mindMapData()->graph().areDirectlyConnected(undoneNode0, undoneNode1), true);

    editorService.redo();

    undoneNode0 = editorService.getNodeByIndex(node0->index());
    undoneNode1 = editorService.getNodeByIndex(node1->index());

    QCOMPARE(editorService.mindMapData()->graph().areDirectlyConnected(undoneNode0, undoneNode1), false);
}

void EditorServiceTest::testUndoBackgroundColor()
{
    EditorService editorService;

    editorService.setMindMapData(std::make_shared<MindMapData>());
    editorService.mindMapData()->setBackgroundColor(QColor(0, 0, 0));

    editorService.saveUndoPoint();
    editorService.mindMapData()->setBackgroundColor(QColor(1, 1, 1));

    editorService.saveUndoPoint();
    editorService.mindMapData()->setBackgroundColor(QColor(2, 2, 2));

    editorService.undo();
    QCOMPARE(editorService.mindMapData()->backgroundColor(), QColor(1, 1, 1));

    editorService.undo();
    QCOMPARE(editorService.mindMapData()->backgroundColor(), QColor(0, 0, 0));

    editorService.redo();
    QCOMPARE(editorService.mindMapData()->backgroundColor(), QColor(1, 1, 1));
}

void EditorServiceTest::testUndoCornerRadius()
{
    EditorService editorService;

    editorService.setMindMapData(std::make_shared<MindMapData>());
    editorService.mindMapData()->setCornerRadius(12);

    editorService.saveUndoPoint();
    editorService.mindMapData()->setCornerRadius(11);

    editorService.saveUndoPoint();
    editorService.mindMapData()->setCornerRadius(10);

    editorService.undo();
    QCOMPARE(editorService.mindMapData()->cornerRadius(), 11);

    editorService.undo();
    QCOMPARE(editorService.mindMapData()->cornerRadius(), 12);

    editorService.redo();
    QCOMPARE(editorService.mindMapData()->cornerRadius(), 11);
}

void EditorServiceTest::testUndoEdgeColor()
{
    EditorService editorService;

    editorService.setMindMapData(std::make_shared<MindMapData>());
    editorService.mindMapData()->setEdgeColor({ 1, 2, 3 });

    editorService.saveUndoPoint();
    editorService.mindMapData()->setEdgeColor({ 2, 3, 4 });

    editorService.saveUndoPoint();
    editorService.mindMapData()->setEdgeColor({ 4, 5, 6 });

    editorService.undo();
    QCOMPARE(editorService.mindMapData()->edgeColor(), QColor(2, 3, 4));

    editorService.undo();
    QCOMPARE(editorService.mindMapData()->edgeColor(), QColor(1, 2, 3));

    editorService.redo();
    QCOMPARE(editorService.mindMapData()->edgeColor(), QColor(2, 3, 4));
}

void EditorServiceTest::testUndoEdgeDashedLine()
{
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());

    const auto node0 = std::make_shared<Node>();
    editorService.mindMapData()->graph().addNode(node0);
    const auto node1 = std::make_shared<Node>();
    editorService.mindMapData()->graph().addNode(node1);
    const auto edge01 = std::make_shared<Edge>(node0, node1);
    editorService.addEdge(edge01);

    edge01->setDashedLine(false);

    editorService.saveUndoPoint();

    edge01->setDashedLine(true);

    editorService.undo();

    QCOMPARE(editorService.mindMapData()->graph().getEdges().at(0)->dashedLine(), false);

    editorService.redo();

    QCOMPARE(editorService.mindMapData()->graph().getEdges().at(0)->dashedLine(), true);
}

void EditorServiceTest::testUndoEdgeWidth()
{
    EditorService editorService;

    editorService.setMindMapData(std::make_shared<MindMapData>());
    editorService.mindMapData()->setEdgeWidth(1.0);

    editorService.saveUndoPoint();
    editorService.mindMapData()->setEdgeWidth(2.0);

    editorService.saveUndoPoint();
    editorService.mindMapData()->setEdgeWidth(3.0);

    editorService.undo();
    QCOMPARE(editorService.mindMapData()->edgeWidth(), 2.0);

    editorService.undo();
    QCOMPARE(editorService.mindMapData()->edgeWidth(), 1.0);

    editorService.redo();
    QCOMPARE(editorService.mindMapData()->edgeWidth(), 2.0);
}

void EditorServiceTest::testUndoEdgeText()
{
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());

    const auto node0 = std::make_shared<Node>();
    editorService.mindMapData()->graph().addNode(node0);
    const auto node1 = std::make_shared<Node>();
    editorService.mindMapData()->graph().addNode(node1);

    const auto edge01 = std::make_shared<Edge>(node0, node1);
    editorService.addEdge(edge01);

    const auto text0 = "First";
    edge01->setText(text0);

    editorService.saveUndoPoint();

    const auto text1 = "Second";
    edge01->setText(text1);

    editorService.undo();

    QCOMPARE(editorService.mindMapData()->graph().getEdges().at(0)->text(), QString(text0));

    editorService.redo();

    QCOMPARE(editorService.mindMapData()->graph().getEdges().at(0)->text(), QString(text1));
}

void EditorServiceTest::testUndoEdgeReversed()
{
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());

    const auto node0 = std::make_shared<Node>();
    editorService.mindMapData()->graph().addNode(node0);
    const auto node1 = std::make_shared<Node>();
    editorService.mindMapData()->graph().addNode(node1);

    const auto edge01 = std::make_shared<Edge>(node0, node1);

    editorService.addEdge(edge01);

    edge01->setReversed(false);

    editorService.saveUndoPoint();

    edge01->setReversed(true);

    editorService.undo();

    QCOMPARE(editorService.mindMapData()->graph().getEdges().at(0)->reversed(), false);

    editorService.redo();

    QCOMPARE(editorService.mindMapData()->graph().getEdges().at(0)->reversed(), true);
}

void EditorServiceTest::testUndoFontChange()
{
    EditorService editorService;

    editorService.setMindMapData(std::make_shared<MindMapData>());
    const auto font = editorService.mindMapData()->font();
    editorService.saveUndoPoint();
    QFont newFont("Foo");
    editorService.mindMapData()->changeFont(newFont);

    QCOMPARE(editorService.mindMapData()->font().family(), newFont.family());

    editorService.undo();

    QCOMPARE(editorService.mindMapData()->font().family(), font.family());

    editorService.redo();

    QCOMPARE(editorService.mindMapData()->font().family(), newFont.family());
}

void EditorServiceTest::testUndoGroupConnection()
{
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());

    const auto node0 = editorService.addNodeAt(QPointF(0, 0));
    const auto node1 = editorService.addNodeAt(QPointF(1, 1));
    const auto node2 = editorService.addNodeAt(QPointF(2, 2));

    editorService.toggleNodeInSelectionGroup(*node0);
    editorService.toggleNodeInSelectionGroup(*node2);
    editorService.toggleNodeInSelectionGroup(*node1);

    editorService.saveUndoPoint();

    const auto edges = editorService.connectSelectedNodes();

    QCOMPARE(editorService.areSelectedNodesConnectable(), false);

    editorService.undo();

    const auto undoneNode0 = editorService.getNodeByIndex(node0->index());
    const auto undoneNode1 = editorService.getNodeByIndex(node1->index());
    const auto undoneNode2 = editorService.getNodeByIndex(node2->index());

    editorService.toggleNodeInSelectionGroup(*undoneNode0);
    editorService.toggleNodeInSelectionGroup(*undoneNode2);
    editorService.toggleNodeInSelectionGroup(*undoneNode1);

    QCOMPARE(editorService.areSelectedNodesConnectable(), true);
}

void EditorServiceTest::testUndoGroupDisconnection()
{
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());

    const auto node0 = editorService.addNodeAt(QPointF(0, 0));
    const auto node1 = editorService.addNodeAt(QPointF(1, 1));
    const auto node2 = editorService.addNodeAt(QPointF(2, 2));

    editorService.toggleNodeInSelectionGroup(*node0);
    editorService.toggleNodeInSelectionGroup(*node2);
    editorService.toggleNodeInSelectionGroup(*node1);

    editorService.connectSelectedNodes();

    editorService.saveUndoPoint();

    editorService.disconnectSelectedNodes();

    QCOMPARE(editorService.areSelectedNodesDisconnectable(), false);

    editorService.undo();

    const auto undoneNode0 = editorService.getNodeByIndex(node0->index());
    const auto undoneNode1 = editorService.getNodeByIndex(node1->index());
    const auto undoneNode2 = editorService.getNodeByIndex(node2->index());

    editorService.toggleNodeInSelectionGroup(*undoneNode0);
    editorService.toggleNodeInSelectionGroup(*undoneNode2);
    editorService.toggleNodeInSelectionGroup(*undoneNode1);

    QCOMPARE(editorService.areSelectedNodesDisconnectable(), true);
}

void EditorServiceTest::testUndoNodeColor()
{
    const auto data = std::make_shared<MindMapData>();
    const auto node = std::make_shared<Node>();
    data->graph().addNode(node);

    EditorService editorService;
    editorService.setMindMapData(data);

    const QColor color0(1, 1, 1);
    node->setColor(color0);

    QCOMPARE(node->color(), color0);

    editorService.saveUndoPoint();

    const QColor color(6, 6, 6);
    node->setColor(color);

    QCOMPARE(node->color(), color);

    editorService.undo();

    const auto undoneNode = editorService.getNodeByIndex(node->index());

    QCOMPARE(undoneNode->color(), color0);

    editorService.redo();

    const auto redoneNode = editorService.getNodeByIndex(node->index());

    QCOMPARE(redoneNode->color(), color);
}

void EditorServiceTest::testUndoNodeImage()
{
    const auto data = std::make_shared<MindMapData>();
    const auto node = std::make_shared<Node>();
    data->graph().addNode(node);

    EditorService editorService;
    editorService.setMindMapData(data);

    node->setImageRef(1);

    QCOMPARE(node->imageRef(), size_t { 1 });

    editorService.saveUndoPoint();

    node->setImageRef(2);

    QCOMPARE(node->imageRef(), size_t { 2 });

    editorService.undo();

    const auto undoneNode = editorService.getNodeByIndex(node->index());

    QCOMPARE(undoneNode->imageRef(), size_t { 1 });

    editorService.redo();

    const auto redoneNode = editorService.getNodeByIndex(node->index());

    QCOMPARE(redoneNode->imageRef(), size_t { 2 });
}

void EditorServiceTest::testUndoNodeLocation()
{
    const auto data = std::make_shared<MindMapData>();
    const auto node = std::make_shared<Node>();
    data->graph().addNode(node);

    EditorService editorService;
    editorService.setMindMapData(data);

    const QPointF location0(1, 1);
    node->setLocation(location0);

    QCOMPARE(node->location(), location0);

    editorService.saveUndoPoint();

    const QPointF location(666, 666);
    node->setLocation(location);

    QCOMPARE(node->location(), location);

    editorService.undo();

    const auto undoneNode = editorService.getNodeByIndex(node->index());

    QCOMPARE(undoneNode->location(), location0);

    editorService.redo();

    const auto redoneNode = editorService.getNodeByIndex(node->index());

    QCOMPARE(redoneNode->location(), location);
}

void EditorServiceTest::testUndoNodeTextColor()
{
    const auto data = std::make_shared<MindMapData>();
    const auto node = std::make_shared<Node>();
    data->graph().addNode(node);

    EditorService editorService;
    editorService.setMindMapData(data);

    const QColor color0(1, 1, 1);
    node->setTextColor(color0);

    editorService.saveUndoPoint();
    const QColor color(6, 6, 6);
    node->setTextColor(color);

    editorService.undo();

    const auto undoneNode = editorService.getNodeByIndex(node->index());

    QCOMPARE(undoneNode->textColor(), color0);

    editorService.redo();

    const auto redoneNode = editorService.getNodeByIndex(node->index());

    QCOMPARE(redoneNode->textColor(), color);
}

void EditorServiceTest::testUndoTextSize()
{
    EditorService editorService;

    editorService.setMindMapData(std::make_shared<MindMapData>());
    editorService.mindMapData()->setTextSize(42);

    editorService.saveUndoPoint();
    editorService.mindMapData()->setTextSize(41);

    editorService.saveUndoPoint();
    editorService.mindMapData()->setTextSize(40);

    editorService.undo();
    QCOMPARE(editorService.mindMapData()->textSize(), 41);

    editorService.undo();
    QCOMPARE(editorService.mindMapData()->textSize(), 42);

    editorService.redo();
    QCOMPARE(editorService.mindMapData()->textSize(), 41);
}

void EditorServiceTest::testUndoState()
{
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());

    const auto node0 = editorService.addNodeAt(QPointF(0, 0));
    const auto node1 = editorService.addNodeAt(QPointF(100, 0));
    const auto edge01 = editorService.addEdge(std::make_shared<Edge>(node0, node1));

    editorService.toggleNodeInSelectionGroup(*node0);
    editorService.setSelectedEdge(edge01.get());
    editorService.saveUndoPoint();

    QCOMPARE(editorService.isUndoable(), true);

    editorService.undo();

    QCOMPARE(editorService.isUndoable(), false);
    QCOMPARE(editorService.selectionGroupSize(), size_t(0));
    QCOMPARE(editorService.selectedEdge(), nullptr);
    QVERIFY(!editorService.selectedNode().has_value());
}

void EditorServiceTest::testRedoState()
{
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());

    const auto node0 = editorService.addNodeAt(QPointF(0, 0));
    const auto node1 = editorService.addNodeAt(QPointF(100, 0));
    const auto edge01 = editorService.addEdge(std::make_shared<Edge>(node0, node1));

    editorService.toggleNodeInSelectionGroup(*node0);
    editorService.setSelectedEdge(edge01.get());
    editorService.saveUndoPoint();

    QCOMPARE(editorService.isUndoable(), true);

    editorService.undo();

    editorService.toggleNodeInSelectionGroup(*node0);
    editorService.setSelectedEdge(edge01.get());

    editorService.redo();

    QCOMPARE(editorService.isUndoable(), true);
    QCOMPARE(editorService.selectionGroupSize(), size_t(0));
    QCOMPARE(editorService.selectedEdge(), nullptr);
    QVERIFY(!editorService.selectedNode().has_value());

    editorService.undo();
    editorService.saveUndoPoint();

    QCOMPARE(editorService.isRedoable(), false);
}

void EditorServiceTest::testUndoStackResetOnNewDesign()
{
    EditorService editorService;

    editorService.setMindMapData(std::make_shared<MindMapData>());

    QCOMPARE(editorService.isUndoable(), false);

    editorService.saveUndoPoint();
    editorService.addNodeAt(QPointF(0, 0));
    QCOMPARE(editorService.isUndoable(), true);

    editorService.setMindMapData(std::make_shared<MindMapData>());

    QCOMPARE(editorService.isUndoable(), false);
}

void EditorServiceTest::testUndoStackResetOnLoadDesign()
{
    EditorService editorService;

    editorService.setMindMapData(std::make_shared<MindMapData>());

    QCOMPARE(editorService.isUndoable(), false);

    editorService.saveUndoPoint();
    editorService.addNodeAt(QPointF(0, 0));
    QCOMPARE(editorService.isUndoable(), true);

    editorService.loadMindMapData("dummy");

    QCOMPARE(editorService.isUndoable(), false);
}

void EditorServiceTest::testUndoModificationFlagOnNewDesign()
{
    EditorService editorService;

    editorService.setMindMapData(std::make_shared<MindMapData>());

    QCOMPARE(editorService.isModified(), false);

    editorService.saveUndoPoint();
    editorService.addNodeAt(QPointF(0, 0));
    QCOMPARE(editorService.isModified(), true);

    editorService.setMindMapData(std::make_shared<MindMapData>());

    QCOMPARE(editorService.isModified(), false);
}

void EditorServiceTest::testUndoModificationFlagOnLoadDesign()
{
    EditorService editorService;

    editorService.setMindMapData(std::make_shared<MindMapData>());

    QCOMPARE(editorService.isModified(), false);

    editorService.saveUndoPoint();
    editorService.addNodeAt(QPointF(0, 0));
    QCOMPARE(editorService.isModified(), true);

    editorService.loadMindMapData("dummy");

    QCOMPARE(editorService.isModified(), false);
}

QTEST_GUILESS_MAIN(EditorServiceTest)
