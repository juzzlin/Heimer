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

#include <QApplication>
#include <QGraphicsItem>
#include <QMouseEvent>
#include <QStatusBar>
#include <QString>
#include <QTransform>
#include <iostream>
#include "editorview.hpp"

#include "draganddropstore.hpp"
#include "mediator.hpp"
#include "mindmapdata.hpp"
#include "node.hpp"
#include "nodehandle.hpp"

#include <cassert>
#include <cstdlib>

EditorView::EditorView(Mediator & mediator)
    : m_mediator(mediator)
{
    createNodeContextMenuActions();

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void EditorView::createNodeContextMenuActions()
{
    const QString dummy1(QString(QWidget::tr("Set size..")));
    auto setSize = new QAction(dummy1, &m_targetNodeContextMenu);
    QObject::connect(setSize, &QAction::triggered, [this] () {
    });

    // Populate the menu
    m_targetNodeContextMenu.addAction(setSize);
}

void EditorView::handleMousePressEventOnNode(QMouseEvent & event, Node & node)
{
    if (event.button() == Qt::RightButton)
    {
        handleRightButtonClickOnNode(node);
    }
    else if (event.button() == Qt::LeftButton)
    {
        handleLeftButtonClickOnNode(node);
    }

    QWidget::mousePressEvent(&event);
}

void EditorView::handleMousePressEventOnNodeHandle(QMouseEvent & event, NodeHandle & nodeHandle)
{
    if (event.button() == Qt::LeftButton)
    {
        handleLeftButtonClickOnNodeHandle(nodeHandle);
    }

    QWidget::mousePressEvent(&event);
}

void EditorView::handleLeftButtonClickOnNode(Node & node)
{
    // User is initiating a drag'n'drop

    m_mediator.saveUndoPoint();

    node.setZValue(node.zValue() + 1);
    m_mediator.dadStore().setDragAndDropNode(&node);

    // Change cursor to the closed hand cursor.
    QApplication::setOverrideCursor(QCursor(Qt::ClosedHandCursor));
}

void EditorView::handleLeftButtonClickOnNodeHandle(NodeHandle & nodeHandle)
{
    // User is initiating a drag'n'drop

    m_mediator.saveUndoPoint();

    m_mediator.dadStore().setDragAndDropNodeHandle(&nodeHandle);

    // Change cursor to the closed hand cursor.
    QApplication::setOverrideCursor(QCursor(Qt::ClosedHandCursor));
}

void EditorView::handleRightButtonClickOnNode(Node & node)
{
    m_mediator.setSelectedNode(&node);

    openNodeContextMenu();
}

void EditorView::handleNodeDragRelease(QMouseEvent * event)
{
    if (auto node = m_mediator.dadStore().dragAndDropNode())
    {
        node->setLocation(mapToScene(event->pos()));
        node->setZValue(node->zValue() - 1);

        update();

        m_mediator.dadStore().clear();

        QApplication::restoreOverrideCursor();
    }
}

void EditorView::handleNodeHandleDragRelease(QMouseEvent *)
{
    if (auto handle = m_mediator.dadStore().dragAndDropNodeHandle())
    {
        auto parentItem = dynamic_cast<Node *>(handle->parentItem());
        assert(parentItem);

        m_mediator.createAndAddNode(parentItem->index(), m_mappedPos - handle->pos());

        hideDummyDragNode();

        m_mediator.dadStore().clear();

        QApplication::restoreOverrideCursor();
    }
}

void EditorView::hideDummyDragNode()
{
    if (m_dummyDragNode)
    {
        m_dummyDragNode->setVisible(false);
    }
}

void EditorView::keyPressEvent(QKeyEvent * event)
{
    if (!event->isAutoRepeat())
    {
        switch (event->key())
        {
        default:
            break;
        }
    }
}

void EditorView::mouseMoveEvent(QMouseEvent * event)
{
    m_mappedPos = mapToScene(event->pos());
    if (auto node = m_mediator.dadStore().dragAndDropNode())
    {
        node->setLocation(m_mappedPos);
    }
    else if (auto handle = m_mediator.dadStore().dragAndDropNodeHandle())
    {
        showDummyDragNode();

        m_dummyDragNode->setPos(m_mappedPos - handle->pos());
    }

    QGraphicsView::mouseMoveEvent(event);
}

void EditorView::mousePressEvent(QMouseEvent * event)
{
    m_clickedPos = event->pos();
    m_clickedScenePos = mapToScene(m_clickedPos);

    // Fetch all items at the location
    QList<QGraphicsItem *> items = scene()->items(
                m_clickedScenePos, Qt::IntersectsItemShape, Qt::DescendingOrder);

    if (items.size())
    {
        auto item = *items.begin();
        if (auto node = dynamic_cast<Node *>(item))
        {
            handleMousePressEventOnNode(*event, *node);
        }
        else if (auto node = dynamic_cast<NodeHandle *>(item))
        {
            handleMousePressEventOnNodeHandle(*event, *node);
        }
    }
}

void EditorView::mouseReleaseEvent(QMouseEvent * event)
{
    handleNodeDragRelease(event);

    handleNodeHandleDragRelease(event);

    QGraphicsView::mouseReleaseEvent(event);
}

void EditorView::openNodeContextMenu()
{
    const QPoint globalPos = mapToGlobal(m_clickedPos);
    m_targetNodeContextMenu.exec(globalPos);
}

void EditorView::showDummyDragNode()
{
    if (!m_dummyDragNode)
    {
        m_dummyDragNode = new Node;
        scene()->addItem(m_dummyDragNode);
    }

    m_dummyDragNode->setVisible(true);
    m_dummyDragNode->setOpacity(0.5f);
}

void EditorView::updateSceneRect()
{
    const QRectF newSceneRect(-1000, -1000, 2000, 2000);

    setSceneRect(newSceneRect);

    assert(scene());
    scene()->setSceneRect(newSceneRect);
}

void EditorView::updateScale(int value)
{
    qreal scale = static_cast<qreal>(value) / 100;

    QTransform transform;
    transform.scale(scale, scale);
    setTransform(transform);
}

void EditorView::wheelEvent(QWheelEvent * event)
{
    if (event->modifiers() & Qt::ControlModifier)
    {
        const int sensitivity = 10;
        if (event->delta() > 0)
        {
            m_scaleValue += sensitivity;
        }
        else if (m_scaleValue > sensitivity)
        {
            m_scaleValue -= sensitivity;
        }

        updateScale(m_scaleValue);
    }
    else
    {
        QGraphicsView::wheelEvent(event);
    }
}

EditorView::~EditorView()
{
    delete m_dummyDragNode;
}
