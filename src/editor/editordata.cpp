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

#include "editordata.hpp"
#include "mediator.hpp"
#include "node.hpp"
#include "serializer.hpp"
#include "writer.hpp"

#include <cassert>
#include <memory>

using std::dynamic_pointer_cast;

EditorData::EditorData(Mediator & mediator)
    : m_mediator(mediator)
{}

void EditorData::clearScene()
{
    removeNodesFromScene();
}

DragAndDropStore & EditorData::dadStore()
{
    return m_dadStore;
}

bool EditorData::loadMindMapData(QString fileName)
{
    m_undoStack.clear();

    clearScene();

    //m_mindMapData = m_mindMapIO.open(fileName);
    //return static_cast<bool>(m_mindMapData);
    return false;
}

bool EditorData::isSaved() const
{
    return m_isSaved;
}

bool EditorData::isUndoable() const
{
    return m_undoStack.isUndoable();
}

void EditorData::undo()
{
    if (m_undoStack.isUndoable())
    {
        m_selectedNode = nullptr;

        m_dragAndDropNode = nullptr;

        saveRedoPoint();

        clearScene();

        m_mindMapData = m_undoStack.undo();
    }
}

bool EditorData::isRedoable() const
{
    return m_undoStack.isRedoable();
}

void EditorData::redo()
{
    if (m_undoStack.isRedoable())
    {
        m_selectedNode = nullptr;

        m_dragAndDropNode = nullptr;

        saveUndoPoint();

        clearScene();

        m_mindMapData = m_undoStack.redo();
    }
}

bool EditorData::saveMindMap()
{
    assert(m_mindMapData);

    return saveMindMapAs(m_fileName);
}

void EditorData::saveUndoPoint()
{
    assert(m_mindMapData);
    m_undoStack.pushUndoPoint(m_mindMapData);
    m_mediator.enableUndo(m_undoStack.isUndoable());
}

void EditorData::saveRedoPoint()
{
    assert(m_mindMapData);
    m_undoStack.pushRedoPoint(m_mindMapData);
}

bool EditorData::saveMindMapAs(QString fileName)
{
    assert(m_mindMapData);

    Serializer serializer(*m_mindMapData);

    if (Writer::writeToFile(serializer.toXml(), fileName))
    {
        m_fileName = fileName;
        m_isSaved = true;
        return true;
    }

    return false;
}

void EditorData::setMindMapData(MindMapDataPtr mindMapData)
{
    clearScene();

    m_mindMapData = mindMapData;
}

void EditorData::addExistingGraphToScene()
{
    for (auto && node : m_mindMapData->graph().getAll())
    {
        addNodeToScene(node);
    }
}

NodeBasePtr EditorData::createAndAddNodeToGraph(QPointF pos)
{
    NodeBasePtr node(new Node);
    node->setLocation(pos);
    m_mindMapData->graph().addNode(node);
    return node;
}

NodeBasePtr EditorData::getNodeByIndex(int index)
{
    return m_mindMapData->graph().get(index);
}

void EditorData::addNodeToScene(NodeBasePtr node)
{
    auto ptr = std::dynamic_pointer_cast<Node>(node);
    if (!ptr->scene())
    {
        m_mediator.addItem(*ptr); // The scene wants a raw pointer
    }
}

void EditorData::removeGraphFromScene()
{
    for (auto && nodeIter : m_mindMapData->graph())
    {
        auto node = dynamic_pointer_cast<Node>(nodeIter.second);
        assert(node);

        m_mediator.removeItem(*node); // The scene wants a raw pointer
    }

    m_mediator.updateView();
}

MindMapDataPtr EditorData::mindMapData()
{
    return m_mindMapData;
}

void EditorData::setSelectedNode(Node * node)
{
    m_selectedNode = node;
}

Node * EditorData::selectedNode() const
{
    return m_selectedNode;
}

void EditorData::removeNodesFromScene()
{
    if (m_mindMapData)
    {
        for (auto nodeIter : m_mindMapData->graph())
        {
            auto node = dynamic_pointer_cast<Node>(nodeIter.second);
            assert(node);

            m_mediator.removeItem(*node); // The scene wants a raw pointer
        }
    }
}

void EditorData::clearGraph()
{
    assert(m_mindMapData);

    removeGraphFromScene();
    m_mindMapData->graph().clear();
}
