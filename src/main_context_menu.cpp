// This file is part of Heimer.
// Copyright (C) 2019 Jussi Lind <jussi.lind@iki.fi>
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

#include "main_context_menu.hpp"

#include "contrib/SimpleLogger/src/simple_logger.hpp"
#include "copy_paste.hpp"
#include "grid.hpp"
#include "mediator.hpp"
#include "mouse_action.hpp"
#include "node.hpp"

#include <QColorDialog>
#include <QShortcut>
#include <QTimer>

MainContextMenu::MainContextMenu(QWidget * parent, Mediator & mediator, Grid & grid, CopyPaste & copyPaste)
  : QMenu(parent)
  , m_copyNodeAction(new QAction(tr("Copy node"), this))
  , m_pasteNodeAction(new QAction(tr("Paste node"), this))
  , m_mediator(mediator)
  , m_copyPaste(copyPaste)
{
    connect(m_copyNodeAction, &QAction::triggered, [this] {
        juzzlin::L().debug() << "Copy node triggered";
        m_copyPaste.copy(*m_mediator.selectedNode());
    });
    m_mainContextMenuActions[Mode::All].push_back(m_copyNodeAction);

    connect(m_pasteNodeAction, &QAction::triggered, [this] {
        juzzlin::L().debug() << "Paste node triggered";
        if (!m_copyPaste.isEmpty()) {
            m_mediator.saveUndoPoint();
            m_copyPaste.paste();
        }
    });
    m_mainContextMenuActions[Mode::All].push_back(m_pasteNodeAction);

    const auto setBackgroundColorAction(new QAction(tr("Set background color"), this));
    connect(setBackgroundColorAction, &QAction::triggered, [this] {
        emit actionTriggered(StateMachine::Action::BackgroundColorChangeRequested);
    });
    m_mainContextMenuActions[Mode::Background].push_back(setBackgroundColorAction);

    const auto setEdgeColorAction(new QAction(tr("Set edge color"), this));
    connect(setEdgeColorAction, &QAction::triggered, [this] {
        emit actionTriggered(StateMachine::Action::EdgeColorChangeRequested);
    });
    m_mainContextMenuActions[Mode::Background].push_back(setEdgeColorAction);

    const auto setGridColorAction(new QAction(tr("Set grid color"), this));
    connect(setGridColorAction, &QAction::triggered, [this] {
        emit actionTriggered(StateMachine::Action::GridColorChangeRequested);
    });
    m_mainContextMenuActions[Mode::Background].push_back(setGridColorAction);

    const auto createNodeAction(new QAction(tr("Create floating node"), this));
    // Here we add a shortcut to the context menu action. However, the action cannot be triggered unless the context menu
    // is open. As a "solution" we create another shortcut and add it to the parent widget.
    const auto createNodeKeySequence = Qt::Key_F | Qt::SHIFT | Qt::CTRL;
    createNodeAction->setShortcut(createNodeKeySequence);
    const auto createNodeShortCut = new QShortcut({ createNodeKeySequence }, parent);
    connect(createNodeShortCut, &QShortcut::activated, [this, grid] {
        emit newNodeRequested(grid.snapToGrid(m_mediator.mouseAction().mappedPos()));
    });
    connect(createNodeAction, &QAction::triggered, [this, grid] {
        emit newNodeRequested(grid.snapToGrid(m_mediator.mouseAction().clickedScenePos()));
    });
    m_mainContextMenuActions[Mode::Background].push_back(createNodeAction);

    const auto setNodeColorAction(new QAction(tr("Set node color"), this));
    connect(setNodeColorAction, &QAction::triggered, [this] {
        const auto node = m_mediator.selectedNode();
        const auto color = QColorDialog::getColor(Qt::white, this);
        if (color.isValid()) {
            m_mediator.saveUndoPoint();
            node->setColor(color);
        }
    });
    m_mainContextMenuActions[Mode::Node].push_back(setNodeColorAction);

    const auto setNodeTextColorAction(new QAction(tr("Set text color"), this));
    connect(setNodeTextColorAction, &QAction::triggered, [this] {
        const auto node = m_mediator.selectedNode();
        const auto color = QColorDialog::getColor(Qt::white, this);
        if (color.isValid()) {
            m_mediator.saveUndoPoint();
            node->setTextColor(color);
        }
    });
    m_mainContextMenuActions[Mode::Node].push_back(setNodeTextColorAction);

    const auto deleteNodeAction(new QAction(tr("Delete node"), this));
    connect(deleteNodeAction, &QAction::triggered, [this] {
        m_mediator.setSelectedNode(nullptr);
        m_mediator.saveUndoPoint();
        // Use a separate variable and timer here because closing the menu will always nullify the selected edge
        QTimer::singleShot(0, [=] {
            m_mediator.deleteNode(*m_selectedNode);
        });
    });

    m_mainContextMenuActions[Mode::Node].push_back(deleteNodeAction);

    const auto attachImageAction(new QAction(tr("Attach image..."), this));
    connect(attachImageAction, &QAction::triggered, [this] {
        emit actionTriggered(StateMachine::Action::ImageAttachmentRequested, m_selectedNode);
    });

    m_mainContextMenuActions[Mode::Node].push_back(attachImageAction);

    m_removeImageAction = new QAction(tr("Remove attached image"), this);
    connect(m_removeImageAction, &QAction::triggered, [this] {
        m_mediator.saveUndoPoint();
        m_selectedNode->setImageRef(0);
    });

    m_mainContextMenuActions[Mode::Node].push_back(m_removeImageAction);

    // Populate the menu
    addAction(createNodeAction);
    addSeparator();
    addAction(m_copyNodeAction);
    addAction(m_pasteNodeAction);
    addSeparator();
    addAction(setBackgroundColorAction);
    addSeparator();
    addAction(setEdgeColorAction);
    addSeparator();
    addAction(setGridColorAction);
    addSeparator();
    addAction(setNodeColorAction);
    addSeparator();
    addAction(setNodeTextColorAction);
    addSeparator();
    addAction(deleteNodeAction);
    addSeparator();
    addAction(attachImageAction);
    addAction(m_removeImageAction);

    connect(this, &QMenu::aboutToShow, [=] {
        m_selectedNode = m_mediator.selectedNode();
        if (m_selectedNode) {
            m_removeImageAction->setEnabled(m_selectedNode->imageRef());
        }
    });

    connect(this, &QMenu::aboutToHide, [=] {
        QTimer::singleShot(0, [=] {
            m_mediator.setSelectedNode(nullptr);
        });
    });
}

void MainContextMenu::setMode(const Mode & mode)
{
    for (auto && modePair : m_mainContextMenuActions) {
        for (auto && action : modePair.second) {
            action->setVisible(modePair.first == mode || modePair.first == Mode::All);
        }
    }

    m_copyNodeAction->setEnabled(mode == Mode::Node);
    m_pasteNodeAction->setEnabled(!m_copyPaste.isEmpty());
}
