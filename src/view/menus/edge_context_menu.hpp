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

#ifndef EDGE_CONTEXT_MENU_H
#define EDGE_CONTEXT_MENU_H

#include <QMenu>

#include "../../common/types.hpp"

class Edge;
class ApplicationService;

class QAction;

namespace Menus {

class EdgeContextMenu : public QMenu
{
    Q_OBJECT

public:
    EdgeContextMenu(QWidget * parent);

protected:
    void changeEvent(QEvent * event) override;

private:
    void connectVisibilitySignals();

    void createActions();

    void initialize();

    void populateMenuWithActions();

    void retranslate();

    QAction * m_changeEdgeDirectionAction = nullptr;

    QAction * m_showEdgeArrowAction = nullptr;

    QAction * m_doubleArrowAction = nullptr;

    QAction * m_dashedLineAction = nullptr;

    QAction * m_deleteEdgeAction = nullptr;

    EdgeP m_selectedEdge = nullptr;
};

} // namespace Menus

#endif // EDGE_CONTEXT_MENU_H
