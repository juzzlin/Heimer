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

#include "mouse_action.hpp"
#include "editor_data.hpp"
#include "editor_scene.hpp"
#include "editor_view.hpp"
#include "main_window.hpp"

#include "simple_logger.hpp"

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QSizePolicy>

#include <cassert>

using std::dynamic_pointer_cast;
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
    for (auto && node : m_editorData->mindMapData()->graph().getNodes())
    {
        if (dynamic_pointer_cast<QGraphicsItem>(node)->scene() != m_editorScene.get())
        {
            auto graphicsNode = dynamic_pointer_cast<Node>(node);
            addItem(*graphicsNode);
            graphicsNode->setCornerRadius(m_editorData->mindMapData()->cornerRadius());
            graphicsNode->setTextSize(m_editorData->mindMapData()->textSize());
            L().debug() << "Added existing node " << node->index() << " to scene";
        }
    }

    for (auto && edge : m_editorData->mindMapData()->graph().getEdges())
    {
        auto node0 = dynamic_pointer_cast<Node>(getNodeByIndex(edge->sourceNodeBase().index()));
        auto node1 = dynamic_pointer_cast<Node>(getNodeByIndex(edge->targetNodeBase().index()));

        if (!m_editorScene->hasEdge(*node0, *node1))
        {
            auto graphicsEdge = dynamic_pointer_cast<Edge>(edge);
            assert(graphicsEdge);
            addItem(*graphicsEdge);
            graphicsEdge->setColor(m_editorData->mindMapData()->edgeColor());
            graphicsEdge->setWidth(m_editorData->mindMapData()->edgeWidth());
            graphicsEdge->setTextSize(m_editorData->mindMapData()->textSize());
            node0->addGraphicsEdge(*graphicsEdge);
            node1->addGraphicsEdge(*graphicsEdge);
            graphicsEdge->updateLine();
            L().debug() << "Added existing edge " << node0->index() << " -> " << node1->index() << " to scene";
        }
    }

    m_mainWindow.setCornerRadius(m_editorData->mindMapData()->cornerRadius());
    m_mainWindow.setEdgeWidth(m_editorData->mindMapData()->edgeWidth());
    m_mainWindow.setTextSize(m_editorData->mindMapData()->textSize());

    m_editorView->setCornerRadius(m_editorData->mindMapData()->cornerRadius());
    m_editorView->setEdgeColor(m_editorData->mindMapData()->edgeColor());
    m_editorView->setEdgeWidth(m_editorData->mindMapData()->edgeWidth());
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
}

void Mediator::clearSelectionGroup()
{
    m_editorData->clearSelectionGroup();
}

bool Mediator::canBeSaved() const
{
    return !m_editorData->fileName().isEmpty();
}

void Mediator::clearScene()
{
    m_editorScene->initialize();
}

void Mediator::connectEdgeToUndoMechanism(EdgePtr edge)
{
    connect(edge.get(), &Edge::undoPointRequested, this, &Mediator::saveUndoPoint);
}

void Mediator::connectNodeToUndoMechanism(NodePtr node)
{
    connect(node.get(), &Node::undoPointRequested, this, &Mediator::saveUndoPoint);
}

void Mediator::connectGraphToUndoMechanism()
{
    for (auto && node : m_editorData->mindMapData()->graph().getNodes())
    {
        connectNodeToUndoMechanism(std::dynamic_pointer_cast<Node>(node));
    }

    for (auto && edge : m_editorData->mindMapData()->graph().getEdges())
    {
        connectEdgeToUndoMechanism(std::dynamic_pointer_cast<Edge>(edge));
    }
}

NodeBasePtr Mediator::createAndAddNode(int sourceNodeIndex, QPointF pos)
{
    auto node1 = m_editorData->addNodeAt(pos);
    assert(node1);
    connectNodeToUndoMechanism(node1);
    L().debug() << "Created a new node at (" << pos.x() << "," << pos.y() << ")";

    auto node0 = dynamic_pointer_cast<Node>(getNodeByIndex(sourceNodeIndex));
    assert(node0);

    // Add edge from the parent node.
    connectEdgeToUndoMechanism(m_editorData->addEdge(std::make_shared<Edge>(*node0, *node1)));
    L().debug() << "Created a new edge " << node0->index() << " -> " << node1->index();

    addExistingGraphToScene();

    node1->setTextInputActive();

    return std::move(node1); // Fix a static analyzer warning: avoid copy on older compilers
}

NodeBasePtr Mediator::createAndAddNode(QPointF pos)
{
    auto node1 = m_editorData->addNodeAt(pos);
    assert(node1);
    connectNodeToUndoMechanism(node1);
    L().debug() << "Created a new node at (" << pos.x() << "," << pos.y() << ")";

    addExistingGraphToScene();

    QTimer::singleShot(0, [node1] () { // Needed due to the context menu
        node1->setTextInputActive();
    });

    return std::move(node1); // Fix a static analyzer warning: avoid copy on older compilers
}

MouseAction & Mediator::mouseAction()
{
    return m_editorData->mouseAction();
}

void Mediator::deleteEdge(Edge & edge)
{
    m_editorData->deleteEdge(edge);
}

void Mediator::deleteNode(Node & node)
{
    m_editorView->resetDummyDragItems();
    m_editorData->deleteNode(node);
}

void Mediator::enableUndo(bool enable)
{
    m_mainWindow.enableUndo(enable);
}

void Mediator::exportToPNG(QString filename, QSize size, bool transparentBackground)
{
    zoomForExport();

    L().info() << "Exporting a PNG image of size (" << size.width() << "x" << size.height() << ") to " << filename.toStdString();

    QImage image(size, QImage::Format_ARGB32);
    image.fill(transparentBackground ? Qt::transparent : m_editorData->backgroundColor());

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);
    m_editorScene->render(&painter);

    emit exportFinished(image.save(filename));
}

QString Mediator::fileName() const
{
    return m_editorData->fileName();
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
    L().debug() << "Initializing a new mind map";

    assert(m_editorData);

    m_editorData->setMindMapData(std::make_shared<MindMapData>());

    m_editorScene->initialize();

    initializeView();

    connectNodeToUndoMechanism(m_editorData->addNodeAt(QPointF(0, 0)));

    addExistingGraphToScene();

    QTimer::singleShot(0, this, &Mediator::zoomToFit);

    m_mainWindow.initializeNewMindMap();
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
    auto node1Ptr = graph.getNode(node1.index());
    auto node2Ptr = graph.getNode(node2.index());
    return graph.areDirectlyConnected(node1Ptr, node2Ptr);
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

int Mediator::nodeCount() const
{
    return m_editorData->mindMapData() ? m_editorData->mindMapData()->graph().numNodes() : 0;
}

bool Mediator::openMindMap(QString fileName)
{
    assert(m_editorData);

    try
    {
        m_editorScene->initialize();

        m_editorData->loadMindMapData(fileName);

        initializeView();

        addExistingGraphToScene();

        connectGraphToUndoMechanism();

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
    L().debug() << "Undo..";

    m_editorView->resetDummyDragItems();
    m_editorData->redo();
}

void Mediator::removeItem(QGraphicsItem & item)
{
    m_editorScene->removeItem(&item);
}

void Mediator::toggleNodeInSelectionGroup(Node & node)
{
    m_editorData->toggleNodeInSelectionGroup(node);
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
    L().debug() << "Saving undo point..";

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
    if (m_editorData->mindMapData()->backgroundColor() != color)
    {
        saveUndoPoint();
        m_editorData->mindMapData()->setBackgroundColor(color);
        m_editorView->setBackgroundBrush(QBrush(color));
    }
}

void Mediator::setCornerRadius(int value)
{
    // Break loop with the spinbox
    if (m_editorData->mindMapData()->cornerRadius() != value)
    {
        saveUndoPoint();
        m_editorData->mindMapData()->setCornerRadius(value);
        m_editorView->setCornerRadius(m_editorData->mindMapData()->cornerRadius());
    }
}

void Mediator::setEdgeColor(QColor color)
{
    if (m_editorData->mindMapData()->edgeColor() != color)
    {
        saveUndoPoint();
        m_editorData->mindMapData()->setEdgeColor(color);
        m_editorView->setEdgeColor(color);
    }
}

void Mediator::setEdgeWidth(double value)
{
    // Break loop with the spinbox
    if (!qFuzzyCompare(m_editorData->mindMapData()->edgeWidth(), value))
    {
        saveUndoPoint();
        m_editorData->mindMapData()->setEdgeWidth(value);
        m_editorView->setEdgeWidth(m_editorData->mindMapData()->edgeWidth());
    }
}

void Mediator::setEditorData(std::shared_ptr<EditorData> editorData)
{
    m_editorData = editorData;

    connect(m_editorData.get(), &EditorData::sceneCleared, this, &Mediator::clearScene);
    connect(m_editorData.get(), &EditorData::undoEnabled, this, &Mediator::enableUndo);
}

void Mediator::setEditorScene(std::shared_ptr<EditorScene> editorScene)
{
    m_editorScene = editorScene;
}

void Mediator::setEditorView(EditorView & editorView)
{
    m_editorView = &editorView;

    m_editorView->setParent(&m_mainWindow);

    connect(m_editorView, &EditorView::newNodeRequested, [=] (QPointF position) {
        saveUndoPoint();
        createAndAddNode(position);
    });
}

void Mediator::setRectagleSelection(QRectF rect)
{
    clearSelectionGroup();

    const auto items = m_editorScene->items(rect, Qt::ContainsItemShape);
    for (auto && item : items) {
        if (auto node = dynamic_cast<Node *>(item)) {
            toggleNodeInSelectionGroup(*node);
        }
    }
}

void Mediator::setSelectedEdge(Edge * edge)
{
    L().debug() << __func__ << "(): " << reinterpret_cast<uint64_t>(edge);

    if (m_editorData->selectedEdge()) {
        m_editorData->selectedEdge()->setSelected(false);
    }

    if (edge)
    {
        edge->setSelected(true);
    }

    m_editorData->setSelectedEdge(edge);
}

void Mediator::setSelectedNode(Node * node)
{
    L().debug() << __func__ << "(): " << reinterpret_cast<uint64_t>(node);

    if (selectedNode()) {
        selectedNode()->setSelected(false);
    }

    if (node)
    {
        node->setSelected(true);
    }

    m_editorData->setSelectedNode(node);
}

void Mediator::setTextSize(int textSize)
{
    // Break loop with the spinbox
    if (m_editorData->mindMapData()->textSize() != textSize)
    {
        saveUndoPoint();
        m_editorData->mindMapData()->setTextSize(textSize);
    }
}

void Mediator::setupMindMapAfterUndoOrRedo()
{
    auto oldSceneRect = m_editorScene->sceneRect();
    auto oldCenter = m_editorView->mapToScene(m_editorView->viewport()->rect()).boundingRect().center();

    m_editorScene.reset(new EditorScene);
    m_editorScene->initialize();

    m_editorView->setScene(m_editorScene.get());
    m_editorView->setBackgroundBrush(QBrush(m_editorData->backgroundColor()));

    addExistingGraphToScene();

    connectGraphToUndoMechanism();

    m_editorScene->setSceneRect(oldSceneRect);
    m_editorView->centerOn(oldCenter);
}

void Mediator::undo()
{
    L().debug() << "Undo..";

    m_editorView->resetDummyDragItems();
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

QSize Mediator::zoomForExport()
{
    clearSelectedNode();
    clearSelectionGroup();
    m_editorScene->setSceneRect(m_editorScene->zoomToFit(true));
    return m_editorScene->sceneRect().size().toSize();
}

void Mediator::zoomToFit()
{
    if (hasNodes())
    {
        m_editorView->zoomToFit(m_editorScene->zoomToFit());
    }
}

double Mediator::calculateNodeOverlapScore(const Node & node1, const Node & node2) const
{
    if (&node1 == &node2)
    {
        return 0;
    }

    const auto rect1 = node1.boundingRect().translated(node1.pos());
    const auto rect2 = node2.boundingRect().translated(node2.pos());

    if (rect1.intersects(rect2))
    {
        auto combined = rect1;
        combined = combined.united(rect2);

        const auto biggestArea = std::max(rect1.width() * rect1.height(), rect2.height() * rect2.width());

        return biggestArea / combined.width() / combined.height();
    }

    return 0;
}

void Mediator::clearSelectedNode()
{
    for (auto && node : m_editorData->mindMapData()->graph().getNodes())
    {
        node->setSelected(false);
    }
}

NodePtr Mediator::getBestOverlapNode(const Node & source)
{
    NodePtr bestNode;
    double bestScore = 0;

    for (auto && nodeBase : m_editorData->mindMapData()->graph().getNodes())
    {
        if (const auto node = std::dynamic_pointer_cast<Node>(nodeBase))
        {
            if (node->index() != source.index() && node->index() != mouseAction().sourceNode()->index() &&
                !areDirectlyConnected(*node, *mouseAction().sourceNode()))
            {
                const auto score = calculateNodeOverlapScore(source, *node);
                if (score > 0.75 && score > bestScore)
                {
                    bestNode = node;
                    bestScore = score;
                }
            }
        }
    }

    return bestNode;
}

Mediator::~Mediator() = default;
