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
#include "editor_scene.hpp"

#include "alz_file_io.hpp"
#include "constants.hpp"
#include "node.hpp"
#include "recent_files_manager.hpp"
#include "selection_group.hpp"
#include "settings_proxy.hpp"
#include "test_mode.hpp"

#include "simple_logger.hpp"

using juzzlin::L;

#include <algorithm>
#include <cassert>
#include <memory>

using std::make_shared;

EditorData::EditorData()
  : m_copyContext(std::make_unique<CopyContext>())
  , m_selectionGroup(std::make_unique<SelectionGroup>())
{
    m_undoTimer.setSingleShot(true);
    m_undoTimer.setInterval(Constants::View::TOO_QUICK_ACTION_DELAY_MS);
}

void EditorData::addNodeToSelectionGroup(NodeR node)
{
    L().debug() << "Adding node " << node.index() << " to selection group..";

    m_selectionGroup->addSelectedNode(node);
}

void EditorData::requestAutosave()
{
    if (SettingsProxy::instance().autosave() && !m_fileName.isEmpty()) {
        L().debug() << "Autosaving to '" << m_fileName.toStdString() << "'";
        saveMindMapAs(m_fileName);
    }
}

QColor EditorData::backgroundColor() const
{
    // Background color of "empty" editor is not the same as default color of new design
    return m_mindMapData ? m_mindMapData->backgroundColor() : Constants::MindMap::Defaults::BACKGROUND_COLOR;
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
    requestAutosave();

    clearImages();
    clearSelectionGroup();

    m_selectedEdge = nullptr;

    if (!TestMode::enabled()) {
        setMindMapData(AlzFileIO().fromFile(fileName));
    } else {
        TestMode::logDisabledCode("setMindMapData");
    }

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
        m_mindMapData = m_undoStack.undo();
        setIsModified(true);
        sendUndoAndRedoSignals();
        requestAutosave();
    }
}

void EditorData::unselectText()
{
    for (auto && node : mindMapData()->graph().getNodes()) {
        node->unselectText();
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
        saveUndoPoint(true);
        m_mindMapData = m_undoStack.redo();
        setIsModified(true);
        sendUndoAndRedoSignals();
        requestAutosave();
    }
}

void EditorData::removeImageRefsOfSelectedNodes()
{
    for (auto && node : m_selectionGroup->nodes()) {
        node->setImageRef(0);
    }
}

bool EditorData::saveMindMap()
{
    if (m_mindMapData && !m_fileName.isEmpty()) {
        return saveMindMapAs(m_fileName);
    }
    return false;
}

void EditorData::saveUndoPoint(bool dontClearRedoStack)
{
    if (!TestMode::enabled()) {
        if (m_undoTimer.isActive()) {
            L().debug() << "Saving undo point skipped..";
            m_undoTimer.start();
            return;
        }
        L().debug() << "Saving undo point..";
        m_undoTimer.start();
    }

    assert(m_mindMapData);
    m_undoStack.pushUndoPoint(*m_mindMapData);
    if (!dontClearRedoStack) {
        m_undoStack.clearRedoStack();
    }
    setIsModified(true);
    sendUndoAndRedoSignals();
    requestAutosave();
}

void EditorData::saveRedoPoint()
{
    L().debug() << "Saving redo point..";

    assert(m_mindMapData);
    m_undoStack.pushRedoPoint(*m_mindMapData);
    setIsModified(true);
    sendUndoAndRedoSignals();
}

bool EditorData::saveMindMapAs(QString fileName)
{
    assert(m_mindMapData);

    if (AlzFileIO().toFile(*m_mindMapData, fileName)) {
        m_fileName = fileName;
        setIsModified(false);
        RecentFilesManager::instance().addRecentFile(fileName);
        return true;
    }

    return false;
}

void EditorData::setColorForSelectedNodes(QColor color)
{
    for (auto && node : m_selectionGroup->nodes()) {
        node->setColor(color);
    }
}

void EditorData::setGridSize(int size, bool autoSnap)
{
    m_grid.setSize(size);

    if (autoSnap) {
        if (!m_selectionGroup->isEmpty()) {
            saveUndoPoint();
            for (auto && node : m_selectionGroup->nodes()) {
                node->setLocation(m_grid.snapToGrid(node->location()));
            }
        } else if (m_mindMapData) {
            saveUndoPoint();
            m_mindMapData->applyGrid(m_grid);
        }
    }
}

void EditorData::setImageRefForSelectedNodes(size_t id)
{
    for (auto && node : m_selectionGroup->nodes()) {
        juzzlin::L().info() << "Setting image id=" << id << " to node " << node->index();
        node->setImageRef(id);
    }
}

void EditorData::setTextColorForSelectedNodes(QColor color)
{
    for (auto && node : m_selectionGroup->nodes()) {
        node->setTextColor(color);
    }
}

void EditorData::setMindMapData(MindMapDataPtr mindMapData)
{
    m_mindMapData = mindMapData;

    m_fileName = "";
    setIsModified(false);

    m_undoStack.clear();
}

void EditorData::selectNodesByText(QString text)
{
    clearSelectionGroup();
    for (auto && node : m_mindMapData->graph().getNodes()) {
        if (!text.isEmpty() && node->containsText(text)) {
            addNodeToSelectionGroup(*node);
        }
        node->highlightText(text);
    }
}

void EditorData::toggleNodeInSelectionGroup(NodeR node)
{
    L().debug() << "Toggling node " << node.index() << " in selection group..";

    m_selectionGroup->toggleNode(node);
}

EdgeS EditorData::addEdge(EdgeS edge)
{
    assert(m_mindMapData);

    m_mindMapData->graph().addEdge(edge);
    return edge;
}

void EditorData::deleteEdge(Edge & edge)
{
    assert(m_mindMapData);

    deleteEdge(edge.sourceNode().index(), edge.targetNode().index());
}

void EditorData::deleteEdge(int index0, int index1)
{
    assert(m_mindMapData);

    if (const auto deletedEdge = m_mindMapData->graph().deleteEdge(index0, index1)) {
        removeEdgeFromScene(*deletedEdge);
    }
}

void EditorData::deleteNode(NodeR node)
{
    assert(m_mindMapData);

    const auto deletionInfo = m_mindMapData->graph().deleteNode(node.index());
    if (deletionInfo.first) {
        removeNodeFromScene(*deletionInfo.first);
    }
    for (auto && deletedEdge : deletionInfo.second) {
        if (deletedEdge) {
            removeEdgeFromScene(*deletedEdge);
        }
    }
}

void EditorData::deleteSelectedNodes()
{
    assert(m_mindMapData);

    const auto selectedNodes = m_selectionGroup->nodes();
    if (selectedNodes.empty()) {
        if (Node::lastHoveredNode()) {
            deleteNode(*Node::lastHoveredNode());
        }
    } else {
        m_selectionGroup->clear();
        for (auto && node : selectedNodes) {
            deleteNode(*node);
        }
    }
}

NodeS EditorData::addNodeAt(QPointF pos)
{
    assert(m_mindMapData);

    const auto node = make_shared<Node>();
    node->setLocation(pos);
    m_mindMapData->graph().addNode(node);
    return node;
}

EditorData::NodePairVector EditorData::getConnectableNodes() const
{
    NodePairVector nodes;
    for (size_t i = 0; i + 1 < m_selectionGroup->nodes().size(); i++) {
        const auto c0 = m_selectionGroup->nodes().at(i);
        const auto c1 = m_selectionGroup->nodes().at(i + 1);
        if (!m_mindMapData->graph().areDirectlyConnected(c0->index(), c1->index())) {
            nodes.push_back({ c0, c1 });
        }
    }
    return nodes;
}

bool EditorData::areSelectedNodesConnectable() const
{
    for (size_t i = 0; i + 1 < m_selectionGroup->nodes().size(); i++) {
        const auto c0 = m_selectionGroup->nodes().at(i);
        const auto c1 = m_selectionGroup->nodes().at(i + 1);
        if (!m_mindMapData->graph().areDirectlyConnected(c0->index(), c1->index())) {
            return true;
        }
    }
    return false;
}

EditorData::NodePairVector EditorData::getDisconnectableNodes() const
{
    NodePairVector nodes;
    for (size_t i = 0; i < m_selectionGroup->nodes().size(); i++) {
        for (size_t j = i + 1; j < m_selectionGroup->nodes().size(); j++) {
            const auto c0 = m_selectionGroup->nodes().at(i);
            const auto c1 = m_selectionGroup->nodes().at(j);
            if (m_mindMapData->graph().areDirectlyConnected(c0->index(), c1->index())) {
                nodes.push_back({ c0, c1 });
            }
        }
    }
    return nodes;
}

bool EditorData::areSelectedNodesDisconnectable() const
{
    for (size_t i = 0; i < m_selectionGroup->nodes().size(); i++) {
        for (size_t j = i + 1; j < m_selectionGroup->nodes().size(); j++) {
            const auto c0 = m_selectionGroup->nodes().at(i);
            const auto c1 = m_selectionGroup->nodes().at(j);
            if (m_mindMapData->graph().areDirectlyConnected(c0->index(), c1->index())) {
                return true;
            }
        }
    }
    return false;
}

std::vector<EdgeS> EditorData::connectSelectedNodes()
{
    const auto connectableNodes = getConnectableNodes();
    std::vector<EdgeS> edges;
    std::transform(std::begin(connectableNodes), std::end(connectableNodes), std::back_inserter(edges),
                   [this](auto && nodePair) { return addEdge(std::make_shared<Edge>(nodePair.first, nodePair.second)); });
    return edges;
}

void EditorData::disconnectSelectedNodes()
{
    for (auto && nodePair : getDisconnectableNodes()) {
        deleteEdge(nodePair.first->index(), nodePair.second->index());
        deleteEdge(nodePair.second->index(), nodePair.first->index());
    }
}

CopyContext::CopiedData EditorData::copiedData() const
{
    return m_copyContext->copiedData();
}

NodeS EditorData::copyNodeAt(NodeCR source, QPointF pos)
{
    assert(m_mindMapData);

    const auto node = std::make_shared<Node>(source);
    node->setIndex(-1); // Results in new index to be assigned
    node->setLocation(pos);
    m_mindMapData->graph().addNode(node);
    return node;
}

void EditorData::copySelectedNodes()
{
    if (m_selectionGroup->size()) {
        clearCopyStack();
        m_copyContext->push(m_selectionGroup->nodes(), m_mindMapData->graph());
        L().debug() << m_selectionGroup->size() << " nodes copied. Reference point calculated at (" << m_copyContext->copiedData().copyReferencePoint.x() << ", " << m_copyContext->copiedData().copyReferencePoint.y() << ")";
    }
}

size_t EditorData::copyStackSize() const
{
    return m_copyContext->copyStackSize();
}

void EditorData::clearCopyStack()
{
    m_copyContext->clear();
}

void EditorData::clearImages()
{
    m_mindMapData->imageManager().clear();
}

void EditorData::clearSelectionGroup()
{
    L().trace() << "Clearing selection group..";

    m_selectionGroup->clear();
}

NodeS EditorData::getNodeByIndex(int index)
{
    assert(m_mindMapData);

    return m_mindMapData->graph().getNode(index);
}

void EditorData::initializeNewMindMap()
{
    requestAutosave();

    clearImages();
    setMindMapData(std::make_shared<MindMapData>());
}

bool EditorData::isInSelectionGroup(NodeR node)
{
    return m_selectionGroup->hasNode(node);
}

MindMapDataPtr EditorData::mindMapData()
{
    return m_mindMapData;
}

void EditorData::moveSelectionGroup(NodeR reference, QPointF location)
{
    m_selectionGroup->move(reference, location);
}

bool EditorData::nodeHasImageAttached() const
{
    for (auto && node : m_selectionGroup->nodes()) {
        if (node->imageRef()) {
            return true;
        }
    }
    return false;
}

void EditorData::setSelectedEdge(EdgeP edge)
{
    m_selectedEdge = edge;
}

EdgeP EditorData::selectedEdge() const
{
    return m_selectedEdge;
}

NodeP EditorData::selectedNode() const
{
    return m_selectionGroup->selectedNode();
}

size_t EditorData::selectionGroupSize() const
{
    return m_selectionGroup->size();
}

void EditorData::removeEdgeFromScene(EdgeR edge)
{
    edge.restoreLabelParent();
    edge.hide();
    if (const auto scene = edge.scene()) {
        scene->removeItem(&edge);
    }
}

void EditorData::removeNodeFromScene(NodeR node)
{
    node.hide();
    node.removeHandles();
    if (const auto scene = node.scene()) {
        scene->removeItem(&node);
    }
}

void EditorData::sendUndoAndRedoSignals()
{
    emit undoEnabled(m_undoStack.isUndoable());
    emit redoEnabled(m_undoStack.isRedoable());
}

void EditorData::setIsModified(bool isModified)
{
    if (isModified != m_isModified) {
        m_isModified = isModified;
        emit isModifiedChanged(isModified);
    }
}

EditorData::~EditorData()
{
    requestAutosave();

    L().debug() << "EditorData deleted";
}
