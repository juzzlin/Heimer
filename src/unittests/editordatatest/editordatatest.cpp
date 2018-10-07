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

#include "editordatatest.hpp"

#include "editordata.hpp"
#include "serializer.hpp"
#include "mindmapdata.hpp"
#include "nodebase.hpp"

EditorDataTest::EditorDataTest()
{
}

void EditorDataTest::testUndoSimple()
{
    EditorData editorData;

    editorData.setMindMapData(std::make_shared<MindMapData>());
    editorData.addNodeAt(QPointF(0, 0));
    QCOMPARE(editorData.isUndoable(), false);
    QCOMPARE(editorData.isModified(), false);

    editorData.saveUndoPoint();
    QCOMPARE(editorData.isUndoable(), true);
    QCOMPARE(editorData.isModified(), true); // Currently saveUndoPoint() triggers isModified

    editorData.addNodeAt(QPointF(1, 1));
    QCOMPARE(editorData.mindMapData()->graph().numNodes(), 2);

    editorData.undo();
    QCOMPARE(editorData.isUndoable(), false);
    QCOMPARE(editorData.mindMapData()->graph().numNodes(), 1);

    editorData.undo();
    QCOMPARE(editorData.isUndoable(), false);
    QCOMPARE(editorData.mindMapData()->graph().numNodes(), 1);
}

void EditorDataTest::testRedoSimple()
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
}

void EditorDataTest::testUndoNodeColor()
{
    auto data = std::make_shared<MindMapData>();
    auto node = std::make_shared<Node>();
    data->graph().addNode(node);

    EditorData editorData;
    editorData.setMindMapData(data);

    editorData.saveUndoPoint();
    const QColor color(1, 1, 1);
    node->setColor(color);

    editorData.undo();

    QCOMPARE(node->color(), color);
}

void EditorDataTest::testUndoNodeTextColor()
{
    auto data = std::make_shared<MindMapData>();
    auto node = std::make_shared<Node>();
    data->graph().addNode(node);

    EditorData editorData;
    editorData.setMindMapData(data);

    editorData.saveUndoPoint();
    const QColor color(1, 1, 1);
    node->setTextColor(color);

    editorData.undo();

    QCOMPARE(node->textColor(), color);
}

QTEST_GUILESS_MAIN(EditorDataTest)
