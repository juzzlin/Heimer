// This file is part of Dementia.
// Copyright (C) 2018 Jussi Lind <jussi.lind@iki.fi>
//
// Dementia is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dementia is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dementia. If not, see <http://www.gnu.org/licenses/>.

#ifndef MEDIATOR_HPP
#define MEDIATOR_HPP

#include <QString>
#include <QPointF>

#include "node.hpp"

class DragAndDropStore;
class EditorData;
class EditorScene;
class EditorView;
class MainWindow;
class QGraphicsItem;

/*! Acts as a communication channel between MainWindow and editor components:
 *
 *  - MainWindow <-> Mediator <-> QGraphicsScene / EditorView / EditorData
 *  - EditorView <-> Mediator <-> EditorData
 */
class Mediator
{
public:

    explicit Mediator(MainWindow & mainWindow);

    ~Mediator();

    void addItem(QGraphicsItem & item);

    void center();

    NodeBasePtr createAndAddNode(int sourceNodeIndex, QPointF pos);

    DragAndDropStore & dadStore();

    void enableUndo(bool enable);

    NodeBasePtr getNodeByIndex(int index);

    bool hasNodes() const;

    void initializeNewMindMap();

    void initScene();

    bool isUndoable() const;

    bool isRedoable() const;

    bool openMindMap(QString fileName);

    void redo();

    void removeItem(QGraphicsItem & item);

    void saveUndoPoint();

    void setSelectedNode(Node * node);

    void setupMindMapAfterUndoOrRedo();

    void undo();

    void updateView();

private:

    EditorData * m_editorData;

    EditorScene * m_editorScene;

    EditorView * m_editorView;

    MainWindow & m_mainWindow;
};

#endif // MEDIATOR_HPP
