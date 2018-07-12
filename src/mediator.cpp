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

#include "mediator.hpp"

#include "draganddropstore.hpp"
#include "editordata.hpp"
#include "editorscene.hpp"
#include "editorview.hpp"
#include "mainwindow.hpp"

#include "mclogger.hh"

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QSizePolicy>

#include <cassert>

using std::dynamic_pointer_cast;

Mediator::Mediator(MainWindow & mainWindow)
    : m_editorData(new EditorData(*this))
    , m_editorScene(new EditorScene)
    , m_editorView(new EditorView(*this))
    , m_mainWindow(mainWindow)
{
    m_editorView->setParent(&mainWindow);

    connect(m_editorView, &EditorView::backgroundColorChanged, [=] (QColor color) {
        saveUndoPoint();
        m_editorData->mindMapData()->setBackgroundColor(color);
        m_editorView->setBackgroundBrush(QBrush(color));
    });

    connect(m_editorData, &EditorData::isModifiedChanged, [=] (bool isModified) {
        m_mainWindow.enableSave(isModified && canBeSaved());
    });

    connect(&m_mainWindow, &MainWindow::zoomToFitTriggered, this, &Mediator::zoomToFit);

    connect(&m_mainWindow, &MainWindow::zoomInTriggered, this, &Mediator::zoomIn);

    connect(&m_mainWindow, &MainWindow::zoomOutTriggered, this, &Mediator::zoomOut);

    initializeView();
}

void Mediator::addExistingGraphToScene()
{
    for (auto && node : m_editorData->mindMapData()->graph().getNodes())
    {
        if (dynamic_pointer_cast<QGraphicsItem>(node)->scene() != m_editorScene)
        {
            addItem(*dynamic_pointer_cast<Node>(node));
            MCLogger().debug() << "Added an existing node " << node->index() << " to scene";
        }
    }

    for (auto && edge : m_editorData->mindMapData()->graph().getEdges())
    {
        auto node0 = dynamic_pointer_cast<Node>(getNodeByIndex(edge->sourceNodeBase().index()));
        auto node1 = dynamic_pointer_cast<Node>(getNodeByIndex(edge->targetNodeBase().index()));

        if (!m_editorScene->hasEdge(*node0, *node1))
        {
            auto graphicsEdge = dynamic_pointer_cast<Edge>(edge);
            assert(graphicsEdge);
            addItem(*graphicsEdge);
            node0->addGraphicsEdge(*graphicsEdge);
            node1->addGraphicsEdge(*graphicsEdge);
            graphicsEdge->updateLine();
            MCLogger().debug() << "Added an existing edge " << node0->index() << " -> " << node1->index() << " to scene";
        }
    }
}

void Mediator::addItem(QGraphicsItem & item)
{
    m_editorScene->addItem(&item);
}

bool Mediator::canBeSaved() const
{
    return !m_editorData->fileName().isEmpty();
}

void Mediator::connectEdgeToUndoMechanism(EdgePtr edge)
{
    connect(edge.get(), &Edge::undoPointRequested, this, &Mediator::saveUndoPoint);
}

void Mediator::connectNodeToUndoMechanism(NodePtr node)
{
    connect(node.get(), &Node::undoPointRequested, this, &Mediator::saveUndoPoint);
}

void Mediator::connectGraphToUndoMechanism()
{
    for (auto && node : m_editorData->mindMapData()->graph().getNodes())
    {
        connectNodeToUndoMechanism(std::dynamic_pointer_cast<Node>(node));
    }

    for (auto && edge : m_editorData->mindMapData()->graph().getEdges())
    {
        connectEdgeToUndoMechanism(std::dynamic_pointer_cast<Edge>(edge));
    }
}

NodeBasePtr Mediator::createAndAddNode(int sourceNodeIndex, QPointF pos)
{
    auto node1 = m_editorData->addNodeAt(pos);
    assert(node1);
    connectNodeToUndoMechanism(node1);
    MCLogger().debug() << "Created a new node at (" << pos.x() << "," << pos.y() << ")";

    auto node0 = dynamic_pointer_cast<Node>(getNodeByIndex(sourceNodeIndex));
    assert(node0);

    // Currently floating nodes cannot be added. Always add edge from the parent node.
    connectEdgeToUndoMechanism(m_editorData->addEdge(std::make_shared<Edge>(*node0, *node1)));
    MCLogger().debug() << "Created a new edge " << node0->index() << " -> " << node1->index();

    addExistingGraphToScene();

    return node1;
}

DragAndDropStore & Mediator::dadStore()
{
    return m_editorData->dadStore();
}

// These cases are supported if node A is going to be deleted:
//
// 1) (C)-->(B)-->(A) => (C)-->(B)
//
// 2) (C)-->(A)-->(B) => (C)-->(B) (a new edge will be created that connects C to B)
void Mediator::deleteNode(Node & node)
{
    m_editorView->resetDummyDragItems();

    auto && graph = m_editorData->mindMapData()->graph();

    if (isInBetween(node))
    {
        auto && nodes = graph.getNodesConnectedToNode(graph.getNode(node.index()));
        assert(nodes.size() == 2);
        const auto node0 = nodes.at(0);
        const auto node1 = nodes.at(1);
        if (!graph.areDirectlyConnected(node0, node1))
        {
            connectEdgeToUndoMechanism(m_editorData->addEdge(
                std::make_shared<Edge>(*dynamic_pointer_cast<Node>(node0), *dynamic_pointer_cast<Node>(node1))));
            MCLogger().debug() << "Created a new edge " << node0->index() << " -> " << node1->index();

            addExistingGraphToScene();
        }
    }

    graph.deleteNode(node.index());
}

void Mediator::enableUndo(bool enable)
{
    m_mainWindow.enableUndo(enable);
}

QString Mediator::fileName() const
{
    return m_editorData->fileName();
}

NodeBasePtr Mediator::getNodeByIndex(int index)
{
    return m_editorData->getNodeByIndex(index);
}

bool Mediator::hasNodes() const
{
    return m_editorData->mindMapData() && m_editorData->mindMapData()->graph().numNodes();
}

void Mediator::initializeNewMindMap()
{
    MCLogger().debug() << "Initializing a new mind map";

    assert(m_editorData);

    m_editorData->setMindMapData(std::make_shared<MindMapData>());

    delete m_editorScene;
    m_editorScene = new EditorScene;

    initializeView();

    connectNodeToUndoMechanism(m_editorData->addNodeAt(QPointF(0, 0)));

    addExistingGraphToScene();

    QTimer::singleShot(0, this, &Mediator::zoomToFit);
}

void Mediator::initializeView()
{
    MCLogger().debug() << "Initializing view";

    // Set scene to the view
    m_editorView->setScene(m_editorScene);

    m_editorView->resetDummyDragItems();

    assert(m_editorData);

    m_editorView->setBackgroundBrush(QBrush(m_editorData->backgroundColor()));

    m_mainWindow.setCentralWidget(m_editorView);
    m_mainWindow.setContentsMargins(0, 0, 0, 0);
}

bool Mediator::isLeafNode(Node & node)
{
    auto && graph = m_editorData->mindMapData()->graph();
    return graph.getEdgesFromNode(graph.getNode(node.index())).size() + graph.getEdgesToNode(graph.getNode(node.index())).size() == 1;
}

bool Mediator::isInBetween(Node & node)
{
    auto && graph = m_editorData->mindMapData()->graph();
    return graph.getEdgesFromNode(graph.getNode(node.index())).size() + graph.getEdgesToNode(graph.getNode(node.index())).size() == 2;
}

bool Mediator::isRedoable() const
{
    return m_editorData->isRedoable();
}

bool Mediator::isModified() const
{
    return m_editorData->isModified();
}

bool Mediator::isUndoable() const
{
    return m_editorData->isUndoable();
}

bool Mediator::openMindMap(QString fileName)
{
    assert(m_editorData);

    try
    {
        delete m_editorScene;
        m_editorScene = new EditorScene;

        m_editorData->loadMindMapData(fileName);

        initializeView();

        addExistingGraphToScene();

        connectGraphToUndoMechanism();

        zoomToFit();
    }
    catch (const FileException & e)
    {
        m_mainWindow.showErrorDialog(e.message());
        return false;
    }

    return true;
}

void Mediator::redo()
{
    MCLogger().debug() << "Undo..";

    m_editorView->resetDummyDragItems();
    m_editorData->redo();
}

void Mediator::removeItem(QGraphicsItem & item)
{
    m_editorScene->removeItem(&item);
}

bool Mediator::saveMindMapAs(QString fileName)
{
    return m_editorData->saveMindMapAs(fileName);
}

bool Mediator::saveMindMap()
{
    return m_editorData->saveMindMap();
}

void Mediator::saveUndoPoint()
{
    MCLogger().debug() << "Saving undo point..";

    m_editorData->saveUndoPoint();
}

Node * Mediator::selectedNode() const
{
    return m_editorData->selectedNode();
}

void Mediator::setSelectedNode(Node * node)
{
    m_editorData->setSelectedNode(node);
}

void Mediator::setupMindMapAfterUndoOrRedo()
{
    delete m_editorScene;
    m_editorScene = new EditorScene;

    m_editorView->setScene(m_editorScene);

    addExistingGraphToScene();

    connectGraphToUndoMechanism();

    zoomToFit();
}

void Mediator::undo()
{
    MCLogger().debug() << "Undo..";

    m_editorView->resetDummyDragItems();
    m_editorData->undo();
}

static const int zoomSensitivity = 20;

void Mediator::zoomIn()
{
    m_editorView->zoom(zoomSensitivity);
}

void Mediator::zoomOut()
{
    m_editorView->zoom(-zoomSensitivity);
}

void Mediator::zoomToFit()
{
    m_editorView->zoomToFit(m_editorScene->getNodeBoundingRectWithHeuristics());
}

Mediator::~Mediator()
{
    delete m_editorData;
}
