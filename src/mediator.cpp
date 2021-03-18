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

#include "defaults.hpp"
#include "editor_data.hpp"
#include "editor_scene.hpp"
#include "editor_view.hpp"
#include "image_manager.hpp"
#include "main_window.hpp"
#include "mouse_action.hpp"
#include "node_action.hpp"
#include "node_handle.hpp"

#include "simple_logger.hpp"

#include <QApplication>
#include <QFileInfo>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QSizePolicy>

#include <cassert>
#include <cmath>

using juzzlin::L;

Mediator::Mediator(MainWindow & mainWindow)
  : m_mainWindow(mainWindow)
{
    connect(&m_mainWindow, &MainWindow::zoomToFitTriggered, this, &Mediator::zoomToFit);
    connect(&m_mainWindow, &MainWindow::zoomInTriggered, this, &Mediator::zoomIn);
    connect(&m_mainWindow, &MainWindow::zoomOutTriggered, this, &Mediator::zoomOut);
}

void Mediator::addExistingGraphToScene()
{
    for (auto && node : m_editorData->mindMapData()->graph().getNodes()) {
        if (node->scene() != m_editorScene.get()) {
            addItem(*node);
            node->setCornerRadius(m_editorData->mindMapData()->cornerRadius());
            node->setTextSize(m_editorData->mindMapData()->textSize());
            L().debug() << "Added existing node " << node->index() << " to scene";
        }
    }

    for (auto && edge : m_editorData->mindMapData()->graph().getEdges()) {
        const auto node0 = getNodeByIndex(edge->sourceNode().index());
        const auto node1 = getNodeByIndex(edge->targetNode().index());
        if (!m_editorScene->hasEdge(*node0, *node1)) {
            addItem(*edge);
            edge->setColor(m_editorData->mindMapData()->edgeColor());
            edge->setWidth(m_editorData->mindMapData()->edgeWidth());
            edge->setTextSize(m_editorData->mindMapData()->textSize());
            node0->addGraphicsEdge(*edge);
            node1->addGraphicsEdge(*edge);
            edge->updateLine();
            L().debug() << "Added existing edge " << node0->index() << " -> " << node1->index() << " to scene";
        }
    }

    // This is to prevent nasty updated loops like in https://github.com/juzzlin/Heimer/issues/96
    m_mainWindow.enableWidgetSignals(false);

    m_mainWindow.setCornerRadius(m_editorData->mindMapData()->cornerRadius());
    m_mainWindow.setEdgeWidth(m_editorData->mindMapData()->edgeWidth());
    m_mainWindow.setTextSize(m_editorData->mindMapData()->textSize());

    m_editorView->setCornerRadius(m_editorData->mindMapData()->cornerRadius());
    m_editorView->setEdgeColor(m_editorData->mindMapData()->edgeColor());
    m_editorView->setEdgeWidth(m_editorData->mindMapData()->edgeWidth());

    m_mainWindow.enableWidgetSignals(true);
}

void Mediator::addEdge(Node & node1, Node & node2)
{
    // Add edge from node1 to node2
    connectEdgeToUndoMechanism(m_editorData->addEdge(std::make_shared<Edge>(node1, node2)));
    L().debug() << "Created a new edge " << node1.index() << " -> " << node2.index();

    addExistingGraphToScene();
}

void Mediator::addItem(QGraphicsItem & item)
{
    m_editorScene->addItem(&item);
    adjustSceneRect();
}

void Mediator::addNodeToSelectionGroup(Node & node)
{
    m_editorData->addNodeToSelectionGroup(node);
    updateNodeConnectionActions();
}

void Mediator::adjustSceneRect()
{
    m_editorScene->adjustSceneRect();
}

void Mediator::clearSelectionGroup()
{
    m_editorData->clearSelectionGroup();
    updateNodeConnectionActions();
}

bool Mediator::canBeSaved() const
{
    return !m_editorData->fileName().isEmpty();
}

void Mediator::connectEdgeToUndoMechanism(EdgePtr edge)
{
    connect(edge.get(), &Edge::undoPointRequested, this, &Mediator::saveUndoPoint, Qt::UniqueConnection);
}

void Mediator::connectNodeToUndoMechanism(NodePtr node)
{
    connect(node.get(), &Node::undoPointRequested, this, &Mediator::saveUndoPoint, Qt::UniqueConnection);
}

void Mediator::connectNodeToImageManager(NodePtr node)
{
    connect(node.get(), &Node::imageRequested, &m_editorData->mindMapData()->imageManager(), &ImageManager::handleImageRequest, Qt::UniqueConnection);
    node->setImageRef(node->imageRef()); // This effectively results in a fetch from ImageManager
}

size_t Mediator::copyStackSize() const
{
    return m_editorData->copyStackSize();
}

void Mediator::connectGraphToUndoMechanism()
{
    for (auto && node : m_editorData->mindMapData()->graph().getNodes()) {
        connectNodeToUndoMechanism(node);
    }

    for (auto && edge : m_editorData->mindMapData()->graph().getEdges()) {
        connectEdgeToUndoMechanism(edge);
    }
}

void Mediator::connectGraphToImageManager()
{
    for (auto && node : m_editorData->mindMapData()->graph().getNodes()) {
        connectNodeToImageManager(node);
    }
}

void Mediator::connectSelectedNodes()
{
    L().debug() << "Connecting selected nodes: " << m_editorData->selectionGroupSize();
    if (areSelectedNodesConnectable()) {
        saveUndoPoint();
        for (auto && edge : m_editorData->connectSelectedNodes()) {
            connectEdgeToUndoMechanism(edge);
        }
        addExistingGraphToScene();
        updateNodeConnectionActions();
    }
}

void Mediator::disconnectSelectedNodes()
{
    L().debug() << "Disconnecting selected nodes: " << m_editorData->selectionGroupSize();
    if (areSelectedNodesDisconnectable()) {
        saveUndoPoint();
        m_editorData->disconnectSelectedNodes();
        updateNodeConnectionActions();
    }
}

NodePtr Mediator::createAndAddNode(int sourceNodeIndex, QPointF pos)
{
    const auto node0 = getNodeByIndex(sourceNodeIndex);
    const auto node1 = m_mainWindow.copyOnDragEnabled() ? m_editorData->copyNodeAt(*node0, pos) : m_editorData->addNodeAt(pos);
    connectNodeToUndoMechanism(node1);
    connectNodeToImageManager(node1);
    L().debug() << "Created a new node at (" << pos.x() << "," << pos.y() << ")";

    // Add edge from the parent node.
    connectEdgeToUndoMechanism(m_editorData->addEdge(std::make_shared<Edge>(*node0, *node1)));
    L().debug() << "Created a new edge " << node0->index() << " -> " << node1->index();

    addExistingGraphToScene();

    node1->setTextInputActive(true);

    return node1;
}

NodePtr Mediator::createAndAddNode(QPointF pos)
{
    const auto node1 = m_editorData->addNodeAt(pos);
    assert(node1);
    connectNodeToUndoMechanism(node1);
    connectNodeToImageManager(node1);
    L().debug() << "Created a new node at (" << pos.x() << "," << pos.y() << ")";

    addExistingGraphToScene();

    QTimer::singleShot(0, [node1]() { // Needed due to the context menu
        node1->setTextInputActive(true);
    });

    return node1;
}

NodePtr Mediator::pasteNodeAt(Node & source, QPointF pos)
{
    const auto copiedNode = m_editorData->copyNodeAt(source, pos);
    assert(copiedNode);
    connectNodeToUndoMechanism(copiedNode);
    connectNodeToImageManager(copiedNode);
    L().debug() << "Pasted node at (" << pos.x() << "," << pos.y() << ")";
    return copiedNode;
}

MouseAction & Mediator::mouseAction()
{
    return m_editorData->mouseAction();
}

void Mediator::deleteEdge(Edge & edge)
{
    m_editorData->deleteEdge(edge);
}

void Mediator::enableUndo(bool enable)
{
    m_mainWindow.enableUndo(enable);
}

void Mediator::enableRedo(bool enable)
{
    m_mainWindow.enableRedo(enable);
}

void Mediator::exportToPng(QString filename, QSize size, bool transparentBackground)
{
    zoomForExport();

    L().info() << "Exporting a PNG image of size (" << size.width() << "x" << size.height() << ") to " << filename.toStdString();
    const auto image = m_editorScene->toImage(size, m_editorData->backgroundColor(), transparentBackground);

    emit pngExportFinished(image.save(filename));
}

void Mediator::exportToSvg(QString filename)
{
    zoomForExport();

    L().info() << "Exporting an SVG image to " << filename.toStdString();
    const QFileInfo fi(filename);
    m_editorScene->toSvg(filename, fi.fileName());

    emit svgExportFinished(true);
}

QString Mediator::fileName() const
{
    return m_editorData->fileName();
}

NodePtr Mediator::getNodeByIndex(int index)
{
    return m_editorData->getNodeByIndex(index);
}

bool Mediator::hasNodes() const
{
    return m_editorData->mindMapData() && m_editorData->mindMapData()->graph().numNodes();
}

void Mediator::initializeNewMindMap()
{
    L().debug() << "Initializing a new mind map";

    assert(m_editorData);

    m_editorScene = std::make_unique<EditorScene>();
    m_editorData->clearImages();
    m_editorData->setMindMapData(std::make_shared<MindMapData>());

    initializeView();

    const auto node = m_editorData->addNodeAt(QPointF(0, 0));
    connectNodeToUndoMechanism(node);
    connectNodeToImageManager(node);

    addExistingGraphToScene();

    QTimer::singleShot(0, this, &Mediator::zoomToFit);

    m_mainWindow.initializeNewMindMap();
}

void Mediator::initiateNewNodeDrag(NodeHandle & nodeHandle)
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

void Mediator::initiateNodeDrag(Node & node)
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

void Mediator::initializeView()
{
    L().debug() << "Initializing view";

    // Set scene to the view
    m_editorView->setScene(m_editorScene.get());
    m_editorView->resetDummyDragItems();

    assert(m_editorData);

    m_editorView->setBackgroundBrush(QBrush(m_editorData->backgroundColor()));

    m_mainWindow.setCentralWidget(m_editorView);
    m_mainWindow.setContentsMargins(0, 0, 0, 0);
}

bool Mediator::areDirectlyConnected(const Node & node1, const Node & node2) const
{
    auto && graph = m_editorData->mindMapData()->graph();
    const auto node1Ptr = graph.getNode(node1.index());
    const auto node2Ptr = graph.getNode(node2.index());
    return graph.areDirectlyConnected(node1Ptr, node2Ptr);
}

bool Mediator::areSelectedNodesConnectable() const
{
    return m_editorData->areSelectedNodesConnectable();
}

bool Mediator::areSelectedNodesDisconnectable() const
{
    return m_editorData->areSelectedNodesDisconnectable();
}

bool Mediator::isLeafNode(Node & node)
{
    auto && graph = m_editorData->mindMapData()->graph();
    return graph.getEdgesFromNode(graph.getNode(node.index())).size() + graph.getEdgesToNode(graph.getNode(node.index())).size() <= 1;
}

bool Mediator::isInBetween(Node & node)
{
    auto && graph = m_editorData->mindMapData()->graph();
    return graph.getEdgesFromNode(graph.getNode(node.index())).size() + graph.getEdgesToNode(graph.getNode(node.index())).size() == 2;
}

bool Mediator::isInSelectionGroup(Node & node)
{
    return m_editorData->isInSelectionGroup(node);
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

void Mediator::moveSelectionGroup(Node & reference, QPointF location)
{
    m_editorData->moveSelectionGroup(reference, location);
}

MindMapDataPtr Mediator::mindMapData() const
{
    return m_editorData->mindMapData();
}

size_t Mediator::nodeCount() const
{
    return m_editorData->mindMapData() ? m_editorData->mindMapData()->graph().numNodes() : 0;
}

bool Mediator::nodeHasImageAttached() const
{
    return m_editorData->nodeHasImageAttached();
}

void Mediator::paste()
{
    if (m_editorData->copyStackSize()) {
        saveUndoPoint();
        for (auto && node : m_editorData->copiedNodes()) {
            pasteNodeAt(*node, m_editorView->grid().snapToGrid(mouseAction().mappedPos() - m_editorData->copyReferencePoint() + node->pos()));
        }
        addExistingGraphToScene();
    }
}

void Mediator::performNodeAction(const NodeAction & action)
{
    juzzlin::L().debug() << "Handling NodeAction: " << static_cast<int>(action.type);

    switch (action.type) {
    case NodeAction::Type::None:
        break;
    case NodeAction::Type::AttachImage: {
        const Image image { action.image, action.fileName.toStdString() };
        const auto id = m_editorData->mindMapData()->imageManager().addImage(image);
        if (m_editorData->selectionGroupSize()) {
            saveUndoPoint();
            m_editorData->setImageRefForSelectedNodes(id);
        }
    } break;
    case NodeAction::Type::ConnectSelected:
        connectSelectedNodes();
        break;
    case NodeAction::Type::Copy:
        m_editorData->copySelectedNodes();
        break;
    case NodeAction::Type::Delete:
        saveUndoPoint();
        m_editorView->resetDummyDragItems();
        m_editorData->deleteSelectedNodes();
        break;
    case NodeAction::Type::DisconnectSelected:
        disconnectSelectedNodes();
        break;
    case NodeAction::Type::Paste:
        paste();
        break;
    case NodeAction::Type::RemoveAttachedImage:
        saveUndoPoint();
        m_editorData->removeImageRefsOfSelectedNodes();
        break;
    case NodeAction::Type::SetNodeColor:
        saveUndoPoint();
        m_editorData->setColorForSelectedNodes(action.color);
        if (m_editorData->selectionGroupSize() == 1) {
            m_editorData->clearSelectionGroup();
        }
        break;
    case NodeAction::Type::SetTextColor:
        saveUndoPoint();
        m_editorData->setTextColorForSelectedNodes(action.color);
        if (m_editorData->selectionGroupSize() == 1) {
            m_editorData->clearSelectionGroup();
        }
        break;
    }
}

bool Mediator::openMindMap(QString fileName)
{
    assert(m_editorData);

    try {
        m_editorScene = std::make_unique<EditorScene>();
        m_editorData->loadMindMapData(fileName);
        initializeView();
        addExistingGraphToScene();
        connectGraphToUndoMechanism();
        connectGraphToImageManager();
        zoomToFit();
    } catch (const FileException & e) {
        m_mainWindow.showErrorDialog(e.message());
        return false;
    } catch (const std::runtime_error & e) {
        m_mainWindow.showErrorDialog(e.what());
        return false;
    }

    return true;
}

void Mediator::redo()
{
    L().debug() << "Undo..";

    m_editorView->resetDummyDragItems();
    m_editorData->redo();

    setupMindMapAfterUndoOrRedo();
}

void Mediator::removeItem(QGraphicsItem & item)
{
    m_editorScene->removeItem(&item);
}

void Mediator::toggleNodeInSelectionGroup(Node & node, bool updateNodeConnectionActions)
{
    m_editorData->toggleNodeInSelectionGroup(node);
    if (updateNodeConnectionActions) {
        this->updateNodeConnectionActions();
    }
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

QSize Mediator::sceneRectSize() const
{
    return m_editorScene->sceneRect().size().toSize();
}

Edge * Mediator::selectedEdge() const
{
    return m_editorData->selectedEdge();
}

Node * Mediator::selectedNode() const
{
    return m_editorData->selectedNode();
}

size_t Mediator::selectionGroupSize() const
{
    return m_editorData->selectionGroupSize();
}

void Mediator::setBackgroundColor(QColor color)
{
    if (m_editorData->mindMapData()->backgroundColor() != color) {
        saveUndoPoint();
        m_editorData->mindMapData()->setBackgroundColor(color);
        m_editorView->setBackgroundBrush(QBrush(color));
    }
}

void Mediator::setCornerRadius(int value)
{
    // Break loop with the spinbox
    if (m_editorData->mindMapData()->cornerRadius() != value) {
        saveUndoPoint();
        m_editorData->mindMapData()->setCornerRadius(value);
        m_editorView->setCornerRadius(m_editorData->mindMapData()->cornerRadius());
    }
}

void Mediator::setEdgeColor(QColor color)
{
    if (m_editorData->mindMapData()->edgeColor() != color) {
        saveUndoPoint();
        m_editorData->mindMapData()->setEdgeColor(color);
        m_editorView->setEdgeColor(color);
    }
}

void Mediator::setGridColor(QColor color)
{
    if (m_editorData->mindMapData()->gridColor() != color) {
        saveUndoPoint();
        m_editorData->mindMapData()->setGridColor(color);
        m_editorView->setGridColor(color);
        m_editorView->scene()->update();
    }
}

void Mediator::setEdgeWidth(double value)
{
    // Break loop with the spinbox
    if (!qFuzzyCompare(m_editorData->mindMapData()->edgeWidth(), value)) {
        saveUndoPoint();
        m_editorData->mindMapData()->setEdgeWidth(value);
        m_editorView->setEdgeWidth(m_editorData->mindMapData()->edgeWidth());
    }
}

void Mediator::setEditorData(std::shared_ptr<EditorData> editorData)
{
    m_editorData = editorData;

    connect(m_editorData.get(), &EditorData::undoEnabled, this, &Mediator::enableUndo);
    connect(m_editorData.get(), &EditorData::redoEnabled, this, &Mediator::enableRedo);
}

void Mediator::setEditorView(EditorView & editorView)
{
    m_editorView = &editorView;

    m_editorView->setParent(&m_mainWindow);

    connect(m_editorView, &EditorView::newNodeRequested, [=](QPointF position) {
        saveUndoPoint();
        createAndAddNode(position);
    });
}

void Mediator::setRectagleSelection(QRectF rect)
{
    const auto items = m_editorScene->items(rect, Defaults::instance().selectNodeGroupByIntersection() ? Qt::IntersectsItemShape : Qt::ContainsItemShape);
    for (auto && item : items) {
        if (const auto node = dynamic_cast<Node *>(item)) {
            toggleNodeInSelectionGroup(*node, false);
        }
    }
    updateNodeConnectionActions();
}

void Mediator::setSelectedEdge(Edge * edge)
{
    L().debug() << __func__ << "(): " << reinterpret_cast<uint64_t>(edge);

    if (m_editorData->selectedEdge()) {
        m_editorData->selectedEdge()->setSelected(false);
    }

    if (edge) {
        edge->setSelected(true);
    }

    m_editorData->setSelectedEdge(edge);
}

void Mediator::setTextSize(int textSize)
{
    // Break loop with the spinbox
    if (m_editorData->mindMapData()->textSize() != textSize) {
        saveUndoPoint();
        m_editorData->mindMapData()->setTextSize(textSize);
    }
}

void Mediator::setupMindMapAfterUndoOrRedo()
{
    const auto oldSceneRect = m_editorScene->sceneRect();
    const auto oldCenter = m_editorView->mapToScene(m_editorView->viewport()->rect()).boundingRect().center();

    m_editorScene = std::make_unique<EditorScene>();
    m_editorView->setScene(m_editorScene.get());
    m_editorView->setBackgroundBrush(QBrush(m_editorData->backgroundColor()));

    addExistingGraphToScene();

    connectGraphToUndoMechanism();
    connectGraphToImageManager();

    m_editorScene->setSceneRect(oldSceneRect);
    m_editorView->centerOn(oldCenter);
}

void Mediator::updateNodeConnectionActions()
{
    m_mainWindow.enableConnectSelectedNodesAction(areSelectedNodesConnectable());
    m_mainWindow.enableDisconnectSelectedNodesAction(areSelectedNodesDisconnectable());
}

void Mediator::undo()
{
    L().debug() << "Undo..";

    m_editorView->resetDummyDragItems();
    m_editorData->undo();

    setupMindMapAfterUndoOrRedo();
}

void Mediator::zoomIn()
{
    m_editorView->zoom(std::pow(Constants::View::ZOOM_SENSITIVITY, 2));
}

void Mediator::zoomOut()
{
    m_editorView->zoom(1.0 / std::pow(Constants::View::ZOOM_SENSITIVITY, 2));
}

QSize Mediator::zoomForExport()
{
    clearSelectedNode();
    clearSelectionGroup();
    m_editorScene->setSceneRect(m_editorScene->zoomToFit(true));
    return m_editorScene->sceneRect().size().toSize();
}

void Mediator::zoomToFit()
{
    if (hasNodes()) {
        m_editorView->zoomToFit(m_editorScene->zoomToFit());
    }
}

double Mediator::calculateNodeOverlapScore(const Node & node1, const Node & node2) const
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

void Mediator::clearSelectedNode()
{
    for (auto && node : m_editorData->mindMapData()->graph().getNodes()) {
        node->setSelected(false);
    }
}

NodePtr Mediator::getBestOverlapNode(const Node & source)
{
    NodePtr bestNode;
    double bestScore = 0;
    const double minThreshold = 0.25;
    for (auto && node : m_editorData->mindMapData()->graph().getNodes()) {
        if (node->index() != source.index() && node->index() != mouseAction().sourceNode()->index() && !areDirectlyConnected(*node, *mouseAction().sourceNode())) {
            const auto score = calculateNodeOverlapScore(source, *node);
            if (score > minThreshold && score > bestScore) {
                bestNode = node;
                bestScore = score;
            }
        }
    }

    return bestNode;
}

Mediator::~Mediator() = default;
