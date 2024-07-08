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

#include "../application//editor_service.hpp"
#include "../application/progress_manager.hpp"
#include "../application/service_container.hpp"
#include "../application/settings_proxy.hpp"
#include "../domain/graph.hpp"
#include "../domain/image_manager.hpp"
#include "../infra/export_params.hpp"
#include "../infra/io/file_exception.hpp"
#include "../infra/settings.hpp"
#include "../view/edge_action.hpp"
#include "../view/editor_scene.hpp"
#include "../view/editor_view.hpp"
#include "../view/magic_zoom.hpp"
#include "../view/main_window.hpp"
#include "../view/mouse_action.hpp"
#include "../view/node_action.hpp"
#include "../view/scene_items/node_handle.hpp"
#include "../view/shadow_effect_params.hpp"

#include "simple_logger.hpp"

#include <QApplication>
#include <QClipboard>
#include <QFileInfo>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QImage>
#include <QMimeData>
#include <QSizePolicy>

#include <chrono>
#include <cmath>

using juzzlin::L;

static const auto TAG = "ApplicationService";

ApplicationService::ApplicationService(MainWindowS mainWindow)
  : m_editorService(std::make_unique<EditorService>())
  , m_mainWindow(mainWindow)
{
    connect(m_mainWindow.get(), &MainWindow::arrowSizeChanged, this, &ApplicationService::setArrowSize);
    connect(m_mainWindow.get(), &MainWindow::autosaveEnabled, this, &ApplicationService::enableAutosave);
    connect(m_mainWindow.get(), &MainWindow::cornerRadiusChanged, this, &ApplicationService::setCornerRadius);
    connect(m_mainWindow.get(), &MainWindow::edgeWidthChanged, this, &ApplicationService::setEdgeWidth);
    connect(m_mainWindow.get(), &MainWindow::fontChanged, this, &ApplicationService::changeFont);
    connect(m_mainWindow.get(), &MainWindow::gridSizeChanged, this, &ApplicationService::setGridSize);
    connect(m_mainWindow.get(), &MainWindow::searchTextChanged, this, &ApplicationService::setSearchText);
    connect(m_mainWindow.get(), &MainWindow::shadowEffectChanged, this, &ApplicationService::setShadowEffect);
    connect(m_mainWindow.get(), &MainWindow::textSizeChanged, this, &ApplicationService::setTextSize);
    connect(m_mainWindow.get(), &MainWindow::zoomInRequested, this, &ApplicationService::zoomIn);
    connect(m_mainWindow.get(), &MainWindow::zoomOutRequested, this, &ApplicationService::zoomOut);
    connect(m_mainWindow.get(), &MainWindow::zoomToFitRequested, this, &ApplicationService::zoomToFit);

    connect(this, &ApplicationService::currentSearchTextRequested, m_mainWindow.get(), &MainWindow::requestCurrentSearchText);

    connect(m_editorService.get(), &EditorService::isModifiedChanged, m_mainWindow.get(), [=](bool isModified) {
        m_mainWindow->enableSave(isModified || canBeSaved());
    });
    connect(m_editorService.get(), &EditorService::redoEnabled, this, &ApplicationService::enableRedo);
    connect(m_editorService.get(), &EditorService::undoEnabled, this, &ApplicationService::enableUndo);
}

void ApplicationService::addExistingGraphToScene(bool zoomToFitAfterNodesLoaded)
{

    for (auto && node : m_editorService->mindMapData()->graph().getNodes()) {
        if (node->scene() != m_editorScene.get()) {
            addItem(*node);
            node->setCornerRadius(m_editorService->mindMapData()->cornerRadius());
            node->setTextSize(m_editorService->mindMapData()->textSize());
            node->changeFont(m_editorService->mindMapData()->font());
            L(TAG).trace() << "Added existing node id=" << node->index() << " to scene";
        }
    }

    if (zoomToFitAfterNodesLoaded) {
        zoomToFit();
    }

    updateProgress();

    size_t progressCounter = 0;
    for (auto && edge : m_editorService->mindMapData()->graph().getEdges()) {
        const auto node0 = getNodeByIndex(edge->sourceNode().index());
        const auto node1 = getNodeByIndex(edge->targetNode().index());
        if (!m_editorScene->hasEdge(*node0, *node1)) {
            addItem(*edge, false);
            edge->setArrowSize(m_editorService->mindMapData()->arrowSize());
            edge->setColor(m_editorService->mindMapData()->edgeColor());
            edge->setEdgeWidth(m_editorService->mindMapData()->edgeWidth());
            edge->setTextSize(m_editorService->mindMapData()->textSize());
            edge->changeFont(m_editorService->mindMapData()->font());
            node0->addGraphicsEdge(*edge);
            node1->addGraphicsEdge(*edge);
            edge->updateLine();
            L(TAG).trace() << "Added existing edge (" << node0->index() << ", " << node1->index() << ") to scene";
        }
        if (++progressCounter % 100 == 0) {
            updateProgress();
        }
    }

    updateProgress();

    // This is to prevent nasty updated loops like in https://github.com/juzzlin/Heimer/issues/96
    m_mainWindow->enableWidgetSignals(false);

    updateProgress();

    m_mainWindow->setArrowSize(m_editorService->mindMapData()->arrowSize());
    m_mainWindow->setCornerRadius(m_editorService->mindMapData()->cornerRadius());
    m_mainWindow->setEdgeWidth(m_editorService->mindMapData()->edgeWidth());
    m_mainWindow->setTextSize(m_editorService->mindMapData()->textSize());
    m_mainWindow->changeFont(m_editorService->mindMapData()->font());

    updateProgress();

    m_editorView->setArrowSize(m_editorService->mindMapData()->arrowSize());
    m_editorView->setCornerRadius(m_editorService->mindMapData()->cornerRadius());
    m_editorView->setEdgeColor(m_editorService->mindMapData()->edgeColor());
    m_editorView->setEdgeWidth(m_editorService->mindMapData()->edgeWidth());

    updateProgress();

    m_mainWindow->enableWidgetSignals(true);
}

void ApplicationService::addEdge(NodeR node1, NodeR node2)
{
    // Add edge from node1 to node2
    connectEdgeToUndoMechanism(m_editorService->addEdge(std::make_shared<SceneItems::Edge>(&node1, &node2)));
    L(TAG).debug() << "Created a new edge " << node1.index() << " -> " << node2.index();

    addExistingGraphToScene();
}

void ApplicationService::addItem(QGraphicsItem & item, bool adjustSceneRect)
{
    m_editorScene->addItem(&item);
    if (adjustSceneRect) {
        this->adjustSceneRect();
    }
}

void ApplicationService::addEdgeToSelectionGroup(EdgeR edge, bool isImplicit)
{
    m_editorService->addEdgeToSelectionGroup(edge, isImplicit);
}

void ApplicationService::addNodeToSelectionGroup(NodeR node, bool isImplicit)
{
    m_editorService->addNodeToSelectionGroup(node, isImplicit);
    updateNodeConnectionActions();
}

void ApplicationService::adjustSceneRect()
{
    m_editorScene->adjustSceneRect();
}

void ApplicationService::clearEdgeSelectionGroup(bool implicitOnly)
{
    m_editorService->clearEdgeSelectionGroup(implicitOnly);
}

void ApplicationService::clearNodeSelectionGroup(bool implicitOnly)
{
    m_editorService->clearNodeSelectionGroup(implicitOnly);
    updateNodeConnectionActions();
}

bool ApplicationService::canBeSaved() const
{
    return !m_editorService->fileName().isEmpty();
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
    connect(node.get(), &SceneItems::Node::imageRequested, &m_editorService->mindMapData()->imageManager(), &ImageManager::handleImageRequest, Qt::UniqueConnection);
    node->setImageRef(node->imageRef()); // This effectively results in a fetch from ImageManager
}

size_t ApplicationService::copyStackSize() const
{
    return m_editorService->copyStackSize();
}

void ApplicationService::connectGraphToUndoMechanism()
{
    for (auto && node : m_editorService->mindMapData()->graph().getNodes()) {
        connectNodeToUndoMechanism(node);
    }

    for (auto && edge : m_editorService->mindMapData()->graph().getEdges()) {
        connectEdgeToUndoMechanism(edge);
    }
}

void ApplicationService::connectGraphToImageManager()
{
    for (auto && node : m_editorService->mindMapData()->graph().getNodes()) {
        connectNodeToImageManager(node);
    }
}

void ApplicationService::connectSelectedNodes()
{
    L(TAG).debug() << "Connecting selected nodes: " << m_editorService->nodeSelectionGroupSize();
    if (areSelectedNodesConnectable()) {
        saveUndoPoint();
        for (auto && edge : m_editorService->connectSelectedNodes()) {
            connectEdgeToUndoMechanism(edge);
        }
        addExistingGraphToScene();
        updateNodeConnectionActions();
    }
}

void ApplicationService::disconnectSelectedNodes()
{
    L(TAG).debug() << "Disconnecting selected nodes: " << m_editorService->nodeSelectionGroupSize();
    if (areSelectedNodesDisconnectable()) {
        saveUndoPoint();
        m_editorService->disconnectSelectedNodes();
        updateNodeConnectionActions();
    }
}

void ApplicationService::changeFont(const QFont & font)
{
    saveUndoPoint();
    m_editorService->mindMapData()->changeFont(font);
}

NodeS ApplicationService::createAndAddNode(int sourceNodeIndex, QPointF pos)
{
    const auto node0 = getNodeByIndex(sourceNodeIndex);
    const auto node1 = m_mainWindow->copyOnDragEnabled() ? m_editorService->copyNodeAt(*node0, pos) : m_editorService->addNodeAt(pos);
    connectNodeToUndoMechanism(node1);
    connectNodeToImageManager(node1);
    L(TAG).debug() << "Created a new node at (" << pos.x() << "," << pos.y() << ")";

    // Add edge from the parent node.
    connectEdgeToUndoMechanism(m_editorService->addEdge(std::make_shared<SceneItems::Edge>(node0.get(), node1.get())));
    L(TAG).debug() << "Created a new edge " << node0->index() << " -> " << node1->index();

    addExistingGraphToScene();

    node1->setText("");
    node1->setTextInputActive(true);

    return node1;
}

NodeS ApplicationService::createAndAddNode(QPointF pos)
{
    const auto node1 = m_editorService->addNodeAt(m_editorView->grid().snapToGrid(pos));
    connectNodeToUndoMechanism(node1);
    connectNodeToImageManager(node1);
    L(TAG).debug() << "Created a new node at (" << pos.x() << "," << pos.y() << ")";

    addExistingGraphToScene();

    QTimer::singleShot(0, this, [node1] { // Needed due to the context menu
        node1->setTextInputActive(true);
    });

    return node1;
}

NodeS ApplicationService::pasteNodeAt(NodeR source, QPointF pos)
{
    const auto copiedNode = m_editorService->copyNodeAt(source, pos);
    connectNodeToUndoMechanism(copiedNode);
    connectNodeToImageManager(copiedNode);
    L(TAG).debug() << "Pasted node at (" << pos.x() << "," << pos.y() << ")";
    return copiedNode;
}

MouseAction & ApplicationService::mouseAction()
{
    return m_editorService->mouseAction();
}

void ApplicationService::deleteEdge(EdgeR edge)
{
    m_editorService->deleteEdge(edge);
}

void ApplicationService::enableAutosave(bool enable)
{
    if (enable) {
        m_editorService->requestAutosave(EditorService::AutosaveContext::Modification, true);
    }
}

void ApplicationService::enableUndo(bool enable)
{
    m_mainWindow->enableUndo(enable);
}

void ApplicationService::enableRedo(bool enable)
{
    m_mainWindow->enableRedo(enable);
}

ApplicationService::GridLineVector ApplicationService::addGridForExport()
{
    GridLineVector addedLineItems;
    if (Settings::Custom::loadGridVisibleState()) {
        const auto gridLines = m_editorView->grid().calculateLines(m_editorScene->sceneRect());
        const auto gridColor = SC::instance().applicationService()->mindMapData()->gridColor();
        for (auto && gridLine : gridLines) {
            addedLineItems.push_back(m_editorScene->addLine(gridLine, gridColor));
        }
    }
    return addedLineItems;
}

void ApplicationService::removeLineItems(const GridLineVector & lines)
{
    for (auto && line : lines) {
        m_editorScene->removeItem(line);
    }
}

void ApplicationService::exportToPng(const ExportParams & exportParams)
{
    m_editorView->saveZoom();
    zoomForExport();

    L(TAG).info() << "Exporting a PNG image of size (" << exportParams.imageSize.width() << "x" << exportParams.imageSize.height() << ") to " << exportParams.fileName.toStdString();
    const auto lines = addGridForExport();
    const auto image = m_editorScene->toImage(exportParams.imageSize, m_editorService->backgroundColor(), exportParams.transparentBackground);
    m_editorView->restoreZoom();
    removeLineItems(lines);

    emit pngExportFinished(image.save(exportParams.fileName));
}

void ApplicationService::exportToSvg(const ExportParams & exportParams)
{
    m_editorView->saveZoom();
    zoomForExport();

    L(TAG).info() << "Exporting an SVG image to " << exportParams.fileName.toStdString();
    const auto lines = addGridForExport();
    m_editorScene->toSvg(exportParams.fileName, QFileInfo { exportParams.fileName }.fileName());
    m_editorView->restoreZoom();
    removeLineItems(lines);

    emit svgExportFinished(true);
}

QString ApplicationService::fileName() const
{
    return m_editorService->fileName();
}

NodeS ApplicationService::getNodeByIndex(int index)
{
    return m_editorService->getNodeByIndex(index);
}

bool ApplicationService::hasNodes() const
{
    return m_editorService->mindMapData() && m_editorService->mindMapData()->graph().nodeCount();
}

void ApplicationService::initializeNewMindMap()
{
    L(TAG).debug() << "Initializing a new mind map";

    m_editorScene = std::make_unique<EditorScene>();
    m_editorService->initializeNewMindMap();

    initializeView();

    const auto node = m_editorService->addNodeAt(QPointF(0, 0));
    connectNodeToUndoMechanism(node);
    connectNodeToImageManager(node);

    addExistingGraphToScene();

    QTimer::singleShot(0, this, &ApplicationService::zoomToFit);

    m_mainWindow->initializeNewMindMap();
}

void ApplicationService::initiateNewNodeDrag(SceneItems::NodeHandle & nodeHandle)
{
    L(TAG).debug() << "Initiating new node drag";

    clearNodeSelectionGroup();
    saveUndoPoint();
    mouseAction().setSourceNode(&nodeHandle.parentNode(), MouseAction::Action::CreateOrConnectNode);
    nodeHandle.parentNode().setHandlesVisible(false);

    // Change cursor to the closed hand cursor.
    QApplication::setOverrideCursor(QCursor(Qt::ClosedHandCursor));
}

void ApplicationService::initiateNodeDrag(NodeR node)
{
    L(TAG).debug() << "Initiating node drag";

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
    L(TAG).debug() << "Initializing view";

    // Set scene to the view
    m_editorView->setScene(m_editorScene.get());
    m_editorView->resetDummyDragItems();

    m_editorView->setBackgroundBrush(QBrush(m_editorService->backgroundColor()));

    m_mainWindow->setCentralWidget(m_editorView);
    m_mainWindow->setContentsMargins(0, 0, 0, 0);
}

bool ApplicationService::areDirectlyConnected(NodeCR node1, NodeCR node2) const
{
    auto && graph = m_editorService->mindMapData()->graph();
    const auto node1Ptr = graph.getNode(node1.index());
    const auto node2Ptr = graph.getNode(node2.index());
    return graph.areDirectlyConnected(node1Ptr, node2Ptr);
}

bool ApplicationService::areSelectedNodesConnectable() const
{
    return m_editorService->areSelectedNodesConnectable();
}

bool ApplicationService::areSelectedNodesDisconnectable() const
{
    return m_editorService->areSelectedNodesDisconnectable();
}

bool ApplicationService::isLeafNode(NodeR node)
{
    auto && graph = m_editorService->mindMapData()->graph();
    return graph.getEdgesFromNode(graph.getNode(node.index())).size() + graph.getEdgesToNode(graph.getNode(node.index())).size() <= 1;
}

bool ApplicationService::isInBetween(NodeR node)
{
    auto && graph = m_editorService->mindMapData()->graph();
    return graph.getEdgesFromNode(graph.getNode(node.index())).size() + graph.getEdgesToNode(graph.getNode(node.index())).size() == 2;
}

bool ApplicationService::isInSelectionGroup(NodeR node)
{
    return m_editorService->isInSelectionGroup(node);
}

bool ApplicationService::isRedoable() const
{
    return m_editorService->isRedoable();
}

bool ApplicationService::isModified() const
{
    return m_editorService->isModified();
}

bool ApplicationService::isUndoable() const
{
    return m_editorService->isUndoable();
}

void ApplicationService::mirror(bool vertically)
{
    m_editorService->mirror(vertically);
}

void ApplicationService::moveSelectionGroup(NodeR reference, QPointF location)
{
    m_editorService->moveSelectionGroup(reference, location);
}

MindMapDataS ApplicationService::mindMapData() const
{
    return m_editorService->mindMapData();
}

size_t ApplicationService::nodeCount() const
{
    return m_editorService->mindMapData() ? m_editorService->mindMapData()->graph().nodeCount() : 0;
}

bool ApplicationService::nodeHasImageAttached() const
{
    return m_editorService->nodeHasImageAttached();
}

QSizeF ApplicationService::normalizedSizeInView(const QRectF & rectInScene) const
{
    const auto rectInView = m_editorView->mapFromScene(rectInScene).boundingRect();
    return { static_cast<double>(rectInView.width()) / m_editorView->width(), static_cast<double>(rectInView.height()) / m_editorView->height() };
}

void ApplicationService::paste()
{
    // Create a new node from OS clipboard if text has been copied

    if (!QApplication::clipboard()->text().isEmpty()) {
        saveUndoPoint();
        const auto node = createAndAddNode(m_editorView->grid().snapToGrid(mouseAction().mappedPos()));
        node->setText(QApplication::clipboard()->text());
        m_editorService->clearCopyStack();
    } else if (!QApplication::clipboard()->image().isNull()) {
        saveUndoPoint();
        using std::chrono::duration_cast;
        using std::chrono::system_clock;
        const auto node = createAndAddNode(m_editorView->grid().snapToGrid(mouseAction().mappedPos()));
        const auto ts = duration_cast<std::chrono::milliseconds>(system_clock::now().time_since_epoch()).count();
        const Image image(QApplication::clipboard()->image(), "copy-pasted-image-data-" + std::to_string(ts) + ".png");
        const auto id = m_editorService->mindMapData()->imageManager().addImage(image);
        node->setImageRef(id);
        m_editorService->clearCopyStack();
    } else { // Paste copied nodes
        if (m_editorService->copyStackSize()) {
            saveUndoPoint();
            std::map<int, NodeS> nodeMapping;
            juzzlin::L(TAG).debug() << "Pasting nodes";
            for (auto && copiedNode : m_editorService->copiedData().nodes) {
                const auto pastedNode = pasteNodeAt(*copiedNode, m_editorView->grid().snapToGrid(mouseAction().mappedPos() - m_editorService->copiedData().copyReferencePoint + copiedNode->pos()));
                nodeMapping[copiedNode->index()] = pastedNode;
            }
            juzzlin::L(TAG).debug() << "Pasting edges";
            for (auto && copiedEdge : m_editorService->copiedData().edges) {
                const auto pastedEdge = std::make_shared<SceneItems::Edge>(*copiedEdge.edge);
                pastedEdge->setSourceNode(*nodeMapping[copiedEdge.sourceNodeIndex]);
                pastedEdge->setTargetNode(*nodeMapping[copiedEdge.targetNodeIndex]);
                connectEdgeToUndoMechanism(m_editorService->addEdge(pastedEdge));
            }
            addExistingGraphToScene();
        }
    }
}

void ApplicationService::performEdgeAction(const EdgeAction & action)
{
    juzzlin::L(TAG).debug() << "Handling EdgeAction: " << static_cast<int>(action.type());

    switch (action.type()) {
    case EdgeAction::Type::None:
        break;
    case EdgeAction::Type::Delete:
        saveUndoPoint();
        m_editorService->deleteSelectedEdges();
        break;
    }
}

void ApplicationService::performNodeAction(const NodeAction & action)
{
    juzzlin::L(TAG).debug() << "Handling NodeAction: " << static_cast<int>(action.type());

    switch (action.type()) {
    case NodeAction::Type::None:
        break;
    case NodeAction::Type::AttachImage: {
        const Image image { action.image(), action.fileName().toStdString() };
        const auto id = m_editorService->mindMapData()->imageManager().addImage(image);
        if (m_editorService->nodeSelectionGroupSize()) {
            saveUndoPoint();
            m_editorService->setImageRefForSelectedNodes(id);
        }
    } break;
    case NodeAction::Type::ConnectSelected:
        connectSelectedNodes();
        break;
    case NodeAction::Type::Copy:
        QApplication::clipboard()->clear();
        m_editorService->copySelectedNodes();
        break;
    case NodeAction::Type::Delete:
        if (mouseAction().action() == MouseAction::Action::CreateOrConnectNode) {
            m_editorView->resetDummyDragItems();
            mouseAction().clear();
            QApplication::restoreOverrideCursor();
        } else if (mouseAction().action() == MouseAction::Action::None) {
            saveUndoPoint();
            m_editorView->resetDummyDragItems();
            m_editorService->deleteSelectedNodes();
        } else {
            juzzlin::L(TAG).warning() << "Cannot delete node due to incompleted MouseAction: " << static_cast<int>(mouseAction().action());
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
        m_editorService->removeImageRefsOfSelectedNodes();
        break;
    case NodeAction::Type::SetNodeColor:
        saveUndoPoint();
        m_editorService->setColorForSelectedNodes(action.color());
        if (m_editorService->nodeSelectionGroupSize() == 1) {
            m_editorService->clearNodeSelectionGroup();
        }
        break;
    case NodeAction::Type::SetTextColor:
        saveUndoPoint();
        m_editorService->setTextColorForSelectedNodes(action.color());
        if (m_editorService->nodeSelectionGroupSize() == 1) {
            m_editorService->clearNodeSelectionGroup();
        }
        break;
    }
}

bool ApplicationService::openMindMap(QString fileName)
{
    try {
        juzzlin::L(TAG).info() << "Loading '" << fileName.toStdString() << "'";
        SC::instance().progressManager()->setEnabled(true);
        m_editorService->loadMindMapData(fileName);
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
        m_mainWindow->showErrorDialog(e.message());
        SC::instance().progressManager()->setEnabled(false);
        return false;
    } catch (const std::runtime_error & e) {
        // Initialize a new mind map to avoid an undefined state.
        initializeNewMindMap();
        m_mainWindow->showErrorDialog(e.what());
        SC::instance().progressManager()->setEnabled(false);
        return false;
    }
    SC::instance().progressManager()->setEnabled(false);
    return true;
}

void ApplicationService::redo()
{
    L(TAG).debug() << "Undo..";

    m_editorView->resetDummyDragItems();
    m_editorService->redo();

    setupMindMapAfterUndoOrRedo();
}

void ApplicationService::removeItem(QGraphicsItem & item)
{
    m_editorScene->removeItem(&item);
}

void ApplicationService::toggleEdgeInSelectionGroup(EdgeR edge)
{
    m_editorService->toggleEdgeInSelectionGroup(edge);
}

void ApplicationService::toggleNodeInSelectionGroup(NodeR node, bool updateNodeConnectionActions)
{
    m_editorService->toggleNodeInSelectionGroup(node);
    if (updateNodeConnectionActions) {
        this->updateNodeConnectionActions();
    }
}

bool ApplicationService::saveMindMapAs(QString fileName)
{
    return m_editorService->saveMindMapAs(fileName, true);
}

bool ApplicationService::saveMindMap()
{
    return m_editorService->saveMindMap(true);
}

void ApplicationService::saveUndoPoint()
{
    m_editorService->saveUndoPoint();
}

QSize ApplicationService::sceneRectSize() const
{
    return m_editorScene->sceneRect().size().toSize();
}

std::optional<EdgeP> ApplicationService::selectedEdge() const
{
    return m_editorService->selectedEdge();
}

std::optional<NodeP> ApplicationService::selectedNode() const
{
    return m_editorService->selectedNode();
}

size_t ApplicationService::edgeSelectionGroupSize() const
{
    return m_editorService->edgeSelectionGroupSize();
}

size_t ApplicationService::nodeSelectionGroupSize() const
{
    return m_editorService->nodeSelectionGroupSize();
}

void ApplicationService::setArrowSize(double arrowSize)
{
    // Break loop with the spinbox
    if (!qFuzzyCompare(m_editorService->mindMapData()->arrowSize(), arrowSize)) {
        saveUndoPoint();
        m_editorService->mindMapData()->setArrowSize(arrowSize);
        m_editorView->setArrowSize(m_editorService->mindMapData()->arrowSize());
    }
}

void ApplicationService::setBackgroundColor(QColor color)
{
    if (m_editorService->mindMapData()->backgroundColor() != color) {
        saveUndoPoint();
        m_editorService->mindMapData()->setBackgroundColor(color);
        m_editorView->setBackgroundBrush(QBrush(color));
    }
}

void ApplicationService::setCornerRadius(int value)
{
    // Break loop with the spinbox
    if (m_editorService->mindMapData()->cornerRadius() != value) {
        saveUndoPoint();
        m_editorService->mindMapData()->setCornerRadius(value);
        m_editorView->setCornerRadius(m_editorService->mindMapData()->cornerRadius());
    }
}

void ApplicationService::setEdgeColor(QColor color)
{
    if (m_editorService->mindMapData()->edgeColor() != color) {
        saveUndoPoint();
        m_editorService->mindMapData()->setEdgeColor(color);
        m_editorView->setEdgeColor(color);
    }
}

void ApplicationService::setEdgeWidth(double value)
{
    // Break loop with the spinbox
    if (!qFuzzyCompare(m_editorService->mindMapData()->edgeWidth(), value)) {
        saveUndoPoint();
        m_editorService->mindMapData()->setEdgeWidth(value);
        m_editorView->setEdgeWidth(m_editorService->mindMapData()->edgeWidth());
    }
}

void ApplicationService::setGridColor(QColor color)
{
    if (m_editorService->mindMapData()->gridColor() != color) {
        saveUndoPoint();
        m_editorService->mindMapData()->setGridColor(color);
        m_editorView->setGridColor(color);
        m_editorView->scene()->update();
    }
}

void ApplicationService::setEditorView(EditorView & editorView)
{
    m_editorView = &editorView;

    m_editorView->setParent(m_mainWindow.get());

    connect(m_editorView, &EditorView::newNodeRequested, this, [=](QPointF position) {
        saveUndoPoint();
        createAndAddNode(position);
    });
}

size_t ApplicationService::setEdgeRectangleSelection(QRectF rect)
{
    size_t edgesInRectangle = 0;
    for (auto && item : m_editorScene->items(rect, SC::instance().settingsProxy()->selectNodeGroupByIntersection() ? Qt::IntersectsItemShape : Qt::ContainsItemShape)) {
        if (const auto edge = dynamic_cast<EdgeP>(item)) {
            toggleEdgeInSelectionGroup(*edge);
            edgesInRectangle++;
        }
    }
    return edgesInRectangle;
}

size_t ApplicationService::setNodeRectangleSelection(QRectF rect)
{
    size_t nodesInRectangle = 0;
    for (auto && item : m_editorScene->items(rect, SC::instance().settingsProxy()->selectNodeGroupByIntersection() ? Qt::IntersectsItemShape : Qt::ContainsItemShape)) {
        if (const auto node = dynamic_cast<NodeP>(item)) {
            toggleNodeInSelectionGroup(*node, false);
            nodesInRectangle++;
        }
    }
    updateNodeConnectionActions();
    return nodesInRectangle;
}

void ApplicationService::setSearchText(QString text)
{
    // Leave zoom setting as it is if user has cleared selected nodes and search field.
    // Otherwise zoom in to search results and select matching texts.

    if (text.isEmpty() && !m_editorService->edgeSelectionGroupSize() && !m_editorService->nodeSelectionGroupSize()) {
        m_editorService->selectNodesByText("");
        m_editorService->selectEdgesByText("");
    } else {
        m_editorService->selectNodesByText(text);
        const auto nodeRect = MagicZoom::calculateRectangleByNodes(m_editorService->selectedNodes());
        m_editorService->selectEdgesByText(text);
        const auto edgeRect = MagicZoom::calculateRectangleByEdges(m_editorService->selectedEdges());
        if (edgeRect.isValid() && nodeRect.isValid()) {
            L(TAG).trace() << "edgeRect and nodeRect valid";
            m_editorView->zoomToFit(edgeRect.united(nodeRect));
        } else if (edgeRect.isValid()) {
            L(TAG).trace() << "edgeRect valid";
            m_editorView->zoomToFit(edgeRect);
        } else if (nodeRect.isValid()) {
            L(TAG).trace() << "nodeRect valid";
            m_editorView->zoomToFit(nodeRect);
        } else {
            L(TAG).trace() << "Default zoom-to-fit";
            zoomToFit();
        }
    }

    updateNodeConnectionActions();
}

void ApplicationService::setShadowEffect(const ShadowEffectParams & params)
{
    m_editorService->mindMapData()->setShadowEffect(params);
}

void ApplicationService::setGridSize(int size, bool autoSnap)
{
    m_editorView->setGridSize(size);
    m_editorService->setGridSize(size, autoSnap);
}

void ApplicationService::setTextSize(int textSize)
{
    // Break loop with the spinbox

    if (m_editorService->mindMapData()->textSize() != textSize) {
        saveUndoPoint();
        m_editorService->mindMapData()->setTextSize(textSize);
    }
}

void ApplicationService::setupMindMapAfterUndoOrRedo()
{
    const auto oldSceneRect = m_editorScene->sceneRect();
    const auto oldCenter = m_editorView->mapToScene(m_editorView->viewport()->rect()).boundingRect().center();

    m_editorScene = std::make_unique<EditorScene>();
    m_editorView->setScene(m_editorScene.get());
    m_editorView->setBackgroundBrush(QBrush(m_editorService->backgroundColor()));

    addExistingGraphToScene();

    connectGraphToUndoMechanism();
    connectGraphToImageManager();

    m_editorScene->setSceneRect(oldSceneRect);
    m_editorView->centerOn(oldCenter);

    emit currentSearchTextRequested();
}

void ApplicationService::showStatusText(QString statusText)
{
    m_editorView->showStatusText(statusText);
}

void ApplicationService::updateNodeConnectionActions()
{
    m_mainWindow->enableConnectSelectedNodesAction(areSelectedNodesConnectable());
    m_mainWindow->enableDisconnectSelectedNodesAction(areSelectedNodesDisconnectable());
}

void ApplicationService::updateProgress()
{
    SC::instance().progressManager()->updateProgress();
}

void ApplicationService::undo()
{
    L(TAG).debug() << "Undo..";

    m_editorView->resetDummyDragItems();
    m_editorService->undo();

    setupMindMapAfterUndoOrRedo();
}

void ApplicationService::unselectText()
{
    m_editorService->unselectText();
}

void ApplicationService::zoomIn()
{
    m_editorView->zoom(std::pow(Constants::View::zoomSensitivity(), 2));
}

void ApplicationService::zoomOut()
{
    m_editorView->zoom(1.0 / std::pow(Constants::View::zoomSensitivity(), 2));
}

QSize ApplicationService::calculateExportImageSize()
{
    unselectSelectedNode();
    clearSelectionGroups();
    return m_editorView->mapFromScene(m_editorScene->calculateZoomToFitRectangle(true)).boundingRect().size();
}

void ApplicationService::zoomForExport()
{
    unselectSelectedNode();
    clearSelectionGroups();
    const auto zoomToFitRectangle = m_editorScene->calculateZoomToFitRectangle(true);
    m_editorScene->setSceneRect(zoomToFitRectangle);
}

void ApplicationService::zoomToFit()
{
    if (hasNodes()) {
        if (auto && selectedNodes = m_editorService->selectedNodes(); !selectedNodes.empty()) {
            m_editorView->zoomToFit(m_editorScene->calculateZoomToFitRectangleByNodes(selectedNodes));
        } else {
            m_editorView->zoomToFit(m_editorScene->calculateZoomToFitRectangle());
        }
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

void ApplicationService::clearSelectionGroups()
{
    clearEdgeSelectionGroup();
    clearNodeSelectionGroup();
}

void ApplicationService::unselectImplicitlySelectedEdges()
{
    m_editorService->clearEdgeSelectionGroup(true);
}

void ApplicationService::unselectImplicitlySelectedNodes()
{
    m_editorService->clearNodeSelectionGroup(true);
}

void ApplicationService::unselectSelectedNode()
{
    for (auto && node : m_editorService->mindMapData()->graph().getNodes()) {
        node->setSelected(false);
    }
}

NodeS ApplicationService::getBestOverlapNode(NodeCR source)
{
    NodeS bestNode;
    double bestScore = 0;
    const double minThreshold = 0.25;
    for (auto && node : m_editorService->mindMapData()->graph().getNodes()) {
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
