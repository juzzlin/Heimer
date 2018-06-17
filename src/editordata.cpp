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

#include "editordata.hpp"

#include "config.hpp"
#include "mediator.hpp"
#include "node.hpp"
#include "serializer.hpp"
#include "reader.hpp"
#include "writer.hpp"

#include <cassert>
#include <memory>

using std::dynamic_pointer_cast;
using std::make_shared;

EditorData::EditorData(Mediator & mediator)
    : m_mediator(mediator)
{}

QColor EditorData::backgroundColor() const
{
    return m_mindMapData ? m_mindMapData->backgroundColor() : QColor(128, 200, 255, 255);
}

void EditorData::clearScene()
{
    removeNodesFromScene();
}

DragAndDropStore & EditorData::dadStore()
{
    return m_dadStore;
}

QString EditorData::fileName() const
{
    return m_fileName;
}

void EditorData::loadMindMapData(QString fileName)
{
    setMindMapData(Serializer::fromXml(Reader::readFromFile(fileName)));

    m_fileName = fileName;

    m_undoStack.clear();
}

bool EditorData::isModified() const
{
    return m_isModified;
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

        setIsModified(true);
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

        setIsModified(true);
    }
}

bool EditorData::saveMindMap()
{
    assert(m_mindMapData);
    assert(!m_fileName.isEmpty());

    return saveMindMapAs(m_fileName);
}

void EditorData::saveUndoPoint()
{
    assert(m_mindMapData);
    m_undoStack.pushUndoPoint(m_mindMapData);
    m_mediator.enableUndo(m_undoStack.isUndoable());

    setIsModified(true);
}

void EditorData::saveRedoPoint()
{
    assert(m_mindMapData);
    m_undoStack.pushRedoPoint(m_mindMapData);

    setIsModified(true);
}

bool EditorData::saveMindMapAs(QString fileName)
{
    assert(m_mindMapData);

    if (Writer::writeToFile(Serializer::toXml(*m_mindMapData), fileName))
    {
        m_fileName = fileName;
        setIsModified(false);
        return true;
    }

    return false;
}

void EditorData::setMindMapData(MindMapDataPtr mindMapData)
{
    m_mindMapData = mindMapData;
}

EdgePtr EditorData::addEdge(EdgePtr edge)
{
    m_mindMapData->graph().addEdge(edge);
    return edge;
}

NodePtr EditorData::addNodeAt(QPointF pos)
{
    auto node = make_shared<Node>();
    node->setLocation(pos);
    m_mindMapData->graph().addNode(node);
    return node;
}

NodeBasePtr EditorData::getNodeByIndex(int index)
{
    return m_mindMapData->graph().getNode(index);
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
        for (auto && node : m_mindMapData->graph().getNodes())
        {
            m_mediator.removeItem(*dynamic_pointer_cast<Node>(node)); // The scene wants a raw pointer
        }
    }
}

void EditorData::setIsModified(bool isModified)
{
    if (isModified != m_isModified)
    {
        m_isModified = isModified;
        emit isModifiedChanged(isModified);
    }
}
