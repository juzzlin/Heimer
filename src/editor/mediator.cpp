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

#include "mediator.hpp"

#include "draganddropstore.hpp"
#include "editordata.hpp"
#include "editorview.hpp"
#include "mainwindow.hpp"

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QSizePolicy>

#include <cassert>

Mediator::Mediator(MainWindow & mainWindow)
    : m_editorData(new EditorData(*this))
    , m_editorScene(new QGraphicsScene)
    , m_editorView(new EditorView(*this))
    , m_mainWindow(mainWindow)
{
    m_editorView->setParent(&mainWindow);
}

void Mediator::addItem(QGraphicsItem & item)
{
    m_editorScene->addItem(&item);
}

NodeBasePtr Mediator::createAndAddNode(int sourceNodeIndex, QPointF pos)
{
    auto targetNode = std::dynamic_pointer_cast<Node>(m_editorData->createAndAddNodeToGraph(pos));
    assert(targetNode);

    auto sourceNode = std::dynamic_pointer_cast<Node>(getNodeByIndex(sourceNodeIndex));
    assert(sourceNode);

    auto graphicsEgde = sourceNode->createAndAddEdge(targetNode);
    targetNode->addEdge(graphicsEgde);

    addItem(*graphicsEgde); // QGraphicsScene needs the raw pointer

    m_editorData->addExistingGraphToScene();

    return targetNode;
}

DragAndDropStore & Mediator::dadStore()
{
    return m_editorData->dadStore();
}

void Mediator::enableUndo(bool enable)
{
    m_mainWindow.enableUndo(enable);
}

int Mediator::fitScale()
{
    m_editorView->centerOn(m_editorView->sceneRect().center());
    return m_editorView->viewport()->height() * 100 / m_editorView->sceneRect().height();
}

NodeBasePtr Mediator::getNodeByIndex(int index)
{
    return m_editorData->getNodeByIndex(index);
}

void Mediator::initializeNewMindMap()
{
    assert(m_editorData);

    m_editorData->setMindMapData(MindMapDataPtr(new MindMapData));

    delete m_editorScene;
    m_editorScene = new QGraphicsScene;

    m_editorView->setScene(m_editorScene);
    m_editorView->updateSceneRect();

    m_editorData->createAndAddNodeToGraph(QPointF(0, 0));
    m_editorData->addExistingGraphToScene();

    fitScale();
}

void Mediator::initScene()
{
    // Set scene to the view
    m_editorView->setScene(m_editorScene);
    m_editorView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    m_editorView->setMouseTracking(true);
    m_editorView->setBackgroundBrush(QBrush(QColor(128, 200, 255, 255)));

    m_mainWindow.setCentralWidget(m_editorView);
    m_mainWindow.setContentsMargins(0, 0, 0, 0);
}

bool Mediator::isRedoable() const
{
    return m_editorData->isRedoable();
}

bool Mediator::isUndoable() const
{
    return m_editorData->isUndoable();
}

bool Mediator::openMindMap(QString fileName)
{
    assert(m_editorData);

    if (m_editorData->loadMindMapData(fileName))
    {
        delete m_editorScene;
        m_editorScene = new QGraphicsScene;

        m_editorView->setScene(m_editorScene);
        m_editorView->updateSceneRect();

        m_editorData->addExistingGraphToScene();

        fitScale();

        return true;
    }

    return false;
}

void Mediator::redo()
{
    m_editorData->redo();
}

void Mediator::removeItem(QGraphicsItem & item)
{
    m_editorScene->removeItem(&item);
}

void Mediator::saveUndoPoint()
{
    m_editorData->saveUndoPoint();
}

void Mediator::setSelectedNode(Node * node)
{
    m_editorData->setSelectedNode(node);
}

void Mediator::setupMindMapAfterUndoOrRedo()
{
    delete m_editorScene;
    m_editorScene = new QGraphicsScene;

    m_editorView->setScene(m_editorScene);
    m_editorView->updateSceneRect();

    m_editorData->addExistingGraphToScene();
}

void Mediator::undo()
{
    m_editorData->undo();
}

void Mediator::updateView()
{
    m_editorView->update();
}

Mediator::~Mediator()
{
    delete m_editorData;
}
