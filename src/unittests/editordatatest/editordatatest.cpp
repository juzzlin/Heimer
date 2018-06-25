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

#include "mediator_mock.hpp"

EditorDataTest::EditorDataTest()
{
}

void EditorDataTest::testUndoSimple()
{
    Mediator mediator;
    EditorData editorData(mediator);

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
    Mediator mediator;
    EditorData editorData(mediator);

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

QTEST_GUILESS_MAIN(EditorDataTest)
