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
#include <utility>
#include <vector>

#include <QObject>
#include <QPointF>
#include <QString>
#include <QTimer>

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

    ~EditorData() override;

    EdgePtr addEdge(EdgePtr edge);

    void addNodeToSelectionGroup(Node & node);

    //! \return true if at least one selected node pair can be connected.
    bool areSelectedNodesConnectable() const;

    //! \return true if at least one selected node pair can be disconnected.
    bool areSelectedNodesDisconnectable() const;

    void deleteEdge(Edge & edge);

    void deleteEdge(int index0, int index1);

    void deleteNode(Node & node);

    void deleteSelectedNodes();

    NodePtr addNodeAt(QPointF pos);

    void clearCopyStack();

    void clearImages();

    void clearSelectionGroup();

    //! Connects selected nodes in the order they were selected.
    //! \return the new edge objects.
    std::vector<std::shared_ptr<Edge>> connectSelectedNodes();

    //! Disconnects (deletes edges) directly connected nodes in the group if possible.
    void disconnectSelectedNodes();

    std::vector<std::shared_ptr<Node>> copiedNodes() const;

    NodePtr copyNodeAt(Node & source, QPointF pos);

    QPointF copyReferencePoint() const;

    void copySelectedNodes();

    size_t copyStackSize() const;

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

    bool nodeHasImageAttached() const;

    void redo();

    void removeImageRefsOfSelectedNodes();

    bool saveMindMap();

    bool saveMindMapAs(QString fileName);

    void saveUndoPoint(bool dontClearRedoStack = false);

    void saveRedoPoint();

    void setColorForSelectedNodes(QColor color);

    void setMindMapData(MindMapDataPtr newMindMapData);

    void setSelectedEdge(Edge * edge);

    void setSelectedNode(Node * node);

    void setImageRefForSelectedNodes(size_t id);

    void setTextColorForSelectedNodes(QColor color);

    Edge * selectedEdge() const;

    Node * selectedNode() const;

    size_t selectionGroupSize() const;

    void selectNodesByText(QString text);

    void toggleNodeInSelectionGroup(Node & node);

    void undo();

signals:

    void isModifiedChanged(bool isModified);

    void undoEnabled(bool enable);

    void redoEnabled(bool enable);

private:
    EditorData(const EditorData & e) = delete;
    EditorData & operator=(const EditorData & e) = delete;

    void autosave();

    using NodePairVector = std::vector<std::pair<Node *, Node *>>;
    NodePairVector getConnectableNodes() const;

    NodePairVector getDisconnectableNodes() const;

    void removeEdgeFromScene(Edge & edge);

    void removeNodeFromScene(Node & node);

    void sendUndoAndRedoSignals();

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

    QTimer m_undoTimer;

    struct CopyContext
    {
        std::vector<std::shared_ptr<Node>> copiedNodes;

        QPointF copyReferencePoint;
    };

    CopyContext m_copyContext;
};

#endif // EDITOR_DATA_HPP
