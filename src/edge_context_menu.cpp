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

#include "edge_context_menu.hpp"

#include "edge.hpp"
#include "mediator.hpp"

#include <cassert>

EdgeContextMenu::EdgeContextMenu(QWidget * parent, Mediator & mediator)
  : QMenu(parent)
  , m_mediator(mediator)
{
    auto changeEdgeDirectionAction(new QAction(tr("Change direction"), this));
    QObject::connect(changeEdgeDirectionAction, &QAction::triggered, [=] {
        auto edge = m_mediator.selectedEdge();
        m_mediator.saveUndoPoint();
        edge->setReversed(!edge->reversed());
    });

    auto hideEdgeArrowAction(new QAction(tr("Hide arrow"), this));
    QObject::connect(hideEdgeArrowAction, &QAction::triggered, [=] {
        assert(m_mediator.selectedEdge());
        m_mediator.saveUndoPoint();
        m_mediator.selectedEdge()->setArrowMode(Edge::ArrowMode::Hidden);
    });

    auto singleArrowAction(new QAction(tr("Single arrow"), this));
    QObject::connect(singleArrowAction, &QAction::triggered, [=] {
        assert(m_mediator.selectedEdge());
        m_mediator.saveUndoPoint();
        m_mediator.selectedEdge()->setArrowMode(Edge::ArrowMode::Single);
    });

    auto doubleArrowAction(new QAction(tr("Double arrow"), this));
    QObject::connect(doubleArrowAction, &QAction::triggered, [=] {
        assert(m_mediator.selectedEdge());
        m_mediator.saveUndoPoint();
        m_mediator.selectedEdge()->setArrowMode(Edge::ArrowMode::Double);
    });

    auto deleteEdgeAction(new QAction(tr("Delete edge"), this));
    QObject::connect(deleteEdgeAction, &QAction::triggered, [=] {
        m_mediator.setSelectedEdge(nullptr);
        m_mediator.saveUndoPoint();
        // Use a separate variable and timer here because closing the menu will always nullify the selected edge
        QTimer::singleShot(0, [=] {
            m_mediator.deleteEdge(*m_selectedEdge);
        });
    });

    // Populate the menu
    addAction(changeEdgeDirectionAction);
    addSeparator();
    addAction(hideEdgeArrowAction);
    addSeparator();
    addAction(singleArrowAction);
    addAction(doubleArrowAction);
    addSeparator();
    addAction(deleteEdgeAction);

    // Set selected edge when the menu opens.
    connect(this, &QMenu::aboutToShow, [=] {
        m_selectedEdge = m_mediator.selectedEdge();
        changeEdgeDirectionAction->setEnabled(m_selectedEdge->arrowMode() != Edge::ArrowMode::Double && m_selectedEdge->arrowMode() != Edge::ArrowMode::Hidden);
        doubleArrowAction->setEnabled(m_selectedEdge->arrowMode() != Edge::ArrowMode::Double);
        hideEdgeArrowAction->setEnabled(m_selectedEdge->arrowMode() != Edge::ArrowMode::Hidden);
        singleArrowAction->setEnabled(m_selectedEdge->arrowMode() != Edge::ArrowMode::Single);
    });

    // Always clear edge selection when the menu closes.
    connect(this, &QMenu::aboutToHide, [=] {
        QTimer::singleShot(0, [=] {
            m_mediator.setSelectedEdge(nullptr);
        });
    });
}
