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

#include "../../application/application_service.hpp"
#include "../../application/service_container.hpp"
#include "../edge_action.hpp"
#include "../grid.hpp"
#include "../mouse_action.hpp"
#include "../node_action.hpp"

#include "simple_logger.hpp"

#include <QShortcut>

namespace Menus {

static const auto TAG = "MainContextMenu";

MainContextMenu::MainContextMenu(QWidget * parent, const Grid & grid)
  : QMenu { parent }
  , m_grid { grid }
{
    addGlobalShortcutsToParent(*parent);

    initialize();
}

void MainContextMenu::addGlobalShortcutsToParent(QWidget & parent)
{
    m_copyNodeShortcut = new QShortcut { m_copyNodeSequence, &parent };

    m_pasteNodeShortcut = new QShortcut { m_pasteNodeSequence, &parent };

    m_createNodeShortcut = new QShortcut { m_createNodeSequence, &parent };

    m_deleteNodesAndEdgesShortCut = new QShortcut { m_deleteNodeSequence, &parent };
}

void MainContextMenu::initialize()
{
    m_mainContextMenuActions.clear();

    createActions(m_grid);

    populateWithActions();
}

void MainContextMenu::createActions(const Grid & grid)
{
    createCopyNodeAction();

    createPasteNodeAction();

    createNodeCreationActions(grid);

    createNodeDeletionActions();

    createImageActions();
}

void MainContextMenu::populateWithActions()
{
    addAction(m_createNodeAction);

    addSeparator();

    addAction(m_copyNodeAction);

    addAction(m_pasteNodeAction);

    addSeparator();

    m_colorMenuAction = addMenu(&createColorSubMenu());

    addAction(m_deleteNodeAction);

    addSeparator();

    addAction(m_attachImageAction);

    addAction(m_removeImageAction);
}

void MainContextMenu::changeEvent(QEvent * event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate();
    }
}

void MainContextMenu::retranslate()
{
    clear();

    initialize();
}

void MainContextMenu::createCopyNodeAction()
{
    // Here we add a shortcut to the context menu action. However, the action cannot be triggered unless the context menu
    // is open. As a "solution" we create another shortcut and add it to the parent widget.
    m_copyNodeAction = new QAction { tr("Copy node"), this };
    m_copyNodeAction->setShortcut(m_copyNodeSequence);

    connect(m_copyNodeShortcut, &QShortcut::activated, m_copyNodeAction, &QAction::trigger);
    connect(m_copyNodeAction, &QAction::triggered, this, [] {
        juzzlin::L(TAG).debug() << "Copy node triggered";
        SC::instance().applicationService()->performNodeAction({ NodeAction::Type::Copy });
    });
    m_mainContextMenuActions[Mode::All].push_back(m_copyNodeAction);
}

void MainContextMenu::createPasteNodeAction()
{
    // Here we add a shortcut to the context menu action. However, the action cannot be triggered unless the context menu
    // is open. As a "solution" we create another shortcut and add it to the parent widget.
    m_pasteNodeAction = new QAction { tr("Paste node"), this };
    m_pasteNodeAction->setShortcut(m_pasteNodeSequence);

    connect(m_pasteNodeShortcut, &QShortcut::activated, m_pasteNodeAction, &QAction::trigger);
    connect(m_pasteNodeAction, &QAction::triggered, this, [] {
        juzzlin::L(TAG).debug() << "Paste node triggered";
        SC::instance().applicationService()->performNodeAction({ NodeAction::Type::Paste });
    });
    m_mainContextMenuActions[Mode::All].push_back(m_pasteNodeAction);
}

void MainContextMenu::createNodeCreationActions(const Grid & grid)
{
    // Here we add a shortcut to the context menu action. However, the action cannot be triggered unless the context menu
    // is open. As a "solution" we create another shortcut and add it to the parent widget.
    m_createNodeAction = new QAction { tr("Create floating node"), this };
    m_createNodeAction->setShortcut(m_createNodeSequence);

    connect(m_createNodeShortcut, &QShortcut::activated, this, [this, grid] {
        emit newNodeRequested(grid.snapToGrid(SC::instance().applicationService()->mouseAction().mappedPos()));
    });
    connect(m_createNodeAction, &QAction::triggered, this, [this, grid] {
        emit newNodeRequested(grid.snapToGrid(SC::instance().applicationService()->mouseAction().clickedScenePos()));
    });
    m_mainContextMenuActions[Mode::Background].push_back(m_createNodeAction);
}

void MainContextMenu::createNodeDeletionActions()
{
    // Here we add a shortcut to the context menu action. However, the action cannot be triggered unless the context menu
    // is open. As a "solution" we create another shortcut and add it to the parent widget.
    m_deleteNodeAction = new QAction { tr("Delete node"), this };
    m_deleteNodeAction->setShortcut(m_deleteNodeSequence);

    connect(m_deleteNodesAndEdgesShortCut, &QShortcut::activated, this, [] {
        SC::instance().applicationService()->performEdgeAction({ EdgeAction::Type::Delete });
        SC::instance().applicationService()->performNodeAction({ NodeAction::Type::Delete });
    });
    connect(m_deleteNodeAction, &QAction::triggered, this, [] {
        SC::instance().applicationService()->performNodeAction({ NodeAction::Type::Delete });
    });

    m_mainContextMenuActions[Mode::Node].push_back(m_deleteNodeAction);
}

void MainContextMenu::createImageActions()
{
    m_attachImageAction = new QAction { tr("Attach image..."), this };
    connect(m_attachImageAction, &QAction::triggered, this, [this] {
        emit actionTriggered(StateMachine::Action::ImageAttachmentRequested);
    });

    m_mainContextMenuActions[Mode::Node].push_back(m_attachImageAction);

    m_removeImageAction = new QAction { tr("Remove attached image"), this };
    connect(m_removeImageAction, &QAction::triggered, this, [] {
        SC::instance().applicationService()->performNodeAction({ NodeAction::Type::RemoveAttachedImage });
    });

    m_mainContextMenuActions[Mode::Node].push_back(m_removeImageAction);
}

QMenu & MainContextMenu::createColorSubMenu()
{
    const auto setBackgroundColorAction { new QAction { tr("Set background color"), this } };
    connect(setBackgroundColorAction, &QAction::triggered, this, [this] {
        emit actionTriggered(StateMachine::Action::BackgroundColorChangeRequested);
    });
    m_mainContextMenuActions[Mode::Background].push_back(setBackgroundColorAction);

    const auto setEdgeColorAction { new QAction { tr("Set edge color"), this } };
    connect(setEdgeColorAction, &QAction::triggered, this, [this] {
        emit actionTriggered(StateMachine::Action::EdgeColorChangeRequested);
    });
    m_mainContextMenuActions[Mode::Background].push_back(setEdgeColorAction);

    const auto setGridColorAction { new QAction { tr("Set grid color"), this } };
    connect(setGridColorAction, &QAction::triggered, this, [this] {
        emit actionTriggered(StateMachine::Action::GridColorChangeRequested);
    });
    m_mainContextMenuActions[Mode::Background].push_back(setGridColorAction);

    const auto setNodeColorAction { new QAction { tr("Set node color"), this } };
    connect(setNodeColorAction, &QAction::triggered, this, [this] {
        emit actionTriggered(StateMachine::Action::NodeColorChangeRequested);
    });
    m_mainContextMenuActions[Mode::Node].push_back(setNodeColorAction);

    const auto setNodeTextColorAction { new QAction { tr("Set text color"), this } };
    connect(setNodeTextColorAction, &QAction::triggered, this, [this] {
        emit actionTriggered(StateMachine::Action::TextColorChangeRequested);
    });
    m_mainContextMenuActions[Mode::Node].push_back(setNodeTextColorAction);

    const auto colorMenu = new QMenu;
    m_colorMenuAction = addMenu(colorMenu);
    colorMenu->addAction(setBackgroundColorAction);
    addSeparator();
    colorMenu->addAction(setEdgeColorAction);
    addSeparator();
    colorMenu->addAction(setGridColorAction);
    addSeparator();
    colorMenu->addAction(setNodeColorAction);
    addSeparator();
    colorMenu->addAction(setNodeTextColorAction);

    return *colorMenu;
}

void MainContextMenu::setMode(const Mode & mode)
{
    for (auto && modePair : m_mainContextMenuActions) {
        for (auto && action : modePair.second) {
            action->setVisible(modePair.first == mode || modePair.first == Mode::All);
        }
    }

    m_colorMenuAction->setText(mode == Mode::Node ? tr("Node &colors") : tr("General &colors"));

    m_copyNodeAction->setEnabled(SC::instance().applicationService()->nodeSelectionGroupSize());
    m_copyNodeAction->setText(SC::instance().applicationService()->nodeSelectionGroupSize() > 1 ? tr("Copy nodes") : tr("Copy node"));

    m_pasteNodeAction->setEnabled(SC::instance().applicationService()->copyStackSize());
    m_pasteNodeAction->setText(SC::instance().applicationService()->copyStackSize() > 1 ? tr("Paste nodes") : tr("Paste node"));

    m_removeImageAction->setEnabled(SC::instance().applicationService()->nodeHasImageAttached());
}

} // namespace Menus
