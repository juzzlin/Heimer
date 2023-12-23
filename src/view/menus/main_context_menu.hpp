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

#ifndef MAIN_CONTEXT_MENU_HPP
#define MAIN_CONTEXT_MENU_HPP

#include <map>
#include <vector>

#include <QMenu>

#include "../../application/state_machine.hpp"
#include "../../common/types.hpp"

class Grid;
class Node;

namespace Menus {

class MainContextMenu : public QMenu
{
    Q_OBJECT

public:
    enum class Mode
    {
        All,
        Background,
        Node
    };

    MainContextMenu(QWidget * parent, Grid & grid);

    void setMode(const Mode & mode);

signals:

    //! This is used mainly for actions that require a dialog to be opened.
    void actionTriggered(StateMachine::Action action);

    //! Request a new node at given position.
    void newNodeRequested(QPointF position);

private:
    QAction * m_colorMenuAction = nullptr;

    QAction * m_copyNodeAction = nullptr;

    QAction * m_pasteNodeAction = nullptr;

    QAction * m_removeImageAction = nullptr;

    NodeP m_selectedNode = nullptr;

    std::map<Mode, std::vector<QAction *>> m_mainContextMenuActions;
};

} // namespace Menus

#endif // MAIN_CONTEXT_MENU_HPP
