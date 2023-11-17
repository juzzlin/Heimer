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

#include "application_service.hpp"

#include "editor_scene.hpp"
#include "editor_service.hpp"
#include "editor_view.hpp"
#include "export_params.hpp"
#include "image_manager.hpp"
#include "magic_zoom.hpp"
#include "main_window.hpp"
#include "mouse_action.hpp"
#include "node_action.hpp"
#include "single_instance_container.hpp"

#include "core/progress_manager.hpp"
#include "core/settings_proxy.hpp"
#include "core/shadow_effect_params.hpp"

#include "core/graph.hpp"
#include "scene_items/node_handle.hpp"

#include "simple_logger.hpp"

#include <QApplication>
#include <QClipboard>
#include <QFileInfo>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QImage>
#include <QMimeData>
#include <QSizePolicy>

#include <cassert>
#include <chrono>
#include <cmath>

using juzzlin::L;

ApplicationService::ApplicationService(MainWindow & mainWindow)
  : m_mainWindow(mainWindow)
{
    connect(&m_mainWindow, &MainWindow::zoomToFitTriggered, this, &ApplicationService::zoomToFit);
    connect(&m_mainWindow, &MainWindow::zoomInTriggered, this, &ApplicationService::zoomIn);
    connect(&m_mainWindow, &MainWindow::zoomOutTriggered, this, &ApplicationService::zoomOut);
}

void ApplicationService::addExistingGraphToScene(bool zoomToFitAfterNodesLoaded)
{
    auto && editorService = SingleInstanceContainer::instance().editorService();

    for (auto && node : editorService->mindMapData()->graph().getNodes()) {
        if (node->scene() != m_editorScene.get()) {
            addItem(*node);
            node->setCornerRadius(editorService->mindMapData()->cornerRadius());
            node->setTextSize(editorService->mindMapData()->textSize());
            node->changeFont(editorService->mindMapData()->font());
            L().trace() << "Added existing node id=" << node->index() << " to scene";
        }
    }

    if (zoomToFitAfterNodesLoaded) {
        zoomToFit();
    }

    updateProgress();

    size_t progressCounter = 0;
    for (auto && edge : editorService->mindMapData()->graph().getEdges()) {
        const auto node0 = getNodeByIndex(edge->sourceNode().index());
        const auto node1 = getNodeByIndex(edge->targetNode().index());
        if (!m_editorScene->hasEdge(*node0, *node1)) {
            addItem(*edge, false);
            edge->setArrowSize(editorService->mindMapData()->arrowSize());
            edge->setColor(editorService->mindMapData()->edgeColor());
            edge->setEdgeWidth(editorService->mindMapData()->edgeWidth());
            edge->setTextSize(editorService->mindMapData()->textSize());
            edge->changeFont(editorService->mindMapData()->font());
            node0->addGraphicsEdge(*edge);
            node1->addGraphicsEdge(*edge);
            edge->updateLine();
            L().trace() << "Added existing edge (" << node0->index() << ", " << node1->index() << ") to scene";
        }
        if (++progressCounter % 100 == 0) {
            updateProgress();
        }
    }

    updateProgress();

    // This is to prevent nasty updated loops like in https://github.com/juzzlin/Heimer/issues/96
    m_mainWindow.enableWidgetSignals(false);

    updateProgress();

    m_mainWindow.setArrowSize(editorService->mindMapData()->arrowSize());
    m_mainWindow.setCornerRadius(editorService->mindMapData()->cornerRadius());
    m_mainWindow.setEdgeWidth(editorService->mindMapData()->edgeWidth());
    m_mainWindow.setTextSize(editorService->mindMapData()->textSize());
    m_mainWindow.changeFont(editorService->mindMapData()->font());

    updateProgress();

    m_editorView->setArrowSize(editorService->mindMapData()->arrowSize());
    m_editorView->setCornerRadius(editorService->mindMapData()->cornerRadius());
    m_editorView->setEdgeColor(editorService->mindMapData()->edgeColor());
    m_editorView->setEdgeWidth(editorService->mindMapData()->edgeWidth());

    updateProgress();

    m_mainWindow.enableWidgetSignals(true);
}

void ApplicationService::addEdge(NodeR node1, NodeR node2)
{
    // Add edge from node1 to node2
    connectEdgeToUndoMechanism(SingleInstanceContainer::instance().editorService()->addEdge(std::make_shared<SceneItems::Edge>(&node1, &node2)));
    L().debug() << "Created a new edge " << node1.index() << " -> " << node2.index();

    addExistingGraphToScene();
}

void ApplicationService::addItem(QGraphicsItem & item, bool adjustSceneRect)
{
    m_editorScene->addItem(&item);
    if (adjustSceneRect) {
        this->adjustSceneRect();
    }
}

void ApplicationService::addNodeToSelectionGroup(NodeR node, bool isImplicit)
{
    SingleInstanceContainer::instance().editorService()->addNodeToSelectionGroup(node, isImplicit);
    updateNodeConnectionActions();
}

void ApplicationService::adjustSceneRect()
{
    m_editorScene->adjustSceneRect();
}

void ApplicationService::clearSelectionGroup(bool onlyImplicitNodes)
{
    SingleInstanceContainer::instance().editorService()->clearSelectionGroup(onlyImplicitNodes);
    updateNodeConnectionActions();
}

bool ApplicationService::canBeSaved() const
{
    return !SingleInstanceContainer::instance().editorService()->fileName().isEmpty();
}

void ApplicationService::connectEdgeToUndoMechanism(EdgeS edge)
{
    connect(edge.get(), &SceneItems::Edge::undoPointRequested, this, &ApplicationService::saveUndoPoint, Qt::UniqueConnection);
}

void ApplicationService::connectNodeToUndoMechanism(NodeS node)
{
    connect(node.get(), &SceneItems::Node::undoPointRequested, this, &ApplicationService::saveUndoPoint, Qt::UniqueConnection);
}

void ApplicationService::connectNodeToImageManager(NodeS node)
{
    connect(node.get(), &SceneItems::Node::imageRequested, &SingleInstanceContainer::instance().editorService()->mindMapData()->imageManager(), &ImageManager::handleImageRequest, Qt::UniqueConnection);
    node->setImageRef(node->imageRef()); // This effectively results in a fetch from ImageManager
}

size_t ApplicationService::copyStackSize() const
{
    return SingleInstanceContainer::instance().editorService()->copyStackSize();
}

void ApplicationService::connectGraphToUndoMechanism()
{
    for (auto && node : SingleInstanceContainer::instance().editorService()->mindMapData()->graph().getNodes()) {
        connectNodeToUndoMechanism(node);
    }

    for (auto && edge : SingleInstanceContainer::instance().editorService()->mindMapData()->graph().getEdges()) {
        connectEdgeToUndoMechanism(edge);
    }
}

void ApplicationService::connectGraphToImageManager()
{
    for (auto && node : SingleInstanceContainer::instance().editorService()->mindMapData()->graph().getNodes()) {
        connectNodeToImageManager(node);
    }
}

void ApplicationService::connectSelectedNodes()
{
    auto && editorService = SingleInstanceContainer::instance().editorService();

    L().debug() << "Connecting selected nodes: " << editorService->selectionGroupSize();
    if (areSelectedNodesConnectable()) {
        saveUndoPoint();
        for (auto && edge : editorService->connectSelectedNodes()) {
            connectEdgeToUndoMechanism(edge);
        }
        addExistingGraphToScene();
        updateNodeConnectionActions();
    }
}

void ApplicationService::disconnectSelectedNodes()
{
    auto && editorService = SingleInstanceContainer::instance().editorService();

    L().debug() << "Disconnecting selected nodes: " << editorService->selectionGroupSize();
    if (areSelectedNodesDisconnectable()) {
        saveUndoPoint();
        editorService->disconnectSelectedNodes();
        updateNodeConnectionActions();
    }
}

void ApplicationService::changeFont(const QFont & font)
{
    saveUndoPoint();
    SingleInstanceContainer::instance().editorService()->mindMapData()->changeFont(font);
}

NodeS ApplicationService::createAndAddNode(int sourceNodeIndex, QPointF pos)
{
    auto && editorService = SingleInstanceContainer::instance().editorService();

    const auto node0 = getNodeByIndex(sourceNodeIndex);
    const auto node1 = m_mainWindow.copyOnDragEnabled() ? editorService->copyNodeAt(*node0, pos) : editorService->addNodeAt(pos);
    connectNodeToUndoMechanism(node1);
    connectNodeToImageManager(node1);
    L().debug() << "Created a new node at (" << pos.x() << "," << pos.y() << ")";

    // Add edge from the parent node.
    connectEdgeToUndoMechanism(editorService->addEdge(std::make_shared<SceneItems::Edge>(node0.get(), node1.get())));
    L().debug() << "Created a new edge " << node0->index() << " -> " << node1->index();

    addExistingGraphToScene();

    node1->setText("");
    node1->setTextInputActive(true);

    return node1;
}

NodeS ApplicationService::createAndAddNode(QPointF pos)
{
    const auto node1 = SingleInstanceContainer::instance().editorService()->addNodeAt(m_editorView->grid().snapToGrid(pos));
    connectNodeToUndoMechanism(node1);
    connectNodeToImageManager(node1);
    L().debug() << "Created a new node at (" << pos.x() << "," << pos.y() << ")";

    addExistingGraphToScene();

    QTimer::singleShot(0, this, [node1] { // Needed due to the context menu
        node1->setTextInputActive(true);
    });

    return node1;
}

NodeS ApplicationService::pasteNodeAt(NodeR source, QPointF pos)
{
    const auto copiedNode = SingleInstanceContainer::instance().editorService()->copyNodeAt(source, pos);
    connectNodeToUndoMechanism(copiedNode);
    connectNodeToImageManager(copiedNode);
    L().debug() << "Pasted node at (" << pos.x() << "," << pos.y() << ")";
    return copiedNode;
}

MouseAction & ApplicationService::mouseAction()
{
    return SingleInstanceContainer::instance().editorService()->mouseAction();
}

void ApplicationService::deleteEdge(EdgeR edge)
{
    SingleInstanceContainer::instance().editorService()->deleteEdge(edge);
}

void ApplicationService::enableAutosave(bool enable)
{
    if (enable) {
        SingleInstanceContainer::instance().editorService()->requestAutosave(EditorService::AutosaveContext::Modification, true);
    }
}

void ApplicationService::enableUndo(bool enable)
{
    m_mainWindow.enableUndo(enable);
}

void ApplicationService::enableRedo(bool enable)
{
    m_mainWindow.enableRedo(enable);
}

void ApplicationService::exportToPng(const ExportParams & exportParams)
{
    m_editorView->saveZoom();
    zoomForExport();

    L().info() << "Exporting a PNG image of size (" << exportParams.imageSize.width() << "x" << exportParams.imageSize.height() << ") to " << exportParams.fileName.toStdString();
    const auto image = m_editorScene->toImage(exportParams.imageSize, SingleInstanceContainer::instance().editorService()->backgroundColor(), exportParams.transparentBackground);
    m_editorView->restoreZoom();

    emit pngExportFinished(image.save(exportParams.fileName));
}

void ApplicationService::exportToSvg(const ExportParams & exportParams)
{
    m_editorView->saveZoom();
    zoomForExport();

    L().info() << "Exporting an SVG image to " << exportParams.fileName.toStdString();
    m_editorScene->toSvg(exportParams.fileName, QFileInfo { exportParams.fileName }.fileName());
    m_editorView->restoreZoom();

    emit svgExportFinished(true);
}

QString ApplicationService::fileName() const
{
    return SingleInstanceContainer::instance().editorService()->fileName();
}

NodeS ApplicationService::getNodeByIndex(int index)
{
    return SingleInstanceContainer::instance().editorService()->getNodeByIndex(index);
}

bool ApplicationService::hasNodes() const
{
    auto && editorService = SingleInstanceContainer::instance().editorService();
    return editorService->mindMapData() && editorService->mindMapData()->graph().nodeCount();
}

void ApplicationService::initializeNewMindMap()
{
    L().debug() << "Initializing a new mind map";

    auto && editorService = SingleInstanceContainer::instance().editorService();

    m_editorScene = std::make_unique<EditorScene>();
    editorService->initializeNewMindMap();

    initializeView();

    const auto node = editorService->addNodeAt(QPointF(0, 0));
    connectNodeToUndoMechanism(node);
    connectNodeToImageManager(node);

    addExistingGraphToScene();

    QTimer::singleShot(0, this, &ApplicationService::zoomToFit);

    m_mainWindow.initializeNewMindMap();
}

void ApplicationService::initiateNewNodeDrag(SceneItems::NodeHandle & nodeHandle)
{
    L().debug() << "Initiating new node drag";

    clearSelectionGroup();
    saveUndoPoint();
    mouseAction().setSourceNode(&nodeHandle.parentNode(), MouseAction::Action::CreateOrConnectNode);
    mouseAction().setSourcePosOnNode(nodeHandle.pos() - nodeHandle.parentNode().pos());
    nodeHandle.parentNode().setHandlesVisible(false);

    // Change cursor to the closed hand cursor.
    QApplication::setOverrideCursor(QCursor(Qt::ClosedHandCursor));
}

void ApplicationService::initiateNodeDrag(NodeR node)
{
    L().debug() << "Initiating node drag";

    saveUndoPoint();
    node.setZValue(node.zValue() + 1);
    mouseAction().setSourceNode(&node, MouseAction::Action::MoveNode);
    mouseAction().setSourcePos(mouseAction().mappedPos());
    mouseAction().setSourcePosOnNode(mouseAction().mappedPos() - node.pos());

    // Change cursor to the closed hand cursor.
    QApplication::setOverrideCursor(QCursor(Qt::ClosedHandCursor));
}

void ApplicationService::initializeView()
{
    L().debug() << "Initializing view";

    // Set scene to the view
    m_editorView->setScene(m_editorScene.get());
    m_editorView->resetDummyDragItems();

    m_editorView->setBackgroundBrush(QBrush(SingleInstanceContainer::instance().editorService()->backgroundColor()));

    m_mainWindow.setCentralWidget(m_editorView);
    m_mainWindow.setContentsMargins(0, 0, 0, 0);
}

bool ApplicationService::areDirectlyConnected(NodeCR node1, NodeCR node2) const
{
    auto && graph = SingleInstanceContainer::instance().editorService()->mindMapData()->graph();
    const auto node1Ptr = graph.getNode(node1.index());
    const auto node2Ptr = graph.getNode(node2.index());
    return graph.areDirectlyConnected(node1Ptr, node2Ptr);
}

bool ApplicationService::areSelectedNodesConnectable() const
{
    return SingleInstanceContainer::instance().editorService()->areSelectedNodesConnectable();
}

bool ApplicationService::areSelectedNodesDisconnectable() const
{
    return SingleInstanceContainer::instance().editorService()->areSelectedNodesDisconnectable();
}

bool ApplicationService::isLeafNode(NodeR node)
{
    auto && graph = SingleInstanceContainer::instance().editorService()->mindMapData()->graph();
    return graph.getEdgesFromNode(graph.getNode(node.index())).size() + graph.getEdgesToNode(graph.getNode(node.index())).size() <= 1;
}

bool ApplicationService::isInBetween(NodeR node)
{
    auto && graph = SingleInstanceContainer::instance().editorService()->mindMapData()->graph();
    return graph.getEdgesFromNode(graph.getNode(node.index())).size() + graph.getEdgesToNode(graph.getNode(node.index())).size() == 2;
}

bool ApplicationService::isInSelectionGroup(NodeR node)
{
    return SingleInstanceContainer::instance().editorService()->isInSelectionGroup(node);
}

bool ApplicationService::isRedoable() const
{
    return SingleInstanceContainer::instance().editorService()->isRedoable();
}

bool ApplicationService::isModified() const
{
    return SingleInstanceContainer::instance().editorService()->isModified();
}

bool ApplicationService::isUndoable() const
{
    return SingleInstanceContainer::instance().editorService()->isUndoable();
}

void ApplicationService::mirror(bool vertically)
{
    SingleInstanceContainer::instance().editorService()->mirror(vertically);
}

void ApplicationService::moveSelectionGroup(NodeR reference, QPointF location)
{
    SingleInstanceContainer::instance().editorService()->moveSelectionGroup(reference, location);
}

MindMapDataS ApplicationService::mindMapData() const
{
    return SingleInstanceContainer::instance().editorService()->mindMapData();
}

size_t ApplicationService::nodeCount() const
{
    auto && editorService = SingleInstanceContainer::instance().editorService();
    return editorService->mindMapData() ? editorService->mindMapData()->graph().nodeCount() : 0;
}

bool ApplicationService::nodeHasImageAttached() const
{
    return SingleInstanceContainer::instance().editorService()->nodeHasImageAttached();
}

void ApplicationService::paste()
{
    // Create a new node from OS clipboard if text has been copied
    auto && editorService = SingleInstanceContainer::instance().editorService();
    if (!QApplication::clipboard()->text().isEmpty()) {
        saveUndoPoint();
        const auto node = createAndAddNode(m_editorView->grid().snapToGrid(mouseAction().mappedPos()));
        node->setText(QApplication::clipboard()->text());
        editorService->clearCopyStack();
    } else if (!QApplication::clipboard()->image().isNull()) {
        saveUndoPoint();
        using std::chrono::duration_cast;
        using std::chrono::system_clock;
        const auto node = createAndAddNode(m_editorView->grid().snapToGrid(mouseAction().mappedPos()));
        const auto ts = duration_cast<std::chrono::milliseconds>(system_clock::now().time_since_epoch()).count();
        const Image image(QApplication::clipboard()->image(), "copy-pasted-image-data-" + std::to_string(ts) + ".png");
        const auto id = editorService->mindMapData()->imageManager().addImage(image);
        node->setImageRef(id);
        editorService->clearCopyStack();
    } else { // Paste copied nodes
        if (editorService->copyStackSize()) {
            saveUndoPoint();
            std::map<int, NodeS> nodeMapping;
            juzzlin::L().debug() << "Pasting nodes";
            for (auto && copiedNode : editorService->copiedData().nodes) {
                const auto pastedNode = pasteNodeAt(*copiedNode, m_editorView->grid().snapToGrid(mouseAction().mappedPos() - editorService->copiedData().copyReferencePoint + copiedNode->pos()));
                nodeMapping[copiedNode->index()] = pastedNode;
            }
            juzzlin::L().debug() << "Pasting edges";
            for (auto && copiedEdge : editorService->copiedData().edges) {
                const auto pastedEdge = std::make_shared<SceneItems::Edge>(*copiedEdge.edge);
                pastedEdge->setSourceNode(*nodeMapping[copiedEdge.sourceNodeIndex]);
                pastedEdge->setTargetNode(*nodeMapping[copiedEdge.targetNodeIndex]);
                connectEdgeToUndoMechanism(editorService->addEdge(pastedEdge));
            }
            addExistingGraphToScene();
        }
    }
}

void ApplicationService::performNodeAction(const NodeAction & action)
{
    juzzlin::L().debug() << "Handling NodeAction: " << static_cast<int>(action.type);

    auto && editorService = SingleInstanceContainer::instance().editorService();

    switch (action.type) {
    case NodeAction::Type::None:
        break;
    case NodeAction::Type::AttachImage: {
        const Image image { action.image, action.fileName.toStdString() };
        const auto id = editorService->mindMapData()->imageManager().addImage(image);
        if (editorService->selectionGroupSize()) {
            saveUndoPoint();
            editorService->setImageRefForSelectedNodes(id);
        }
    } break;
    case NodeAction::Type::ConnectSelected:
        connectSelectedNodes();
        break;
    case NodeAction::Type::Copy:
        QApplication::clipboard()->clear();
        editorService->copySelectedNodes();
        break;
    case NodeAction::Type::Delete:
        if (mouseAction().action() == MouseAction::Action::CreateOrConnectNode) {
            m_editorView->resetDummyDragItems();
            mouseAction().clear();
            QApplication::restoreOverrideCursor();
        } else if (mouseAction().action() == MouseAction::Action::None) {
            saveUndoPoint();
            m_editorView->resetDummyDragItems();
            editorService->deleteSelectedNodes();
        } else {
            juzzlin::L().warning() << "Cannot delete node due to incompleted MouseAction: " << static_cast<int>(mouseAction().action());
        }
        break;
    case NodeAction::Type::DisconnectSelected:
        disconnectSelectedNodes();
        break;
    case NodeAction::Type::MirrorLayoutHorizontally:
        saveUndoPoint();
        mirror(false);
        break;
    case NodeAction::Type::MirrorLayoutVertically:
        saveUndoPoint();
        mirror(true);
        break;
    case NodeAction::Type::Paste:
        paste();
        break;
    case NodeAction::Type::RemoveAttachedImage:
        saveUndoPoint();
        editorService->removeImageRefsOfSelectedNodes();
        break;
    case NodeAction::Type::SetNodeColor:
        saveUndoPoint();
        editorService->setColorForSelectedNodes(action.color);
        if (editorService->selectionGroupSize() == 1) {
            editorService->clearSelectionGroup();
        }
        break;
    case NodeAction::Type::SetTextColor:
        saveUndoPoint();
        editorService->setTextColorForSelectedNodes(action.color);
        if (editorService->selectionGroupSize() == 1) {
            editorService->clearSelectionGroup();
        }
        break;
    }
}

bool ApplicationService::openMindMap(QString fileName)
{
    try {
        juzzlin::L().info() << "Loading '" << fileName.toStdString() << "'";
        SingleInstanceContainer::instance().progressManager()->setEnabled(true);
        SingleInstanceContainer::instance().editorService()->loadMindMapData(fileName);
        updateProgress();
        m_editorScene = std::make_unique<EditorScene>();
        updateProgress();
        initializeView();
        updateProgress();
        addExistingGraphToScene(true);
        updateProgress();
        connectGraphToUndoMechanism();
        updateProgress();
        connectGraphToImageManager();
        updateProgress();
        zoomToFit();
        updateProgress();
    } catch (const IO::FileException & e) {
        // Initialize a new mind map to avoid an undefined state.
        initializeNewMindMap();
        m_mainWindow.showErrorDialog(e.message());
        SingleInstanceContainer::instance().progressManager()->setEnabled(false);
        return false;
    } catch (const std::runtime_error & e) {
        // Initialize a new mind map to avoid an undefined state.
        initializeNewMindMap();
        m_mainWindow.showErrorDialog(e.what());
        SingleInstanceContainer::instance().progressManager()->setEnabled(false);
        return false;
    }
    SingleInstanceContainer::instance().progressManager()->setEnabled(false);
    return true;
}

void ApplicationService::redo()
{
    L().debug() << "Undo..";

    m_editorView->resetDummyDragItems();
    SingleInstanceContainer::instance().editorService()->redo();

    setupMindMapAfterUndoOrRedo();
}

void ApplicationService::removeItem(QGraphicsItem & item)
{
    m_editorScene->removeItem(&item);
}

void ApplicationService::toggleNodeInSelectionGroup(NodeR node, bool updateNodeConnectionActions)
{
    SingleInstanceContainer::instance().editorService()->toggleNodeInSelectionGroup(node);
    if (updateNodeConnectionActions) {
        this->updateNodeConnectionActions();
    }
}

bool ApplicationService::saveMindMapAs(QString fileName)
{
    return SingleInstanceContainer::instance().editorService()->saveMindMapAs(fileName, true);
}

bool ApplicationService::saveMindMap()
{
    return SingleInstanceContainer::instance().editorService()->saveMindMap(true);
}

void ApplicationService::saveUndoPoint()
{
    SingleInstanceContainer::instance().editorService()->saveUndoPoint();
}

QSize ApplicationService::sceneRectSize() const
{
    return m_editorScene->sceneRect().size().toSize();
}

EdgeP ApplicationService::selectedEdge() const
{
    return SingleInstanceContainer::instance().editorService()->selectedEdge();
}

NodeP ApplicationService::selectedNode() const
{
    return SingleInstanceContainer::instance().editorService()->selectedNode();
}

size_t ApplicationService::selectionGroupSize() const
{
    return SingleInstanceContainer::instance().editorService()->selectionGroupSize();
}

void ApplicationService::setArrowSize(double arrowSize)
{
    // Break loop with the spinbox
    if (auto && editorService = SingleInstanceContainer::instance().editorService(); !qFuzzyCompare(editorService->mindMapData()->arrowSize(), arrowSize)) {
        saveUndoPoint();
        editorService->mindMapData()->setArrowSize(arrowSize);
        m_editorView->setArrowSize(editorService->mindMapData()->arrowSize());
    }
}

void ApplicationService::setBackgroundColor(QColor color)
{
    if (auto && editorService = SingleInstanceContainer::instance().editorService(); editorService->mindMapData()->backgroundColor() != color) {
        saveUndoPoint();
        editorService->mindMapData()->setBackgroundColor(color);
        m_editorView->setBackgroundBrush(QBrush(color));
    }
}

void ApplicationService::setCornerRadius(int value)
{
    // Break loop with the spinbox
    if (auto && editorService = SingleInstanceContainer::instance().editorService(); editorService->mindMapData()->cornerRadius() != value) {
        saveUndoPoint();
        editorService->mindMapData()->setCornerRadius(value);
        m_editorView->setCornerRadius(editorService->mindMapData()->cornerRadius());
    }
}

void ApplicationService::setEdgeColor(QColor color)
{
    if (auto && editorService = SingleInstanceContainer::instance().editorService(); editorService->mindMapData()->edgeColor() != color) {
        saveUndoPoint();
        editorService->mindMapData()->setEdgeColor(color);
        m_editorView->setEdgeColor(color);
    }
}

void ApplicationService::setEdgeWidth(double value)
{
    // Break loop with the spinbox
    if (auto && editorService = SingleInstanceContainer::instance().editorService(); !qFuzzyCompare(editorService->mindMapData()->edgeWidth(), value)) {
        saveUndoPoint();
        editorService->mindMapData()->setEdgeWidth(value);
        m_editorView->setEdgeWidth(editorService->mindMapData()->edgeWidth());
    }
}

void ApplicationService::setGridColor(QColor color)
{
    if (auto && editorService = SingleInstanceContainer::instance().editorService(); editorService->mindMapData()->gridColor() != color) {
        saveUndoPoint();
        editorService->mindMapData()->setGridColor(color);
        m_editorView->setGridColor(color);
        m_editorView->scene()->update();
    }
}

void ApplicationService::setEditorView(EditorView & editorView)
{
    m_editorView = &editorView;

    m_editorView->setParent(&m_mainWindow);

    connect(m_editorView, &EditorView::newNodeRequested, this, [=](QPointF position) {
        saveUndoPoint();
        createAndAddNode(position);
    });
}

size_t ApplicationService::setRectagleSelection(QRectF rect)
{
    size_t nodesInRectangle = 0;
    for (auto && item : m_editorScene->items(rect, SingleInstanceContainer::instance().settingsProxy()->selectNodeGroupByIntersection() ? Qt::IntersectsItemShape : Qt::ContainsItemShape)) {
        if (const auto node = dynamic_cast<NodeP>(item)) {
            toggleNodeInSelectionGroup(*node, false);
            nodesInRectangle++;
        }
    }
    updateNodeConnectionActions();
    return nodesInRectangle;
}

void ApplicationService::setSelectedEdge(EdgeP edge)
{
    L().debug() << __func__ << "(): " << reinterpret_cast<uint64_t>(edge);

    auto && editorService = SingleInstanceContainer::instance().editorService();
    if (editorService->selectedEdge()) {
        editorService->selectedEdge()->setSelected(false);
    }

    if (edge) {
        edge->setSelected(true);
    }

    editorService->setSelectedEdge(edge);
}

void ApplicationService::setSearchText(QString text)
{
    // Leave zoom setting as it is if user has cleared selected nodes and search field.
    // Otherwise zoom in to search results and select matching texts.
    auto && editorService = SingleInstanceContainer::instance().editorService();
    if (text.isEmpty() && !editorService->selectionGroupSize()) {
        editorService->selectNodesByText("");
    } else {
        editorService->selectNodesByText(text);
        if (const auto selectedNodes = editorService->selectedNodes(); selectedNodes.size()) {
            m_editorView->zoomToFit(MagicZoom::calculateRectangleByNodes(selectedNodes));
        } else {
            zoomToFit();
        }
    }

    updateNodeConnectionActions();
}

void ApplicationService::setShadowEffect(const ShadowEffectParams & params)
{
    SingleInstanceContainer::instance().editorService()->mindMapData()->setShadowEffect(params);
}

void ApplicationService::setGridSize(int size, bool autoSnap)
{
    m_editorView->setGridSize(size);
    SingleInstanceContainer::instance().editorService()->setGridSize(size, autoSnap);
}

void ApplicationService::setTextSize(int textSize)
{
    // Break loop with the spinbox
    auto && editorService = SingleInstanceContainer::instance().editorService();
    if (editorService->mindMapData()->textSize() != textSize) {
        saveUndoPoint();
        editorService->mindMapData()->setTextSize(textSize);
    }
}

void ApplicationService::setupMindMapAfterUndoOrRedo()
{
    const auto oldSceneRect = m_editorScene->sceneRect();
    const auto oldCenter = m_editorView->mapToScene(m_editorView->viewport()->rect()).boundingRect().center();

    m_editorScene = std::make_unique<EditorScene>();
    m_editorView->setScene(m_editorScene.get());
    m_editorView->setBackgroundBrush(QBrush(SingleInstanceContainer::instance().editorService()->backgroundColor()));

    addExistingGraphToScene();

    connectGraphToUndoMechanism();
    connectGraphToImageManager();

    m_editorScene->setSceneRect(oldSceneRect);
    m_editorView->centerOn(oldCenter);
}

void ApplicationService::showStatusText(QString statusText)
{
    m_editorView->showStatusText(statusText);
}

void ApplicationService::updateNodeConnectionActions()
{
    m_mainWindow.enableConnectSelectedNodesAction(areSelectedNodesConnectable());
    m_mainWindow.enableDisconnectSelectedNodesAction(areSelectedNodesDisconnectable());
}

void ApplicationService::updateProgress()
{
    SingleInstanceContainer::instance().progressManager()->updateProgress();
}

void ApplicationService::undo()
{
    L().debug() << "Undo..";

    m_editorView->resetDummyDragItems();
    SingleInstanceContainer::instance().editorService()->undo();

    setupMindMapAfterUndoOrRedo();
}

void ApplicationService::unselectText()
{
    SingleInstanceContainer::instance().editorService()->unselectText();
}

void ApplicationService::zoomIn()
{
    m_editorView->zoom(std::pow(Constants::View::ZOOM_SENSITIVITY, 2));
}

void ApplicationService::zoomOut()
{
    m_editorView->zoom(1.0 / std::pow(Constants::View::ZOOM_SENSITIVITY, 2));
}

QSize ApplicationService::zoomForExport(bool dryRun)
{
    unselectSelectedNode();
    clearSelectionGroup();
    const auto zoomToFitRectangle = m_editorScene->calculateZoomToFitRectangle(true);
    if (!dryRun) {
        m_editorScene->setSceneRect(zoomToFitRectangle);
    }
    return zoomToFitRectangle.size().toSize();
}

void ApplicationService::zoomToFit()
{
    if (hasNodes()) {
        m_editorView->zoomToFit(m_editorScene->calculateZoomToFitRectangle());
    }
}

double ApplicationService::calculateNodeOverlapScore(NodeCR node1, NodeCR node2) const
{
    if (&node1 == &node2) {
        return 0;
    }

    const auto rect1 = node1.boundingRect().translated(node1.pos());
    const auto rect2 = node2.boundingRect().translated(node2.pos());

    if (rect1.intersects(rect2)) {
        auto combined = rect1;
        combined = combined.united(rect2);

        const auto biggestArea = std::max(rect1.width() * rect1.height(), rect2.height() * rect2.width());

        return biggestArea / combined.width() / combined.height();
    }

    return 0;
}

void ApplicationService::unselectImplicitlySelectedNodes()
{
    SingleInstanceContainer::instance().editorService()->clearSelectionGroup(true);
}

void ApplicationService::unselectSelectedNode()
{
    for (auto && node : SingleInstanceContainer::instance().editorService()->mindMapData()->graph().getNodes()) {
        node->setSelected(false);
    }
}

NodeS ApplicationService::getBestOverlapNode(NodeCR source)
{
    NodeS bestNode;
    double bestScore = 0;
    const double minThreshold = 0.25;
    for (auto && node : SingleInstanceContainer::instance().editorService()->mindMapData()->graph().getNodes()) {
        if (node->index() != source.index() && node->index() != mouseAction().sourceNode()->index() && !areDirectlyConnected(*node, *mouseAction().sourceNode())) {
            if (const auto score = calculateNodeOverlapScore(source, *node); score > minThreshold && score > bestScore) {
                bestNode = node;
                bestScore = score;
            }
        }
    }

    return bestNode;
}

ApplicationService::~ApplicationService() = default;
