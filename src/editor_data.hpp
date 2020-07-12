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

#ifndef EDITOR_DATA_HPP
#define EDITOR_DATA_HPP

#include <memory>
#include <set>
#include <vector>

#include <QObject>
#include <QPointF>
#include <QString>

#include "edge.hpp"
#include "file_exception.hpp"
#include "mind_map_data.hpp"
#include "mouse_action.hpp"
#include "node.hpp"
#include "undo_stack.hpp"

class Node;
class NodeBase;
class MindMapTile;
class SelectionGroup;
class QGraphicsLineItem;

class EditorData : public QObject
{
    Q_OBJECT

public:
    EditorData();

    ~EditorData();

    EdgePtr addEdge(EdgePtr edge);

    void deleteEdge(Edge & edge);

    void deleteNode(Node & node);

    NodePtr addNodeAt(QPointF pos);

    void clearImages();

    void clearSelectionGroup();

    NodePtr copyNodeAt(Node & source, QPointF pos);

    QColor backgroundColor() const;

    MouseAction & mouseAction();

    QString fileName() const;

    NodePtr getNodeByIndex(int index);

    bool isInSelectionGroup(Node & node);

    bool isUndoable() const;

    bool isRedoable() const;

    bool isModified() const;

    void loadMindMapData(QString fileName);

    MindMapDataPtr mindMapData();

    void moveSelectionGroup(Node & reference, QPointF location);

    void redo();

    bool saveMindMap();

    bool saveMindMapAs(QString fileName);

    void saveUndoPoint();

    void saveRedoPoint();

    void setMindMapData(MindMapDataPtr newMindMapData);

    void setSelectedEdge(Edge * edge);

    void setSelectedNode(Node * node);

    Edge * selectedEdge() const;

    Node * selectedNode() const;

    size_t selectionGroupSize() const;

    void toggleNodeInSelectionGroup(Node & node);

    void undo();

signals:

    void isModifiedChanged(bool isModified);

    void undoEnabled(bool enable);

private:
    EditorData(const EditorData & e) = delete;
    EditorData & operator=(const EditorData & e) = delete;

    void removeNodesFromScene();

    void setIsModified(bool isModified);

    MouseAction m_mouseAction;

    MindMapDataPtr m_mindMapData;

    std::unique_ptr<SelectionGroup> m_selectionGroup;

    UndoStack m_undoStack;

    Edge * m_selectedEdge = nullptr;

    Node * m_dragAndDropNode = nullptr;

    QPointF m_dragAndDropSourcePos;

    std::vector<QGraphicsLineItem *> m_targetNodes;

    unsigned int m_activeColumn = 0;

    unsigned int m_activeRow = 0;

    bool m_isModified = false;

    QString m_fileName;
};

#endif // EDITOR_DATA_HPP
