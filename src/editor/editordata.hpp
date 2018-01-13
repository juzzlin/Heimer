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

#ifndef EDITORDATA_HPP
#define EDITORDATA_HPP

#include <memory>
#include <vector>

#include <QString>
#include <QPointF>

#include "draganddropstore.hpp"
#include "undostack.hpp"
#include "mindmapdata.hpp"

class Mediator;
class Node;
class NodeBase;
class MindMapTile;
class QGraphicsLineItem;

class EditorData
{
public:

    EditorData(Mediator & mediator);

    void addExistingGraphToScene();

    void clearGraph();

    NodeBasePtr createAndAddNodeToGraph(QPointF pos);

    DragAndDropStore & dadStore();

    NodeBasePtr getNodeByIndex(int index);

    bool isUndoable() const;

    bool isRedoable() const;

    bool loadMindMapData(QString fileName);

    MindMapDataPtr mindMapData();

    void redo();

    void removeGraphFromScene();

    bool saveMindMapData();

    bool saveMindMapDataAs(QString fileName);

    void saveUndoPoint();

    void saveRedoPoint();

    void setMindMapData(MindMapDataPtr newMindMapData);

    void setSelectedNode(Node * node);

    Node * selectedNode() const;

    void undo();

private:

    EditorData(const EditorData & e) = delete;
    EditorData & operator= (const EditorData & e) = delete;

    void addNodeToScene(NodeBasePtr node);

    void clearScene();

    void removeNodesFromScene();

    DragAndDropStore m_dadStore;

    MindMapDataPtr m_mindMapData;

    UndoStack m_undoStack;

    Node * m_selectedNode = nullptr;

    Node * m_dragAndDropNode = nullptr;

    QPointF m_dragAndDropSourcePos;

    Mediator & m_mediator;

    std::vector<QGraphicsLineItem *> m_targetNodes;

    unsigned int m_activeColumn = 0;

    unsigned int m_activeRow = 0;
};

#endif // EDITORDATA_HPP
