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

#ifndef MAIN_MENU_HPP
#define MAIN_MENU_HPP

#include "../../application/state_machine.hpp"

#include <QMenuBar>
#include <QObject>

class QAction;
class QMenu;

namespace Menus {

class MainMenu : public QMenuBar
{
    Q_OBJECT

public:
    MainMenu();

    void initialize();

    bool isFullScreenActionChecked() const;

    void setConnectSelectedNodesActionEnabled(bool enable);

    void setDisconnectSelectedNodesActionEnabled(bool enable);

    void setSaveActionEnabled(bool enable);

    void setSaveAsActionEnabled(bool enable);

    void setUndoActionEnabled(bool enable);

    void setRedoActionEnabled(bool enable);

    void triggerFullScreenAction();

signals:
    void aboutDialogRequested();

    void aboutQtDialogRequested();

    void actionTriggered(StateMachine::Action action);

    void fullScreenChanged(bool fullScreen);

    void settingsDialogRequested();

    void zoomInRequested();

    void zoomOutRequested();

    void zoomToFitRequested();

    void whatsNewDialogRequested();

private:
    void addConnectSelectedNodesAction(QMenu & menu);

    void addDisconnectSelectedNodesAction(QMenu & menu);

    void addRedoAction(QMenu & menu);

    void addUndoAction(QMenu & menu);

    void createColorSubMenu(QMenu & editMenu);

    void createEditMenu();

    void createExportSubMenu(QMenu & fileMenu);

    void createFileMenu();

    void createHelpMenu();

    void createMirrorSubMenu(QMenu & editMenu);

    void createViewMenu();

    QAction * m_connectSelectedNodesAction = nullptr;

    QAction * m_disconnectSelectedNodesAction = nullptr;

    QAction * m_fullScreenAction = nullptr;

    QAction * m_undoAction = nullptr;

    QAction * m_redoAction = nullptr;

    QAction * m_saveAction = nullptr;

    QAction * m_saveAsAction = nullptr;
};

} // namespace Menus

#endif // MAIN_MENU_HPP
