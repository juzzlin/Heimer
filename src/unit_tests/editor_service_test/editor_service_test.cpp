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
#include "../../common/constants.hpp"
#include "../../common/test_mode.hpp"
#include "../../domain/graph.hpp"
#include "../../domain/mind_map_data.hpp"
#include "../../domain/shadow_effect_params.hpp"
#include "../../domain/theme.hpp"

using SceneItems::Edge;
using SceneItems::EdgeModel;
using SceneItems::Node;
using SceneItems::NodeModel;

EditorServiceTest::EditorServiceTest()
{
    TestMode::setEnabled(true);
}

void EditorServiceTest::test_addAndDeleteEdge()
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

void EditorServiceTest::test_groupConnection()
{
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());
    QCOMPARE(editorService.areSelectedNodesConnectable(), false);

    const auto node0 = editorService.addNodeAt(QPointF { 0, 0 });
    const auto node1 = editorService.addNodeAt(QPointF { 1, 1 });
    const auto node2 = editorService.addNodeAt(QPointF { 2, 2 });

    QCOMPARE(editorService.nodeSelectionGroupSize(), size_t(0));
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

void EditorServiceTest::test_groupDisconnection()
{
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());
    QCOMPARE(editorService.areSelectedNodesDisconnectable(), false);

    const auto node0 = editorService.addNodeAt(QPointF { 0, 0 });
    const auto node1 = editorService.addNodeAt(QPointF { 1, 1 });
    const auto node2 = editorService.addNodeAt(QPointF { 2, 2 });

    QCOMPARE(editorService.nodeSelectionGroupSize(), size_t(0));
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

void EditorServiceTest::test_groupDeletionOfEdges_shouldDeleteSelectedEdges()
{
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());
    const auto node0 = editorService.addNodeAt(QPointF { 0, 0 });
    const auto node1 = editorService.addNodeAt(QPointF { 1, 1 });
    const auto edge0 = std::make_shared<Edge>(node0, node1);
    editorService.addEdge(edge0);
    editorService.toggleEdgeInSelectionGroup(*edge0);

    editorService.deleteSelectedEdges();

    QCOMPARE(editorService.edgeSelectionGroupSize(), size_t(0));
    QVERIFY(editorService.mindMapData()->graph().getEdges().empty());
}

void EditorServiceTest::test_groupDeletionOfNodes_shouldDeleteSelectedNodes()
{
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());
    const auto node0 = editorService.addNodeAt(QPointF { 0, 0 });
    const auto node1 = editorService.addNodeAt(QPointF { 1, 1 });

    editorService.toggleNodeInSelectionGroup(*node0);
    editorService.toggleNodeInSelectionGroup(*node1);

    editorService.deleteSelectedNodes();

    QCOMPARE(editorService.nodeSelectionGroupSize(), size_t(0));
    QVERIFY(editorService.mindMapData()->graph().getNodes().empty());
}

void EditorServiceTest::test_groupMove()
{
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());
    const auto node0 = editorService.addNodeAt(QPointF { 0, 0 });
    const auto node1 = editorService.addNodeAt(QPointF { 1, 1 });

    editorService.toggleNodeInSelectionGroup(*node0);
    editorService.toggleNodeInSelectionGroup(*node1);

    editorService.moveSelectionGroup(*node0, { 1, 1 });

    QCOMPARE(qFuzzyCompare(node0->location().x(), 1), true);
    QCOMPARE(qFuzzyCompare(node0->location().y(), 1), true);
    QCOMPARE(qFuzzyCompare(node1->location().x(), 2), true);
    QCOMPARE(qFuzzyCompare(node1->location().y(), 2), true);
}

void EditorServiceTest::test_initializationResetsFileName()
{
    EditorService editorService;
    const QString fileName = "dummy";
    editorService.loadMindMapData(fileName); // Doesn't really load anything if in unit tests
    QCOMPARE(editorService.fileName(), fileName);
    editorService.initializeNewMindMap();
    QCOMPARE(editorService.fileName(), QString { "" });
}

void EditorServiceTest::test_groupSelectionOfEdges()
{
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());

    const auto node0 = editorService.addNodeAt(QPointF { 0, 0 });
    const auto node1 = editorService.addNodeAt(QPointF { 1, 1 });
    const auto edge = std::make_shared<Edge>(node0, node1);
    editorService.addEdge(edge);

    QCOMPARE(editorService.edgeSelectionGroupSize(), size_t(0));

    editorService.toggleEdgeInSelectionGroup(*edge);

    QCOMPARE(editorService.edgeSelectionGroupSize(), size_t(1));
    QVERIFY(edge->selected());

    editorService.clearEdgeSelectionGroup();

    QCOMPARE(editorService.edgeSelectionGroupSize(), size_t(0));
    QVERIFY(!edge->selected());

    editorService.addEdgeToSelectionGroup(*edge);

    QCOMPARE(editorService.edgeSelectionGroupSize(), size_t(1));
    QVERIFY(edge->selected());

    editorService.loadMindMapData(""); // Doesn't really load anything if in unit tests

    QCOMPARE(editorService.edgeSelectionGroupSize(), size_t(0));
}

void EditorServiceTest::test_groupSelectionOfNodes()
{
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());

    const auto node0 = editorService.addNodeAt(QPointF { 0, 0 });
    const auto node1 = editorService.addNodeAt(QPointF { 1, 1 });

    QCOMPARE(editorService.nodeSelectionGroupSize(), size_t(0));

    editorService.toggleNodeInSelectionGroup(*node0);
    editorService.toggleNodeInSelectionGroup(*node1);

    QCOMPARE(editorService.nodeSelectionGroupSize(), size_t(2));
    QVERIFY(node0->selected());
    QVERIFY(node1->selected());

    editorService.clearNodeSelectionGroup();

    QCOMPARE(editorService.nodeSelectionGroupSize(), size_t(0));
    QVERIFY(!node0->selected());
    QVERIFY(!node1->selected());

    editorService.addNodeToSelectionGroup(*node0);
    editorService.addNodeToSelectionGroup(*node1);

    QCOMPARE(editorService.nodeSelectionGroupSize(), size_t(2));
    QVERIFY(node0->selected());
    QVERIFY(node1->selected());

    editorService.loadMindMapData(""); // Doesn't really load anything if in unit tests

    QCOMPARE(editorService.nodeSelectionGroupSize(), size_t(0));
}

void EditorServiceTest::test_loadState()
{
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());

    const auto node0 = editorService.addNodeAt(QPointF { 0, 0 });
    const auto node1 = editorService.addNodeAt(QPointF { 100, 0 });
    const auto edge01 = editorService.addEdge(std::make_shared<Edge>(node0, node1));

    editorService.toggleNodeInSelectionGroup(*node0);
    editorService.addEdgeToSelectionGroup(*edge01.get());
    editorService.saveUndoPoint();

    QCOMPARE(editorService.isUndoable(), true);

    const QString fileName = "dummy";
    editorService.loadMindMapData(fileName); // Doesn't really load anything if in unit tests

    QCOMPARE(editorService.fileName(), fileName);
    QCOMPARE(editorService.isUndoable(), false);
    QCOMPARE(editorService.nodeSelectionGroupSize(), size_t(0));
    QVERIFY(!editorService.selectedEdge().has_value());
    QVERIFY(!editorService.selectedNode().has_value());
}

void EditorServiceTest::test_undoAddNodes()
{
    EditorService editorService;

    editorService.setMindMapData(std::make_shared<MindMapData>());

    QCOMPARE(editorService.isUndoable(), false);
    QCOMPARE(editorService.isModified(), false);

    editorService.saveUndoPoint();
    editorService.addNodeAt(QPointF { 0, 0 });
    QCOMPARE(editorService.isUndoable(), true);
    QCOMPARE(editorService.isModified(), true);

    editorService.saveUndoPoint();
    QCOMPARE(editorService.isUndoable(), true);
    QCOMPARE(editorService.isModified(), true); // Currently saveUndoPoint() triggers isModified

    editorService.addNodeAt(QPointF { 1, 1 });
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

void EditorServiceTest::test_redoAddNodes()
{
    EditorService editorService;

    editorService.setMindMapData(std::make_shared<MindMapData>());
    editorService.addNodeAt(QPointF { 0, 0 });
    editorService.saveUndoPoint();
    editorService.addNodeAt(QPointF { 1, 1 });
    QCOMPARE(editorService.mindMapData()->graph().nodeCount(), static_cast<size_t>(2));

    editorService.undo();
    QCOMPARE(editorService.isUndoable(), false);
    QCOMPARE(editorService.mindMapData()->graph().nodeCount(), static_cast<size_t>(1));

    editorService.redo();
    QCOMPARE(editorService.isUndoable(), true);
    QCOMPARE(editorService.mindMapData()->graph().nodeCount(), static_cast<size_t>(2));
}

void EditorServiceTest::test_saveUndoPoint_quickSuccessionShouldBeMerged()
{
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());
    editorService.setUndoPointMergingEnabled(true);

    editorService.saveUndoPoint();
    editorService.addNodeAt(QPointF { 0, 0 });
    editorService.saveUndoPoint(); // Merged with the previous one
    editorService.addNodeAt(QPointF { 1, 1 });

    editorService.undo();
    QCOMPARE(editorService.mindMapData()->graph().nodeCount(), static_cast<size_t>(0));
    QCOMPARE(editorService.isUndoable(), false);
}

void EditorServiceTest::test_undoRedo_quickSuccessionShouldNotLoseStates()
{
    // Regression test for https://github.com/juzzlin/Heimer/issues/163
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());
    for (int i = 0; i < 3; i++) {
        editorService.saveUndoPoint();
        editorService.addNodeAt(QPointF { static_cast<qreal>(i), static_cast<qreal>(i) });
    }
    QCOMPARE(editorService.mindMapData()->graph().nodeCount(), static_cast<size_t>(3));

    // The undo/redo cycles below happen well within the merging delay
    editorService.setUndoPointMergingEnabled(true);
    for (int cycle = 0; cycle < 2; cycle++) {
        for (size_t expectedNodeCount : { 2, 1, 0 }) {
            QVERIFY(editorService.isUndoable());
            editorService.undo();
            QCOMPARE(editorService.mindMapData()->graph().nodeCount(), expectedNodeCount);
        }
        QVERIFY(!editorService.isUndoable());

        for (size_t expectedNodeCount : { 1, 2, 3 }) {
            QVERIFY(editorService.isRedoable());
            editorService.redo();
            QCOMPARE(editorService.mindMapData()->graph().nodeCount(), expectedNodeCount);
        }
        QVERIFY(!editorService.isRedoable());
    }
}

void EditorServiceTest::test_undo_modificationRightAfterUndoShouldGetOwnUndoPoint()
{
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());
    editorService.saveUndoPoint();
    editorService.addNodeAt(QPointF { 0, 0 });

    editorService.setUndoPointMergingEnabled(true);
    editorService.saveUndoPoint();
    editorService.addNodeAt(QPointF { 1, 1 });
    editorService.undo();
    QCOMPARE(editorService.mindMapData()->graph().nodeCount(), static_cast<size_t>(1));

    // Must not be merged with the undo point saved before undo
    editorService.saveUndoPoint();
    editorService.addNodeAt(QPointF { 2, 2 });
    editorService.undo();
    QCOMPARE(editorService.mindMapData()->graph().nodeCount(), static_cast<size_t>(1));
}

void EditorServiceTest::test_textSearch()
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

void EditorServiceTest::test_undoAddEdge()
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

void EditorServiceTest::test_undoArrowMode()
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

void EditorServiceTest::test_undoArrowSize()
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

void EditorServiceTest::test_undoDeleteEdge()
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

void EditorServiceTest::test_undoBackgroundColor()
{
    EditorService editorService;

    editorService.setMindMapData(std::make_shared<MindMapData>());
    editorService.mindMapData()->setBackgroundColor(QColor { 0, 0, 0 });

    editorService.saveUndoPoint();
    editorService.mindMapData()->setBackgroundColor(QColor { 1, 1, 1 });

    editorService.saveUndoPoint();
    editorService.mindMapData()->setBackgroundColor(QColor { 2, 2, 2 });

    editorService.undo();
    QCOMPARE(editorService.mindMapData()->backgroundColor(), (QColor { 1, 1, 1 }));

    editorService.undo();
    QCOMPARE(editorService.mindMapData()->backgroundColor(), (QColor { 0, 0, 0 }));

    editorService.redo();
    QCOMPARE(editorService.mindMapData()->backgroundColor(), (QColor { 1, 1, 1 }));
}

void EditorServiceTest::test_undoCornerRadius()
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

void EditorServiceTest::test_undoEdgeColor()
{
    EditorService editorService;

    editorService.setMindMapData(std::make_shared<MindMapData>());
    editorService.mindMapData()->setEdgeColor({ 1, 2, 3 });

    editorService.saveUndoPoint();
    editorService.mindMapData()->setEdgeColor({ 2, 3, 4 });

    editorService.saveUndoPoint();
    editorService.mindMapData()->setEdgeColor({ 4, 5, 6 });

    editorService.undo();
    QCOMPARE(editorService.mindMapData()->edgeColor(), (QColor { 2, 3, 4 }));

    editorService.undo();
    QCOMPARE(editorService.mindMapData()->edgeColor(), (QColor { 1, 2, 3 }));

    editorService.redo();
    QCOMPARE(editorService.mindMapData()->edgeColor(), (QColor { 2, 3, 4 }));
}

void EditorServiceTest::test_undoEdgeDashedLine()
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

void EditorServiceTest::test_undoEdgeWidth()
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

void EditorServiceTest::test_undoEdgeText()
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

    QCOMPARE(editorService.mindMapData()->graph().getEdges().at(0)->text(), QString { text0 });

    editorService.redo();

    QCOMPARE(editorService.mindMapData()->graph().getEdges().at(0)->text(), QString { text1 });
}

void EditorServiceTest::test_undoEdgeReversed()
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

void EditorServiceTest::test_undoFontChange()
{
    EditorService editorService;

    editorService.setMindMapData(std::make_shared<MindMapData>());
    const auto font = editorService.mindMapData()->font();
    editorService.saveUndoPoint();
    QFont newFont { "Foo" };
    editorService.mindMapData()->changeFont(newFont);

    QCOMPARE(editorService.mindMapData()->font().family(), newFont.family());

    editorService.undo();

    QCOMPARE(editorService.mindMapData()->font().family(), font.family());

    editorService.redo();

    QCOMPARE(editorService.mindMapData()->font().family(), newFont.family());
}

void EditorServiceTest::test_undoGroupConnection()
{
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());

    const auto node0 = editorService.addNodeAt(QPointF { 0, 0 });
    const auto node1 = editorService.addNodeAt(QPointF { 1, 1 });
    const auto node2 = editorService.addNodeAt(QPointF { 2, 2 });

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

void EditorServiceTest::test_undoGroupDisconnection()
{
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());

    const auto node0 = editorService.addNodeAt(QPointF { 0, 0 });
    const auto node1 = editorService.addNodeAt(QPointF { 1, 1 });
    const auto node2 = editorService.addNodeAt(QPointF { 2, 2 });

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

void EditorServiceTest::test_undoNodeColor()
{
    const auto data = std::make_shared<MindMapData>();
    const auto node = std::make_shared<Node>();
    data->graph().addNode(node);

    EditorService editorService;
    editorService.setMindMapData(data);

    const QColor color0 { 1, 1, 1 };
    node->setColor(color0);

    QCOMPARE(node->color(), color0);

    editorService.saveUndoPoint();

    const QColor color { 6, 6, 6 };
    node->setColor(color);

    QCOMPARE(node->color(), color);

    editorService.undo();

    const auto undoneNode = editorService.getNodeByIndex(node->index());

    QCOMPARE(undoneNode->color(), color0);

    editorService.redo();

    const auto redoneNode = editorService.getNodeByIndex(node->index());

    QCOMPARE(redoneNode->color(), color);
}

void EditorServiceTest::test_undoNodeImage()
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

void EditorServiceTest::test_undoNodeLocation()
{
    const auto data = std::make_shared<MindMapData>();
    const auto node = std::make_shared<Node>();
    data->graph().addNode(node);

    EditorService editorService;
    editorService.setMindMapData(data);

    const QPointF location0 { 1, 1 };
    node->setLocation(location0);

    QCOMPARE(node->location(), location0);

    editorService.saveUndoPoint();

    const QPointF location { 666, 666 };
    node->setLocation(location);

    QCOMPARE(node->location(), location);

    editorService.undo();

    const auto undoneNode = editorService.getNodeByIndex(node->index());

    QCOMPARE(undoneNode->location(), location0);

    editorService.redo();

    const auto redoneNode = editorService.getNodeByIndex(node->index());

    QCOMPARE(redoneNode->location(), location);
}

void EditorServiceTest::test_undoNodeTextColor()
{
    const auto data = std::make_shared<MindMapData>();
    const auto node = std::make_shared<Node>();
    data->graph().addNode(node);

    EditorService editorService;
    editorService.setMindMapData(data);

    const QColor color0 { 1, 1, 1 };
    node->setTextColor(color0);

    editorService.saveUndoPoint();
    const QColor color { 6, 6, 6 };
    node->setTextColor(color);

    editorService.undo();

    const auto undoneNode = editorService.getNodeByIndex(node->index());

    QCOMPARE(undoneNode->textColor(), color0);

    editorService.redo();

    const auto redoneNode = editorService.getNodeByIndex(node->index());

    QCOMPARE(redoneNode->textColor(), color);
}

void EditorServiceTest::test_undoTextSize()
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

void EditorServiceTest::test_undoState()
{
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());

    const auto node0 = editorService.addNodeAt(QPointF { 0, 0 });
    const auto node1 = editorService.addNodeAt(QPointF { 100, 0 });
    const auto edge01 = editorService.addEdge(std::make_shared<Edge>(node0, node1));

    editorService.toggleNodeInSelectionGroup(*node0);
    editorService.addEdgeToSelectionGroup(*edge01.get());
    editorService.saveUndoPoint();

    QCOMPARE(editorService.isUndoable(), true);

    editorService.undo();

    QCOMPARE(editorService.isUndoable(), false);
    QCOMPARE(editorService.nodeSelectionGroupSize(), size_t(0));
    QVERIFY(!editorService.selectedEdge().has_value());
    QVERIFY(!editorService.selectedNode().has_value());
}

void EditorServiceTest::test_redoState()
{
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());

    const auto node0 = editorService.addNodeAt(QPointF { 0, 0 });
    const auto node1 = editorService.addNodeAt(QPointF { 100, 0 });
    const auto edge01 = editorService.addEdge(std::make_shared<Edge>(node0, node1));

    editorService.toggleNodeInSelectionGroup(*node0);
    editorService.addEdgeToSelectionGroup(*edge01.get());
    editorService.saveUndoPoint();

    QCOMPARE(editorService.isUndoable(), true);

    editorService.undo();

    editorService.toggleNodeInSelectionGroup(*node0);
    editorService.addEdgeToSelectionGroup(*edge01.get());

    editorService.redo();

    QCOMPARE(editorService.isUndoable(), true);
    QCOMPARE(editorService.nodeSelectionGroupSize(), size_t(0));
    QVERIFY(!editorService.selectedEdge().has_value());
    QVERIFY(!editorService.selectedNode().has_value());

    editorService.undo();
    editorService.saveUndoPoint();

    QCOMPARE(editorService.isRedoable(), false);
}

void EditorServiceTest::test_undoStackResetOnNewDesign()
{
    EditorService editorService;

    editorService.setMindMapData(std::make_shared<MindMapData>());

    QCOMPARE(editorService.isUndoable(), false);

    editorService.saveUndoPoint();
    editorService.addNodeAt(QPointF { 0, 0 });
    QCOMPARE(editorService.isUndoable(), true);

    editorService.setMindMapData(std::make_shared<MindMapData>());

    QCOMPARE(editorService.isUndoable(), false);
}

void EditorServiceTest::test_undoStackResetOnLoadDesign()
{
    EditorService editorService;

    editorService.setMindMapData(std::make_shared<MindMapData>());

    QCOMPARE(editorService.isUndoable(), false);

    editorService.saveUndoPoint();
    editorService.addNodeAt(QPointF { 0, 0 });
    QCOMPARE(editorService.isUndoable(), true);

    editorService.loadMindMapData("dummy");

    QCOMPARE(editorService.isUndoable(), false);
}

void EditorServiceTest::test_undoHistory_shouldBeLimited()
{
    // https://github.com/juzzlin/Heimer/issues/237
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());
    const auto maxHistorySize = Constants::Misc::maxUndoHistorySize();
    const size_t extraUndoPoints = 10;
    for (size_t i = 0; i < maxHistorySize + extraUndoPoints; i++) {
        editorService.saveUndoPoint();
        editorService.addNodeAt(QPointF { static_cast<double>(i), 0 });
    }

    size_t undoCount = 0;
    while (editorService.isUndoable()) {
        editorService.undo();
        undoCount++;
    }

    QCOMPARE(undoCount, maxHistorySize);
    QCOMPARE(editorService.mindMapData()->graph().nodeCount(), extraUndoPoints);
}

void EditorServiceTest::test_addNodeAt_shouldUseThemeNodeColors()
{
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());
    editorService.mindMapData()->setNodeColor({ 1, 2, 3 });
    editorService.mindMapData()->setNodeTextColor({ 4, 5, 6 });

    const auto node = editorService.addNodeAt({ 0, 0 });

    QCOMPARE(node->color(), (QColor { 1, 2, 3 }));
    QCOMPARE(node->textColor(), (QColor { 4, 5, 6 }));
}

void EditorServiceTest::test_undoEdgeLabelColors()
{
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());
    const auto originalLabelColor = editorService.mindMapData()->edgeLabelColor();
    const auto originalLabelTextColor = editorService.mindMapData()->edgeLabelTextColor();

    editorService.saveUndoPoint();
    editorService.mindMapData()->setEdgeLabelColors({ 1, 2, 3 }, { 4, 5, 6 });
    editorService.undo();

    QCOMPARE(editorService.mindMapData()->edgeLabelColor(), originalLabelColor);
    QCOMPARE(editorService.mindMapData()->edgeLabelTextColor(), originalLabelTextColor);

    editorService.redo();

    QCOMPARE(editorService.mindMapData()->edgeLabelColor(), (QColor { 1, 2, 3 }));
    QCOMPARE(editorService.mindMapData()->edgeLabelTextColor(), (QColor { 4, 5, 6 }));
}

void EditorServiceTest::test_undoShadowEffect()
{
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());
    const auto originalShadowEffect = editorService.mindMapData()->shadowEffect();
    const ShadowEffectParams shadowEffect { 1, 2, 3, QColor { 1, 2, 3, 4 }, QColor { 5, 6, 7, 8 } };

    editorService.saveUndoPoint();
    editorService.mindMapData()->setShadowEffect(shadowEffect);
    editorService.undo();

    QVERIFY(editorService.mindMapData()->shadowEffect() == originalShadowEffect);

    editorService.redo();

    QVERIFY(editorService.mindMapData()->shadowEffect() == shadowEffect);
}

void EditorServiceTest::test_applyTheme_shouldBeUndoable()
{
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());
    editorService.addNodeAt({ 0, 0 });
    const auto originalTheme = editorService.mindMapData()->theme();
    Theme theme;
    theme.backgroundColor = { 1, 2, 3 };
    theme.edgeLabelColor = { 4, 5, 6 };
    theme.nodeColor = { 7, 8, 9 };

    editorService.applyTheme(theme, true);

    QVERIFY(editorService.mindMapData()->theme().isVisuallyEqual(theme));
    QCOMPARE(editorService.mindMapData()->graph().getNodes().at(0)->color(), (QColor { 7, 8, 9 }));
    QVERIFY(editorService.isUndoable());

    editorService.undo();

    QVERIFY(editorService.mindMapData()->theme().isVisuallyEqual(originalTheme));
    QCOMPARE(editorService.mindMapData()->graph().getNodes().at(0)->color(), originalTheme.nodeColor);
}

void EditorServiceTest::test_applyTheme_shouldKeepIndividualNodeColorsIfRequested()
{
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());
    const auto node = editorService.addNodeAt({ 0, 0 });
    node->setColor({ 100, 0, 0 });
    Theme theme;
    theme.nodeColor = { 7, 8, 9 };

    editorService.applyTheme(theme, false);

    QCOMPARE(editorService.mindMapData()->nodeColor(), (QColor { 7, 8, 9 }));
    QCOMPARE(node->color(), (QColor { 100, 0, 0 }));
}

void EditorServiceTest::test_hasIndividualNodeColors()
{
    EditorService editorService;
    editorService.setMindMapData(std::make_shared<MindMapData>());
    QVERIFY(!editorService.hasIndividualNodeColors());

    const auto node = editorService.addNodeAt({ 0, 0 });
    QVERIFY(!editorService.hasIndividualNodeColors());

    node->setTextColor({ 1, 2, 3 });
    QVERIFY(editorService.hasIndividualNodeColors());

    node->setTextColor(editorService.mindMapData()->nodeTextColor());
    node->setColor({ 1, 2, 3 });
    QVERIFY(editorService.hasIndividualNodeColors());
}

void EditorServiceTest::test_undoModificationFlagOnNewDesign()
{
    EditorService editorService;

    editorService.setMindMapData(std::make_shared<MindMapData>());

    QCOMPARE(editorService.isModified(), false);

    editorService.saveUndoPoint();
    editorService.addNodeAt(QPointF { 0, 0 });
    QCOMPARE(editorService.isModified(), true);

    editorService.setMindMapData(std::make_shared<MindMapData>());

    QCOMPARE(editorService.isModified(), false);
}

void EditorServiceTest::test_undoModificationFlagOnLoadDesign()
{
    EditorService editorService;

    editorService.setMindMapData(std::make_shared<MindMapData>());

    QCOMPARE(editorService.isModified(), false);

    editorService.saveUndoPoint();
    editorService.addNodeAt(QPointF { 0, 0 });
    QCOMPARE(editorService.isModified(), true);

    editorService.loadMindMapData("dummy");

    QCOMPARE(editorService.isModified(), false);
}

QTEST_GUILESS_MAIN(EditorServiceTest)
