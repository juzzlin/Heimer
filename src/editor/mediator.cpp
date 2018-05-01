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
        m_editorData->mindMapData()->setBackgroundColor(color);
        m_editorView->setBackgroundBrush(QBrush(color));
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
        }
    }

    for (auto && edge : m_editorData->mindMapData()->graph().getEdges())
    {
        auto node0 = dynamic_pointer_cast<Node>(getNodeByIndex(edge.first));
        auto node1 = dynamic_pointer_cast<Node>(getNodeByIndex(edge.second));

        if (!m_editorScene->hasEdge(*node0, *node1))
        {
            const auto graphicsEgde = node0->createAndAddGraphicsEdge(node1);
            node1->addGraphicsEdge(graphicsEgde);
            addItem(*graphicsEgde); // QGraphicsScene needs the raw pointer
            MCLogger().info() << "Created a new edge from " << node0->index() << " to " << node1->index();
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

NodeBasePtr Mediator::createAndAddNode(int sourceNodeIndex, QPointF pos)
{
    auto node1 = dynamic_pointer_cast<Node>(m_editorData->addNodeAt(pos));
    assert(node1);

    auto node0 = dynamic_pointer_cast<Node>(getNodeByIndex(sourceNodeIndex));
    assert(node0);

    // Currently floating nodes cannot be added. Always add edge from the parent node.
    m_editorData->addEdge(node0, node1);

    addExistingGraphToScene();

    MCLogger().info() << "Created a new node at (" << pos.x() << "," << pos.y() << ")";

    return node1;
}

DragAndDropStore & Mediator::dadStore()
{
    return m_editorData->dadStore();
}

void Mediator::enableUndo(bool enable)
{
    m_mainWindow.enableUndo(enable);
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
    MCLogger().info() << "Initializing a new mind map";

    assert(m_editorData);

    m_editorData->setMindMapData(std::make_shared<MindMapData>());

    delete m_editorScene;
    m_editorScene = new EditorScene;

    m_editorView->resetDummyDragItems();

    initializeView();

    m_editorData->addNodeAt(QPointF(0, 0));

    addExistingGraphToScene();

    zoomToFit();
}

void Mediator::initializeView()
{
    MCLogger().info() << "Initializing view";

    // Set scene to the view
    m_editorView->setScene(m_editorScene);

    assert(m_editorData);

    m_editorView->setBackgroundBrush(QBrush(m_editorData->backgroundColor()));

    m_mainWindow.setCentralWidget(m_editorView);
    m_mainWindow.setContentsMargins(0, 0, 0, 0);
}

bool Mediator::isRedoable() const
{
    return m_editorData->isRedoable();
}

bool Mediator::isSaved() const
{
    return m_editorData->isSaved();
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
}

void Mediator::showHelloText()
{
    m_editorView->showHelloText(true);
}

void Mediator::undo()
{
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
    m_editorView->zoomToFit(m_editorScene->getNodeBoundingRect());
}

Mediator::~Mediator()
{
    delete m_editorData;
}
