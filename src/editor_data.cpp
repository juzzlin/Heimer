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

#include "editor_data.hpp"

#include "constants.hpp"
#include "node.hpp"
#include "reader.hpp"
#include "recent_files_manager.hpp"
#include "selection_group.hpp"
#include "serializer.hpp"
#include "writer.hpp"

#include <cassert>
#include <memory>

using std::make_shared;

EditorData::EditorData()
  : m_selectionGroup(std::make_unique<SelectionGroup>())
{
}

QColor EditorData::backgroundColor() const
{
    // Background color of "empty" editor is not the same as default color of new design
    return m_mindMapData ? m_mindMapData->backgroundColor() : Constants::MindMap::DEFAULT_BACKGROUND_COLOR;
}

void EditorData::clearScene()
{
    emit sceneCleared();
}

MouseAction & EditorData::mouseAction()
{
    return m_mouseAction;
}

QString EditorData::fileName() const
{
    return m_fileName;
}

void EditorData::loadMindMapData(QString fileName)
{
    clearImages();
    clearSelectionGroup();

    m_selectedEdge = nullptr;

#ifndef HEIMER_UNIT_TEST
    setMindMapData(Serializer::fromXml(Reader::readFromFile(fileName)));
#endif
    m_fileName = fileName;
    setIsModified(false);
    RecentFilesManager::instance().addRecentFile(fileName);

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
    if (m_undoStack.isUndoable()) {
        clearSelectionGroup();

        m_selectedEdge = nullptr;

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
    if (m_undoStack.isRedoable()) {
        clearSelectionGroup();

        m_selectedEdge = nullptr;

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
    m_undoStack.pushUndoPoint(*m_mindMapData);
    emit undoEnabled(m_undoStack.isUndoable());

    setIsModified(true);
}

void EditorData::saveRedoPoint()
{
    assert(m_mindMapData);
    m_undoStack.pushRedoPoint(*m_mindMapData);

    setIsModified(true);
}

bool EditorData::saveMindMapAs(QString fileName)
{
    assert(m_mindMapData);

    if (Writer::writeToFile(Serializer::toXml(*m_mindMapData), fileName)) {
        m_fileName = fileName;
        setIsModified(false);
        RecentFilesManager::instance().addRecentFile(fileName);
        return true;
    }

    return false;
}

void EditorData::setMindMapData(MindMapDataPtr mindMapData)
{
    m_mindMapData = mindMapData;

    m_fileName = "";
    setIsModified(false);

    m_undoStack.clear();
}

void EditorData::toggleNodeInSelectionGroup(Node & node)
{
    m_selectionGroup->toggleNode(node);
}

EdgePtr EditorData::addEdge(EdgePtr edge)
{
    assert(m_mindMapData);

    m_mindMapData->graph().addEdge(edge);
    return edge;
}

void EditorData::deleteEdge(Edge & edge)
{
    assert(m_mindMapData);

    m_mindMapData->graph().deleteEdge(edge.sourceNode().index(), edge.targetNode().index());
}

void EditorData::deleteNode(Node & node)
{
    assert(m_mindMapData);

    m_mindMapData->graph().deleteNode(node.index());
}

NodePtr EditorData::addNodeAt(QPointF pos)
{
    assert(m_mindMapData);

    const auto node = make_shared<Node>();
    node->setLocation(pos);
    m_mindMapData->graph().addNode(node);
    return node;
}

NodePtr EditorData::copyNodeAt(Node & source, QPointF pos)
{
    assert(m_mindMapData);

    const auto node = make_shared<Node>(source);
    node->setIndex(-1); // Results in new index to be assigned
    node->setLocation(pos);
    m_mindMapData->graph().addNode(node);
    return node;
}

void EditorData::clearImages()
{
    m_mindMapData->imageManager().clear();
}

void EditorData::clearSelectionGroup()
{
    m_selectionGroup->clear();
}

NodePtr EditorData::getNodeByIndex(int index)
{
    assert(m_mindMapData);

    return m_mindMapData->graph().getNode(index);
}

bool EditorData::isInSelectionGroup(Node & node)
{
    return m_selectionGroup->hasNode(node);
}

MindMapDataPtr EditorData::mindMapData()
{
    return m_mindMapData;
}

void EditorData::moveSelectionGroup(Node & reference, QPointF location)
{
    m_selectionGroup->move(reference, location);
}

void EditorData::setSelectedEdge(Edge * edge)
{
    m_selectedEdge = edge;
}

void EditorData::setSelectedNode(Node * node)
{
    m_selectionGroup->setSelectedNode(node);
}

Edge * EditorData::selectedEdge() const
{
    return m_selectedEdge;
}

Node * EditorData::selectedNode() const
{
    return m_selectionGroup->selectedNode();
}

size_t EditorData::selectionGroupSize() const
{
    return m_selectionGroup->size();
}

void EditorData::setIsModified(bool isModified)
{
    if (isModified != m_isModified) {
        m_isModified = isModified;
        emit isModifiedChanged(isModified);
    }
}

EditorData::~EditorData() = default;
