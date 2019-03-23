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

#ifndef MEDIATOR_HPP
#define MEDIATOR_HPP

#include <QObject>
#include <QPointF>
#include <QString>

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
class Mediator : public QObject
{
    Q_OBJECT

public:

    explicit Mediator(MainWindow & mainWindow);

    ~Mediator();

    void addEdge(Node & node1, Node & node2);

    void addItem(QGraphicsItem & item);

    bool areDirectlyConnected(const Node & node1, const Node & node2) const;

    bool canBeSaved() const;

    void clearSelectedNode();

    void clearSelectionGroup();

    void connectEdgeToUndoMechanism(EdgePtr edge);

    void connectNodeToUndoMechanism(NodePtr node);

    // Create a new node and add edge to the source (parent) node
    NodeBasePtr createAndAddNode(int sourceNodeIndex, QPointF pos);

    // Create a new floating node
    NodeBasePtr createAndAddNode(QPointF pos);

    DragAndDropStore & dadStore();

    void deleteEdge(Edge & edge);

    void deleteNode(Node & node);

    QString fileName() const;

    NodePtr getBestOverlapNode(const Node & source);

    NodeBasePtr getNodeByIndex(int index);

    bool hasNodes() const;

    void initializeNewMindMap();

    void initializeView();

    bool isInBetween(Node & node);

    bool isInSelectionGroup(Node & node);

    bool isLeafNode(Node & node);

    bool isUndoable() const;

    bool isRedoable() const;

    bool isModified() const;

    void moveSelectionGroup(Node & reference, QPointF location);

    int nodeCount() const;

    bool openMindMap(QString fileName);

    void redo();

    void removeItem(QGraphicsItem & item);

    bool saveMindMapAs(QString fileName);

    bool saveMindMap();

    QSize sceneRectSize() const;

    Edge * selectedEdge() const;

    Node * selectedNode() const;

    size_t selectionGroupSize() const;

    void setEditorData(std::shared_ptr<EditorData> editorData);

    void setEditorScene(std::shared_ptr<EditorScene> editorScene);

    void setEditorView(EditorView & editorView);

    void setSelectedEdge(Edge * edge);

    void setSelectedNode(Node * node);

    void setupMindMapAfterUndoOrRedo();

    void toggleNodeInSelectionGroup(Node & node);

    void undo();

public slots:

    void clearScene();

    void enableUndo(bool enable);

    void exportToPNG(QString filename, QSize size, bool transparentBackground);

    void saveUndoPoint();

    void setBackgroundColor(QColor color);

    void setEdgeColor(QColor color);

    void setEdgeWidth(double value);

    void setTextSize(int textSize);

    QSize zoomForExport();

private slots:

    void zoomIn();

    void zoomOut();

    void zoomToFit();

signals:

    void exportFinished(bool success);

private:

    void addExistingGraphToScene();

    double calculateNodeOverlapScore(const Node & node1, const Node & node2) const;

    void connectGraphToUndoMechanism();

    std::shared_ptr<EditorData> m_editorData;

    std::shared_ptr<EditorScene> m_editorScene;

    EditorView * m_editorView = nullptr;

    MainWindow & m_mainWindow;
};

#endif // MEDIATOR_HPP
