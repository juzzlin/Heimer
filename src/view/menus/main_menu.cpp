// This file is part of Heimer.
// Copyright (C) 2024 Jussi Lind <jussi.lind@iki.fi>
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

#include "main_menu.hpp"

#include "../../../application/application_service.hpp"
#include "../../../application/control_strategy.hpp"
#include "../../../application/recent_files_manager.hpp"
#include "../../../application/service_container.hpp"
#include "../../../application/state_machine.hpp"
#include "../../../common/constants.hpp"
#include "../../../infra/settings.hpp"
#include "../../../view/menus/recent_files_menu.hpp"
#include "../../../view/node_action.hpp"

#include "simple_logger.hpp"

#include <QAction>

namespace Menus {

MainMenu::MainMenu()
  : m_connectSelectedNodesAction(new QAction(tr("Connect selected nodes"), this))
  , m_disconnectSelectedNodesAction(new QAction(tr("Disconnect selected nodes"), this))
  , m_undoAction(new QAction(tr("Undo"), this))
  , m_redoAction(new QAction(tr("Redo"), this))
  , m_saveAction(new QAction(tr("&Save"), this))
  , m_saveAsAction(new QAction(tr("&Save as") + Constants::Misc::threeDots(), this))

{
}

void MainMenu::initialize()
{
    createFileMenu();

    createEditMenu();

    createViewMenu();

    createHelpMenu();
}

void MainMenu::addConnectSelectedNodesAction(QMenu & menu)
{
    m_connectSelectedNodesAction->setShortcut(QKeySequence("Ctrl+Shift+C"));
    connect(m_connectSelectedNodesAction, &QAction::triggered, this, [] {
        juzzlin::L().debug() << "Connect selected triggered";
        SC::instance().applicationService()->performNodeAction({ NodeAction::Type::ConnectSelected });
    });
    menu.addAction(m_connectSelectedNodesAction);
    connect(&menu, &QMenu::aboutToShow, this, [=] {
        m_connectSelectedNodesAction->setEnabled(SC::instance().applicationService()->areSelectedNodesConnectable());
    });
}

void MainMenu::addDisconnectSelectedNodesAction(QMenu & menu)
{
    m_disconnectSelectedNodesAction->setShortcut(QKeySequence("Ctrl+Shift+D"));
    connect(m_disconnectSelectedNodesAction, &QAction::triggered, this, [] {
        juzzlin::L().debug() << "Disconnect selected triggered";
        SC::instance().applicationService()->performNodeAction({ NodeAction::Type::DisconnectSelected });
    });
    menu.addAction(m_disconnectSelectedNodesAction);
    connect(&menu, &QMenu::aboutToShow, this, [=] {
        m_disconnectSelectedNodesAction->setEnabled(SC::instance().applicationService()->areSelectedNodesDisconnectable());
    });
}

void MainMenu::setConnectSelectedNodesActionEnabled(bool enable)
{
    m_connectSelectedNodesAction->setEnabled(enable);
}

void MainMenu::setDisconnectSelectedNodesActionEnabled(bool enable)
{
    m_disconnectSelectedNodesAction->setEnabled(enable);
}

void MainMenu::setSaveActionEnabled(bool enable)
{
    m_saveAction->setEnabled(enable);
}

void MainMenu::setSaveAsActionEnabled(bool enable)
{
    m_saveAsAction->setEnabled(enable);
}

void MainMenu::setUndoActionEnabled(bool enable)
{
    m_undoAction->setEnabled(enable);
}

void MainMenu::setRedoActionEnabled(bool enable)
{
    m_redoAction->setEnabled(enable);
}

void MainMenu::triggerFullScreenAction()
{
    m_fullScreenAction->trigger();
}

bool MainMenu::isFullScreenActionChecked() const
{
    return m_fullScreenAction->isChecked();
}

void MainMenu::addRedoAction(QMenu & menu)
{
    m_redoAction->setShortcut(QKeySequence(QKeySequence::Redo));

    connect(m_redoAction, &QAction::triggered, SC::instance().applicationService().get(), &ApplicationService::redo);

    m_redoAction->setEnabled(false);

    menu.addAction(m_redoAction);
}

void MainMenu::addUndoAction(QMenu & menu)
{
    m_undoAction->setShortcut(QKeySequence(QKeySequence::Undo));

    connect(m_undoAction, &QAction::triggered, SC::instance().applicationService().get(), &ApplicationService::undo);

    m_undoAction->setEnabled(false);

    menu.addAction(m_undoAction);
}

void MainMenu::createColorSubMenu(QMenu & editMenu)
{
    const auto colorMenu = new QMenu;
    const auto colorMenuAction = editMenu.addMenu(colorMenu);
    colorMenuAction->setText(tr("General &colors"));

    const auto backgroundColorAction = new QAction(tr("Set background color") + Constants::Misc::threeDots(), this);
    backgroundColorAction->setShortcut(QKeySequence("Ctrl+B"));

    connect(backgroundColorAction, &QAction::triggered, this, [=] {
        emit actionTriggered(StateMachine::Action::BackgroundColorChangeRequested);
    });

    colorMenu->addAction(backgroundColorAction);

    colorMenu->addSeparator();

    const auto edgeColorAction = new QAction(tr("Set edge color") + Constants::Misc::threeDots(), this);
    edgeColorAction->setShortcut(QKeySequence("Ctrl+E"));

    connect(edgeColorAction, &QAction::triggered, this, [=] {
        emit actionTriggered(StateMachine::Action::EdgeColorChangeRequested);
    });

    colorMenu->addAction(edgeColorAction);

    colorMenu->addSeparator();

    const auto gridColorAction = new QAction(tr("Set grid color") + Constants::Misc::threeDots(), this);
    gridColorAction->setShortcut(QKeySequence("Ctrl+G"));

    connect(gridColorAction, &QAction::triggered, this, [=] {
        emit actionTriggered(StateMachine::Action::GridColorChangeRequested);
    });

    colorMenu->addAction(gridColorAction);
}

void MainMenu::createEditMenu()
{
    const auto editMenu = addMenu(tr("&Edit"));

    addUndoAction(*editMenu);

    addRedoAction(*editMenu);

    editMenu->addSeparator();

    addConnectSelectedNodesAction(*editMenu);

    addDisconnectSelectedNodesAction(*editMenu);

    editMenu->addSeparator();

    createColorSubMenu(*editMenu);

    editMenu->addSeparator();

    createMirrorSubMenu(*editMenu);

    editMenu->addSeparator();

    auto optimizeLayoutAction = new QAction(tr("Optimize layout") + Constants::Misc::threeDots(), this);
    optimizeLayoutAction->setShortcut(QKeySequence("Ctrl+Shift+O"));

    connect(optimizeLayoutAction, &QAction::triggered, this, [=] {
        emit actionTriggered(StateMachine::Action::LayoutOptimizationRequested);
    });

    editMenu->addAction(optimizeLayoutAction);
}

void MainMenu::createExportSubMenu(QMenu & fileMenu)
{
    const auto exportMenu = new QMenu;
    const auto exportMenuAction = fileMenu.addMenu(exportMenu);
    exportMenuAction->setText(tr("&Export"));

    // Add "export to PNG image"-action
    const auto exportToPngAction = new QAction(tr("&PNG"), this);
    exportMenu->addAction(exportToPngAction);
    connect(exportToPngAction, &QAction::triggered, this, [=] {
        emit actionTriggered(StateMachine::Action::PngExportSelected);
    });

    exportMenu->addSeparator();

    // Add "export to SVG file"-action
    const auto exportToSvgAction = new QAction(tr("&SVG"), this);
    exportMenu->addAction(exportToSvgAction);
    connect(exportToSvgAction, &QAction::triggered, this, [=] {
        emit actionTriggered(StateMachine::Action::SvgExportSelected);
    });

    connect(&fileMenu, &QMenu::aboutToShow, this, [=] {
        exportToPngAction->setEnabled(SC::instance().applicationService()->hasNodes());
        exportToSvgAction->setEnabled(SC::instance().applicationService()->hasNodes());
    });
}

void MainMenu::createFileMenu()
{
    const auto fileMenu = addMenu(tr("&File"));

    // Add "new"-action
    const auto newAction = new QAction(tr("&New") + Constants::Misc::threeDots(), this);
    newAction->setShortcut(QKeySequence(QKeySequence::New));
    fileMenu->addAction(newAction);
    connect(newAction, &QAction::triggered, this, [=] {
        emit actionTriggered(StateMachine::Action::NewSelected);
    });

    // Add "open"-action
    const auto openAction = new QAction(tr("&Open") + Constants::Misc::threeDots(), this);
    openAction->setShortcut(QKeySequence(QKeySequence::Open));
    fileMenu->addAction(openAction);
    connect(openAction, &QAction::triggered, this, [=] {
        emit actionTriggered(StateMachine::Action::OpenSelected);
    });

    // Add "Recent Files"-menu
    const auto recentFilesMenu = new Menus::RecentFilesMenu;
    const auto recentFilesMenuAction = fileMenu->addMenu(recentFilesMenu);
    recentFilesMenuAction->setText(tr("Recent &Files"));
    connect(recentFilesMenu, &Menus::RecentFilesMenu::fileSelected, this, [=] {
        emit actionTriggered(StateMachine::Action::RecentFileSelected);
    });

    fileMenu->addSeparator();

    // Add "save"-action
    m_saveAction->setShortcut(QKeySequence(QKeySequence::Save));
    m_saveAction->setEnabled(false);
    fileMenu->addAction(m_saveAction);
    connect(m_saveAction, &QAction::triggered, this, [=] {
        emit actionTriggered(StateMachine::Action::SaveSelected);
    });

    // Add "save as"-action
    m_saveAsAction->setShortcut(QKeySequence(QKeySequence::SaveAs));
    m_saveAsAction->setEnabled(false);
    fileMenu->addAction(m_saveAsAction);
    connect(m_saveAsAction, &QAction::triggered, this, [=] {
        emit actionTriggered(StateMachine::Action::SaveAsSelected);
    });

    fileMenu->addSeparator();

    createExportSubMenu(*fileMenu);

    fileMenu->addSeparator();

    // Add "settings"-action
    const auto settingsAction = new QAction(tr("Settings") + Constants::Misc::threeDots(), this);
    connect(settingsAction, &QAction::triggered, this, &MainMenu::settingsDialogRequested);
    fileMenu->addAction(settingsAction);

    fileMenu->addSeparator();

    // Add "quit"-action
    const auto quitAction = new QAction(tr("&Quit"), this);
    quitAction->setShortcut(QKeySequence(QKeySequence::Quit));
    fileMenu->addAction(quitAction);
    connect(quitAction, &QAction::triggered, this, [=] {
        emit actionTriggered(StateMachine::Action::QuitSelected);
    });

    connect(fileMenu, &QMenu::aboutToShow, this, [=] {
        recentFilesMenuAction->setEnabled(SC::instance().recentFilesManager()->hasRecentFiles());
    });
}

void MainMenu::createMirrorSubMenu(QMenu & editMenu)
{
    const auto mirrorMenu = new QMenu;
    auto mirrorAction = editMenu.addMenu(mirrorMenu);
    mirrorAction->setText(tr("&Mirror layout"));

    const auto mirrorHorizontallyAction = new QAction(tr("Horizontally"), this);
    mirrorHorizontallyAction->setShortcut(QKeySequence(SC::instance().controlStrategy()->mirrorLayoutHorizontallyShortcut()));
    mirrorMenu->addAction(mirrorHorizontallyAction);
    connect(mirrorHorizontallyAction, &QAction::triggered, this, [=] {
        SC::instance().applicationService()->performNodeAction(NodeAction::Type::MirrorLayoutHorizontally);
    });
    mirrorMenu->addSeparator();

    const auto mirrorVerticallyAction = new QAction(tr("Vertically"), this);
    mirrorVerticallyAction->setShortcut(QKeySequence(SC::instance().controlStrategy()->mirrorLayoutVerticallyShortcut()));
    mirrorMenu->addAction(mirrorVerticallyAction);
    connect(mirrorVerticallyAction, &QAction::triggered, this, [=] {
        SC::instance().applicationService()->performNodeAction(NodeAction::Type::MirrorLayoutVertically);
    });
}

void MainMenu::createHelpMenu()
{
    const auto helpMenu = addMenu(tr("&Help"));

    // Add "about"-action
    const auto aboutAction = new QAction(tr("&About"), this);
    helpMenu->addAction(aboutAction);
    connect(aboutAction, &QAction::triggered, this, &MainMenu::aboutDialogRequested);

    // Add "about Qt"-action
    const auto aboutQtAction = new QAction(tr("About &Qt"), this);
    helpMenu->addAction(aboutQtAction);
    connect(aboutQtAction, &QAction::triggered, this, &MainMenu::aboutQtDialogRequested);

    helpMenu->addSeparator();

    // Add "What's new"-action
    const auto whatsNewAction = new QAction(tr("What's New"), this);
    helpMenu->addAction(whatsNewAction);
    connect(whatsNewAction, &QAction::triggered, this, &MainMenu::whatsNewDialogRequested);
}

void MainMenu::createViewMenu()
{
    const auto viewMenu = addMenu(tr("&View"));

    // Add "fullScreen"-action
    m_fullScreenAction = new QAction(tr("Full Screen"), this);
    m_fullScreenAction->setCheckable(true);
    m_fullScreenAction->setChecked(false);
    m_fullScreenAction->setShortcut(QKeySequence("F11"));
    viewMenu->addAction(m_fullScreenAction);
    connect(m_fullScreenAction, &QAction::triggered, this, &MainMenu::fullScreenChanged);

    viewMenu->addSeparator();

    // Add "zoom in"-action
    const auto zoomInAction = new QAction(tr("Zoom In"), this);
    viewMenu->addAction(zoomInAction);
    zoomInAction->setShortcut(QKeySequence(QKeySequence::ZoomIn));
    connect(zoomInAction, &QAction::triggered, this, &MainMenu::zoomInRequested);

    // Add "zoom out"-action
    const auto zoomOutAction = new QAction(tr("Zoom Out"), this);
    viewMenu->addAction(zoomOutAction);
    connect(zoomOutAction, &QAction::triggered, this, &MainMenu::zoomOutRequested);
    zoomOutAction->setShortcut(QKeySequence(QKeySequence::ZoomOut));

    viewMenu->addSeparator();

    // Add "zoom to fit"-action
    const auto zoomToFitAction = new QAction(tr("&Zoom To Fit"), this);
    zoomToFitAction->setShortcut(QKeySequence("Ctrl+0"));
    viewMenu->addAction(zoomToFitAction);
    connect(zoomToFitAction, &QAction::triggered, this, &MainMenu::zoomToFitRequested);

    connect(viewMenu, &QMenu::aboutToShow, this, [=] {
        zoomToFitAction->setEnabled(SC::instance().applicationService()->hasNodes());
    });
}

} // namespace Menus
