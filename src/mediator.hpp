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

#include "mind_map_data.hpp"
#include "node.hpp"

class MouseAction;
class EditorData;
class EditorScene;
class EditorView;
class Graph;
class MainWindow;
class NodeAction;
class NodeHandle;
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

    ~Mediator() override;

    void addEdge(Node & node1, Node & node2);

    void addItem(QGraphicsItem & item);

    void addNodeToSelectionGroup(Node & node);

    void adjustSceneRect();

    bool areDirectlyConnected(const Node & node1, const Node & node2) const;

    bool areSelectedNodesConnectable() const;

    bool areSelectedNodesDisconnectable() const;

    bool canBeSaved() const;

    void clearSelectedNode();

    void clearSelectionGroup();

    void connectEdgeToUndoMechanism(EdgePtr edge);

    void connectNodeToUndoMechanism(NodePtr node);

    void connectNodeToImageManager(NodePtr node);

    size_t copyStackSize() const;

    // Create a new node and add edge to the source (parent) node
    NodePtr createAndAddNode(int sourceNodeIndex, QPointF pos);

    // Create a new floating node
    NodePtr createAndAddNode(QPointF pos);

    MouseAction & mouseAction();

    void deleteEdge(Edge & edge);

    QString fileName() const;

    NodePtr getBestOverlapNode(const Node & source);

    NodePtr getNodeByIndex(int index);

    bool hasNodes() const;

    void initializeNewMindMap();

    void initiateNewNodeDrag(NodeHandle & nodeHandle);

    void initiateNodeDrag(Node & node);

    void initializeView();

    bool isInBetween(Node & node);

    bool isInSelectionGroup(Node & node);

    bool isLeafNode(Node & node);

    bool isUndoable() const;

    bool isRedoable() const;

    bool isModified() const;

    void moveSelectionGroup(Node & reference, QPointF location);

    size_t nodeCount() const;

    bool nodeHasImageAttached() const;

    NodePtr pasteNodeAt(Node & source, QPointF pos);

    MindMapDataPtr mindMapData() const;

    bool openMindMap(QString fileName);

    void performNodeAction(const NodeAction & action);

    void redo();

    void removeItem(QGraphicsItem & item);

    bool saveMindMapAs(QString fileName);

    bool saveMindMap();

    QSize sceneRectSize() const;

    Edge * selectedEdge() const;

    Node * selectedNode() const;

    size_t selectionGroupSize() const;

    void setEditorData(std::shared_ptr<EditorData> editorData);

    void setEditorView(EditorView & editorView);

    //! \returns number of nodes in the current rectangle.
    size_t setRectagleSelection(QRectF rect);

    void setSelectedEdge(Edge * edge);

    void toggleNodeInSelectionGroup(Node & node, bool updateNodeConnectionActions = true);

    void undo();

public slots:

    void enableUndo(bool enable);

    void enableRedo(bool enable);

    void exportToPng(QString filename, QSize size, bool transparentBackground);

    void exportToSvg(QString filename);

    void saveUndoPoint();

    void setBackgroundColor(QColor color);

    void setCornerRadius(int value);

    void setEdgeColor(QColor color);

    void setGridColor(QColor color);

    void setEdgeWidth(double value);

    void setSearchText(QString text);

    void setTextSize(int textSize);

    QSize zoomForExport();

    void zoomToFit();

private slots:

    void zoomIn();

    void zoomOut();

signals:

    void pngExportFinished(bool success);

    void svgExportFinished(bool success);

private:
    void addExistingGraphToScene();

    double calculateNodeOverlapScore(const Node & node1, const Node & node2) const;

    void connectGraphToUndoMechanism();

    void connectGraphToImageManager();

    void connectSelectedNodes();

    void disconnectSelectedNodes();

    void paste();

    void setupMindMapAfterUndoOrRedo();

    void updateNodeConnectionActions();

    std::shared_ptr<EditorData> m_editorData;

    std::unique_ptr<EditorScene> m_editorScene;

    EditorView * m_editorView = nullptr;

    MainWindow & m_mainWindow;
};

#endif // MEDIATOR_HPP
