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
#include "../../../application/recent_files_manager.hpp"
#include "../../../application/service_container.hpp"
#include "../../../application/state_machine.hpp"
#include "../../../common/constants.hpp"
#include "../../../view/menus/recent_files_menu.hpp"

#include <QAction>

namespace Menus {

MainMenu::MainMenu()
  : m_saveAction(new QAction(tr("&Save"), this))
  , m_saveAsAction(new QAction(tr("&Save as") + Constants::Misc::THREE_DOTS, this))
{
    createFileMenu();
}

void MainMenu::setSaveActionEnabled(bool enable)
{
    m_saveAction->setEnabled(enable);
}

void MainMenu::setSaveAsActionEnabled(bool enable)
{
    m_saveAsAction->setEnabled(enable);
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
    const auto newAct = new QAction(tr("&New") + Constants::Misc::THREE_DOTS, this);
    newAct->setShortcut(QKeySequence(QKeySequence::New));
    fileMenu->addAction(newAct);
    connect(newAct, &QAction::triggered, this, [=] {
        emit actionTriggered(StateMachine::Action::NewSelected);
    });

    // Add "open"-action
    const auto openAct = new QAction(tr("&Open") + Constants::Misc::THREE_DOTS, this);
    openAct->setShortcut(QKeySequence(QKeySequence::Open));
    fileMenu->addAction(openAct);
    connect(openAct, &QAction::triggered, this, [=] {
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
    const auto settingsAct = new QAction(tr("Settings") + Constants::Misc::THREE_DOTS, this);
    connect(settingsAct, &QAction::triggered, this, &MainMenu::settingsDialogRequested);
    fileMenu->addAction(settingsAct);

    fileMenu->addSeparator();

    // Add "quit"-action
    const auto quitAct = new QAction(tr("&Quit"), this);
    quitAct->setShortcut(QKeySequence(QKeySequence::Quit));
    fileMenu->addAction(quitAct);
    connect(quitAct, &QAction::triggered, this, [=] {
        emit actionTriggered(StateMachine::Action::QuitSelected);
    });

    connect(fileMenu, &QMenu::aboutToShow, this, [=] {
        recentFilesMenuAction->setEnabled(SC::instance().recentFilesManager()->hasRecentFiles());
    });
}

} // namespace Menus
