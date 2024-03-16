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

#include "editor_service.hpp"

#include "../application/recent_files_manager.hpp"
#include "../application/service_container.hpp"
#include "../application/settings_proxy.hpp"
#include "../common/constants.hpp"
#include "../common/test_mode.hpp"
#include "../domain/graph.hpp"
#include "../domain/mind_map_data.hpp"
#include "../domain/undo_stack.hpp"
#include "../infra/io/alz_file_io.hpp"
#include "../view/node_selection_group.hpp"
#include "../view/scene_items/edge.hpp"
#include "../view/scene_items/node.hpp"

#include "simple_logger.hpp"

using juzzlin::L;

#include <algorithm>
#include <cassert>
#include <memory>

using std::make_shared;

EditorService::EditorService()
  : m_alzFileIO(std::make_unique<IO::AlzFileIO>())
  , m_copyContext(std::make_unique<CopyContext>())
  , m_selectionGroup(std::make_unique<NodeSelectionGroup>())
  , m_undoStack(std::make_unique<UndoStack>())
{
    m_undoTimer.setSingleShot(true);
    m_undoTimer.setInterval(Constants::View::TOO_QUICK_ACTION_DELAY_MS);
}

void EditorService::addNodeToSelectionGroup(NodeR node, bool isImplicit)
{
    L().debug() << "Adding node " << node.index() << " to selection group..";

    m_selectionGroup->addNode(node, isImplicit);
}

void EditorService::requestAutosave(AutosaveContext context, bool async)
{
    const auto doRequestAutosave = [this](bool async) {
        if (SC::instance().settingsProxy()->autosave() && !m_fileName.isEmpty()) {
            L().debug() << "Autosaving to '" << m_fileName.toStdString() << "'";
            saveMindMapAs(m_fileName, async);
        }
    };

    switch (context) {
    case AutosaveContext::Modification:
        doRequestAutosave(async);
        break;
    case AutosaveContext::InitializeNewMindMap:
    case AutosaveContext::OpenMindMap:
    case AutosaveContext::QuitApplication:
        if (m_isTouched) {
            doRequestAutosave(async);
        }
        break;
    }
}

QColor EditorService::backgroundColor() const
{
    // Background color of "empty" editor is not the same as default color of new design
    return m_mindMapData ? m_mindMapData->backgroundColor() : Constants::MindMap::Defaults::BACKGROUND_COLOR;
}

MouseAction & EditorService::mouseAction()
{
    return m_mouseAction;
}

QString EditorService::fileName() const
{
    return m_fileName;
}

void EditorService::loadMindMapData(QString fileName)
{
    requestAutosave(AutosaveContext::OpenMindMap, false);
    clearSelectionGroup();

    m_selectedEdge = nullptr;

    if (!TestMode::enabled()) {
        setMindMapData(m_alzFileIO->fromFile(fileName));
    } else {
        TestMode::logDisabledCode("setMindMapData");
    }

    m_fileName = fileName;
    setIsModified(false);
    SC::instance().recentFilesManager()->addRecentFile(fileName);

    m_undoStack->clear();
}

bool EditorService::isModified() const
{
    return m_isModified;
}

bool EditorService::isUndoable() const
{
    return m_undoStack->isUndoable();
}

void EditorService::undo()
{
    if (m_undoStack->isUndoable()) {
        clearSelectionGroup();
        m_selectedEdge = nullptr;
        m_dragAndDropNode = nullptr;
        saveRedoPoint();
        m_mindMapData = m_undoStack->undo();
        setIsModified(true);
        sendUndoAndRedoSignals();
        requestAutosave(AutosaveContext::Modification, true);
    }
}

void EditorService::unselectText()
{
    for (auto && edge : mindMapData()->graph().getEdges()) {
        edge->unselectText();
    }

    for (auto && node : mindMapData()->graph().getNodes()) {
        node->unselectText();
    }
}

bool EditorService::isRedoable() const
{
    return m_undoStack->isRedoable();
}

void EditorService::redo()
{
    if (m_undoStack->isRedoable()) {
        clearSelectionGroup();
        m_selectedEdge = nullptr;
        m_dragAndDropNode = nullptr;
        saveUndoPoint(true);
        m_mindMapData = m_undoStack->redo();
        setIsModified(true);
        sendUndoAndRedoSignals();
        requestAutosave(AutosaveContext::Modification, true);
    }
}

void EditorService::removeImageRefsOfSelectedNodes()
{
    for (auto && node : m_selectionGroup->nodes()) {
        node->setImageRef(0);
    }
}

bool EditorService::saveMindMap(bool async)
{
    if (m_mindMapData && !m_fileName.isEmpty()) {
        return saveMindMapAs(m_fileName, async);
    }
    return false;
}

void EditorService::saveUndoPoint(bool dontClearRedoStack)
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
    m_undoStack->pushUndoPoint(*m_mindMapData);
    if (!dontClearRedoStack) {
        m_undoStack->clearRedoStack();
    }
    setIsModified(true);
    sendUndoAndRedoSignals();
    requestAutosave(AutosaveContext::Modification, true);

    m_isTouched = true;
}

void EditorService::saveRedoPoint()
{
    L().debug() << "Saving redo point..";

    assert(m_mindMapData);
    m_undoStack->pushRedoPoint(*m_mindMapData);
    setIsModified(true);
    sendUndoAndRedoSignals();
}

bool EditorService::saveMindMapAs(QString fileName, bool async)
{
    assert(m_mindMapData);

    if (m_alzFileIO->toFile(m_mindMapData, fileName, async)) {
        m_fileName = fileName;
        setIsModified(false);
        SC::instance().recentFilesManager()->addRecentFile(fileName);
        return true;
    }

    return false;
}

void EditorService::setColorForSelectedNodes(QColor color)
{
    for (auto && node : m_selectionGroup->nodes()) {
        node->setColor(color);
    }
}

void EditorService::setGridSize(int size, bool autoSnap)
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

void EditorService::setImageRefForSelectedNodes(size_t id)
{
    for (auto && node : m_selectionGroup->nodes()) {
        juzzlin::L().info() << "Setting image id=" << id << " to node " << node->index();
        node->setImageRef(id);
    }
}

void EditorService::setTextColorForSelectedNodes(QColor color)
{
    for (auto && node : m_selectionGroup->nodes()) {
        node->setTextColor(color);
    }
}

void EditorService::setMindMapData(MindMapDataS mindMapData)
{
    m_mindMapData = mindMapData;

    setIsModified(false);

    m_undoStack->clear();
}

void EditorService::selectEdgesByText(QString text)
{
    for (auto && edge : m_mindMapData->graph().getEdges()) {
        edge->highlightText(text);
    }
}

void EditorService::selectNodesByText(QString text)
{
    clearSelectionGroup();
    for (auto && node : m_mindMapData->graph().getNodes()) {
        if (!text.isEmpty() && node->containsText(text)) {
            addNodeToSelectionGroup(*node);
        }
        node->highlightText(text);
    }
}

void EditorService::toggleNodeInSelectionGroup(NodeR node)
{
    L().debug() << "Toggling node " << node.index() << " in selection group..";

    m_selectionGroup->toggleNode(node);
}

EdgeS EditorService::addEdge(EdgeS edge)
{
    assert(m_mindMapData);

    m_mindMapData->graph().addEdge(edge);
    return edge;
}

void EditorService::deleteEdge(EdgeR edge)
{
    assert(m_mindMapData);

    deleteEdge(edge.sourceNode().index(), edge.targetNode().index());
}

void EditorService::deleteEdge(int index0, int index1)
{
    assert(m_mindMapData);

    if (const auto deletedEdge = m_mindMapData->graph().deleteEdge(index0, index1)) {
        deletedEdge->hideWithAnimation();
    }
}

void EditorService::deleteNode(NodeR node)
{
    assert(m_mindMapData);

    const auto deletionInfo = m_mindMapData->graph().deleteNode(node.index());
    if (deletionInfo.first) {
        deletionInfo.first->hideWithAnimation();
    }
    for (auto && deletedEdge : deletionInfo.second) {
        if (deletedEdge) {
            deletedEdge->hideWithAnimation();
        }
    }
}

void EditorService::deleteSelectedNodes()
{
    assert(m_mindMapData);

    const auto selectedNodes = m_selectionGroup->nodes();
    if (selectedNodes.empty()) {
        if (SceneItems::Node::lastHoveredNode()) {
            deleteNode(*SceneItems::Node::lastHoveredNode());
        }
    } else {
        m_selectionGroup->clear();
        for (auto && node : selectedNodes) {
            deleteNode(*node);
        }
    }
}

NodeS EditorService::addNodeAt(QPointF pos)
{
    assert(m_mindMapData);

    const auto node = make_shared<SceneItems::Node>();
    node->setLocation(pos);
    m_mindMapData->graph().addNode(node);
    return node;
}

EditorService::NodePairVector EditorService::getConnectableNodes() const
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

bool EditorService::areSelectedNodesConnectable() const
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

EditorService::NodePairVector EditorService::getDisconnectableNodes() const
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

bool EditorService::areSelectedNodesDisconnectable() const
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

std::vector<EdgeS> EditorService::connectSelectedNodes()
{
    const auto connectableNodes = getConnectableNodes();
    std::vector<EdgeS> edges;
    std::transform(std::begin(connectableNodes), std::end(connectableNodes), std::back_inserter(edges),
                   [this](auto && nodePair) { return addEdge(std::make_shared<SceneItems::Edge>(nodePair.first, nodePair.second)); });
    return edges;
}

void EditorService::disconnectSelectedNodes()
{
    for (auto && nodePair : getDisconnectableNodes()) {
        deleteEdge(nodePair.first->index(), nodePair.second->index());
        deleteEdge(nodePair.second->index(), nodePair.first->index());
    }
}

CopyContext::CopiedData EditorService::copiedData() const
{
    return m_copyContext->copiedData();
}

NodeS EditorService::copyNodeAt(NodeCR source, QPointF pos)
{
    assert(m_mindMapData);

    const auto node = std::make_shared<SceneItems::Node>(source);
    node->setIndex(-1); // Results in new index to be assigned
    node->setLocation(pos);
    m_mindMapData->graph().addNode(node);
    return node;
}

void EditorService::copySelectedNodes()
{
    if (m_selectionGroup->size()) {
        clearCopyStack();
        m_copyContext->push(m_selectionGroup->nodes(), m_mindMapData->graph());
        L().debug() << m_selectionGroup->size() << " nodes copied. Reference point calculated at (" << m_copyContext->copiedData().copyReferencePoint.x() << ", " << m_copyContext->copiedData().copyReferencePoint.y() << ")";
    }
}

size_t EditorService::copyStackSize() const
{
    return m_copyContext->copyStackSize();
}

void EditorService::clearCopyStack()
{
    m_copyContext->clear();
}

void EditorService::clearSelectionGroup(bool onlyImplicitNodes)
{
    L().trace() << "Clearing selection group: onlyImplicitNodes == " << onlyImplicitNodes;

    m_selectionGroup->clear(onlyImplicitNodes);
}

NodeS EditorService::getNodeByIndex(int index)
{
    assert(m_mindMapData);

    return m_mindMapData->graph().getNode(index);
}

void EditorService::initializeNewMindMap()
{
    requestAutosave(AutosaveContext::InitializeNewMindMap, false);
    setMindMapData(std::make_shared<MindMapData>());
    m_fileName = "";
}

bool EditorService::isInSelectionGroup(NodeR node)
{
    return m_selectionGroup->hasNode(node);
}

MindMapDataS EditorService::mindMapData()
{
    return m_mindMapData;
}

void EditorService::mirror(bool vertically)
{
    m_mindMapData->mirror(vertically);
}

void EditorService::moveSelectionGroup(NodeR reference, QPointF location)
{
    m_selectionGroup->move(reference, location);
}

bool EditorService::nodeHasImageAttached() const
{
    for (auto && node : m_selectionGroup->nodes()) {
        if (node->imageRef()) {
            return true;
        }
    }
    return false;
}

void EditorService::setSelectedEdge(EdgeP edge)
{
    m_selectedEdge = edge;
}

EdgeP EditorService::selectedEdge() const
{
    return m_selectedEdge;
}

std::optional<NodeP> EditorService::selectedNode() const
{
    return m_selectionGroup->selectedNode();
}

std::vector<NodeP> EditorService::selectedNodes() const
{
    return m_selectionGroup->nodes();
}

size_t EditorService::selectionGroupSize() const
{
    return m_selectionGroup->size();
}

void EditorService::sendUndoAndRedoSignals()
{
    emit undoEnabled(m_undoStack->isUndoable());
    emit redoEnabled(m_undoStack->isRedoable());
}

void EditorService::setIsModified(bool isModified)
{
    if (isModified != m_isModified) {
        m_isModified = isModified;
        emit isModifiedChanged(isModified);
    }
}

EditorService::~EditorService()
{
    requestAutosave(AutosaveContext::QuitApplication, false);

    L().debug() << "EditorService deleted";
}
