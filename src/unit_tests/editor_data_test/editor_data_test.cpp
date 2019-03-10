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

#include "editor_data.hpp"
#include "serializer.hpp"
#include "mind_map_data.hpp"
#include "node_base.hpp"

EditorDataTest::EditorDataTest()
{
}

void EditorDataTest::testGroupMove()
{
    EditorData editorData;
    editorData.setMindMapData(std::make_shared<MindMapData>());
    auto node0 = editorData.addNodeAt(QPointF(0, 0));
    auto node1 = editorData.addNodeAt(QPointF(1, 1));

    editorData.toggleNodeInSelectionGroup(*node0);
    editorData.toggleNodeInSelectionGroup(*node1);

    editorData.moveSelectionGroup(*node0, {1, 1});

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

    editorData.toggleNodeInSelectionGroup(*node0);
    editorData.toggleNodeInSelectionGroup(*node1);

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

    editorData.toggleNodeInSelectionGroup(*node0);
    editorData.setSelectedNode(node0.get());
    editorData.saveUndoPoint();

    QCOMPARE(editorData.isUndoable(), true);

    editorData.loadMindMapData(""); // Doesn't really load anything if in unit tests

    QCOMPARE(editorData.isUndoable(), false);
    QCOMPARE(editorData.selectionGroupSize(), size_t(0));
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
    QCOMPARE(editorData.mindMapData()->graph().numNodes(), 2);

    editorData.undo();
    QCOMPARE(editorData.isUndoable(), true);
    QCOMPARE(editorData.mindMapData()->graph().numNodes(), 1);

    editorData.undo();
    QCOMPARE(editorData.isUndoable(), false);
    QCOMPARE(editorData.mindMapData()->graph().numNodes(), 0);

    editorData.undo();
    QCOMPARE(editorData.isUndoable(), false);
    QCOMPARE(editorData.mindMapData()->graph().numNodes(), 0);
}

void EditorDataTest::testRedoAddNodes()
{
    EditorData editorData;

    editorData.setMindMapData(std::make_shared<MindMapData>());
    editorData.addNodeAt(QPointF(0, 0));
    editorData.saveUndoPoint();
    editorData.addNodeAt(QPointF(1, 1));
    QCOMPARE(editorData.mindMapData()->graph().numNodes(), 2);

    editorData.undo();
    QCOMPARE(editorData.isUndoable(), false);
    QCOMPARE(editorData.mindMapData()->graph().numNodes(), 1);

    editorData.redo();
    QCOMPARE(editorData.isUndoable(), true);
    QCOMPARE(editorData.mindMapData()->graph().numNodes(), 2);
}

void EditorDataTest::testUndoAddEdge()
{
    auto data = std::make_shared<MindMapData>();
    EditorData editorData;
    editorData.setMindMapData(data);

    auto node0 = std::make_shared<Node>();
    data->graph().addNode(node0);
    auto node1 = std::make_shared<Node>();
    data->graph().addNode(node1);
    auto node2 = std::make_shared<Node>();
    data->graph().addNode(node2);

    auto edge01 = std::make_shared<Edge>(*node0, *node1);

    editorData.saveUndoPoint();

    editorData.addEdge(edge01);

    auto edge21 = std::make_shared<Edge>(*node2, *node1);

    editorData.saveUndoPoint();

    editorData.addEdge(edge21);

    QCOMPARE(editorData.mindMapData()->graph().areDirectlyConnected(node0, node1), true);
    QCOMPARE(editorData.mindMapData()->graph().areDirectlyConnected(node2, node1), true);

    editorData.undo();

    auto && undoneNode0 = editorData.getNodeByIndex(node0->index());
    auto && undoneNode1 = editorData.getNodeByIndex(node1->index());
    auto && undoneNode2 = editorData.getNodeByIndex(node2->index());

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

void EditorDataTest::testUndoEdgeColor()
{
    EditorData editorData;

    editorData.setMindMapData(std::make_shared<MindMapData>());
    editorData.mindMapData()->setEdgeColor({1, 2, 3});

    editorData.saveUndoPoint();
    editorData.mindMapData()->setEdgeColor({2, 3, 4});

    editorData.saveUndoPoint();
    editorData.mindMapData()->setEdgeColor({4, 5, 6});

    editorData.undo();
    QCOMPARE(editorData.mindMapData()->edgeColor(), QColor(2, 3, 4));

    editorData.undo();
    QCOMPARE(editorData.mindMapData()->edgeColor(), QColor(1, 2, 3));

    editorData.redo();
    QCOMPARE(editorData.mindMapData()->edgeColor(), QColor(2, 3, 4));
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

void EditorDataTest::testUndoNodeColor()
{
    auto data = std::make_shared<MindMapData>();
    auto node = std::make_shared<Node>();
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

    auto && undoneNode = editorData.getNodeByIndex(node->index());

    QCOMPARE(undoneNode->color(), color0);

    editorData.redo();

    auto && redoneNode = editorData.getNodeByIndex(node->index());

    QCOMPARE(redoneNode->color(), color);
}

void EditorDataTest::testUndoNodeLocation()
{
    auto data = std::make_shared<MindMapData>();
    auto node = std::make_shared<Node>();
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

    auto && undoneNode = editorData.getNodeByIndex(node->index());

    QCOMPARE(undoneNode->location(), location0);

    editorData.redo();

    auto && redoneNode = editorData.getNodeByIndex(node->index());

    QCOMPARE(redoneNode->location(), location);
}

void EditorDataTest::testUndoNodeTextColor()
{
    auto data = std::make_shared<MindMapData>();
    auto node = std::make_shared<Node>();
    data->graph().addNode(node);

    EditorData editorData;
    editorData.setMindMapData(data);

    const QColor color0(1, 1, 1);
    node->setTextColor(color0);

    editorData.saveUndoPoint();
    const QColor color(6, 6, 6);
    node->setTextColor(color);

    editorData.undo();

    auto && undoneNode = editorData.getNodeByIndex(node->index());

    QCOMPARE(undoneNode->textColor(), color0);

    editorData.redo();

    auto && redoneNode = editorData.getNodeByIndex(node->index());

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

    editorData.toggleNodeInSelectionGroup(*node0);
    editorData.setSelectedNode(node0.get());
    editorData.saveUndoPoint();

    QCOMPARE(editorData.isUndoable(), true);

    editorData.undo();

    QCOMPARE(editorData.isUndoable(), false);
    QCOMPARE(editorData.selectionGroupSize(), size_t(0));
    QCOMPARE(editorData.selectedNode(), nullptr);
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
