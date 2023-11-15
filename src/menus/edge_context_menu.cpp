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

#include "../application_service.hpp"
#include "../scene_items/edge.hpp"

#include <cassert>

namespace Menus {

EdgeContextMenu::EdgeContextMenu(QWidget * parent, ApplicationService & applicationService)
  : QMenu(parent)
  , m_applicationService(applicationService)
{
    const auto changeEdgeDirectionAction(new QAction(tr("Change direction"), this));
    QObject::connect(changeEdgeDirectionAction, &QAction::triggered, this, [=] {
        m_applicationService.saveUndoPoint();
        m_applicationService.selectedEdge()->setReversed(!m_applicationService.selectedEdge()->reversed());
    });

    using SceneItems::EdgeModel;

    const auto showEdgeArrowAction(new QAction(tr("Show arrow"), this));
    QObject::connect(showEdgeArrowAction, &QAction::triggered, this, [=] {
        m_applicationService.saveUndoPoint();
        m_applicationService.selectedEdge()->setArrowMode(showEdgeArrowAction->isChecked() ? EdgeModel::ArrowMode::Single : EdgeModel::ArrowMode::Hidden);
    });
    showEdgeArrowAction->setCheckable(true);

    const auto doubleArrowAction(new QAction(tr("Double arrow"), this));
    QObject::connect(doubleArrowAction, &QAction::triggered, this, [=] {
        m_applicationService.saveUndoPoint();
        m_applicationService.selectedEdge()->setArrowMode(doubleArrowAction->isChecked() ? EdgeModel::ArrowMode::Double : EdgeModel::ArrowMode::Single);
    });
    doubleArrowAction->setCheckable(true);

    const auto dashedLineAction(new QAction(tr("Dashed line"), this));
    QObject::connect(dashedLineAction, &QAction::triggered, this, [=] {
        m_applicationService.saveUndoPoint();
        m_applicationService.selectedEdge()->setDashedLine(dashedLineAction->isChecked());
    });
    dashedLineAction->setCheckable(true);

    const auto deleteEdgeAction(new QAction(tr("Delete edge"), this));
    QObject::connect(deleteEdgeAction, &QAction::triggered, this, [=] {
        m_applicationService.setSelectedEdge(nullptr);
        m_applicationService.saveUndoPoint();
        // Use a separate variable and timer here because closing the menu will always nullify the selected edge
        QTimer::singleShot(0, this, [=] {
            m_applicationService.deleteEdge(*m_selectedEdge);
        });
    });

    // Populate the menu
    addAction(changeEdgeDirectionAction);
    addSeparator();
    addAction(showEdgeArrowAction);
    addSeparator();
    addAction(doubleArrowAction);
    addSeparator();
    addAction(dashedLineAction);
    addSeparator();
    addAction(deleteEdgeAction);

    // Set correct edge config when the menu opens.
    connect(this, &QMenu::aboutToShow, this, [=] {
        m_selectedEdge = m_applicationService.selectedEdge();
        assert(m_selectedEdge);
        changeEdgeDirectionAction->setEnabled(m_selectedEdge->arrowMode() != EdgeModel::ArrowMode::Double && m_selectedEdge->arrowMode() != EdgeModel::ArrowMode::Hidden);
        dashedLineAction->setChecked(m_selectedEdge->dashedLine());
        doubleArrowAction->setChecked(m_selectedEdge->arrowMode() == EdgeModel::ArrowMode::Double);
        showEdgeArrowAction->setChecked(m_selectedEdge->arrowMode() != EdgeModel::ArrowMode::Hidden);
    });

    // Always clear edge selection when the menu closes.
    connect(this, &QMenu::aboutToHide, [=] {
        QTimer::singleShot(0, this, [=] {
            m_applicationService.setSelectedEdge(nullptr);
        });
    });
}

} // namespace Menus
