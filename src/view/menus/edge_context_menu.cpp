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

#include "../../application/application_service.hpp"
#include "../../application/service_container.hpp"
#include "../edge_action.hpp"
#include "../scene_items/edge.hpp"

#include <cassert>

namespace Menus {

EdgeContextMenu::EdgeContextMenu(QWidget * parent)
  : QMenu(parent)
{
    const auto changeEdgeDirectionAction(new QAction(tr("Change direction"), this));
    QObject::connect(changeEdgeDirectionAction, &QAction::triggered, this, [=] {
        if (const auto selectedEdge = SC::instance().applicationService()->selectedEdge(); selectedEdge.has_value()) {
            SC::instance().applicationService()->saveUndoPoint();
            selectedEdge.value()->setReversed(!selectedEdge.value()->reversed());
        }
    });

    using SceneItems::EdgeModel;

    const auto showEdgeArrowAction(new QAction(tr("Show arrow"), this));
    QObject::connect(showEdgeArrowAction, &QAction::triggered, this, [=] {
        if (const auto selectedEdge = SC::instance().applicationService()->selectedEdge(); selectedEdge.has_value()) {
            SC::instance().applicationService()->saveUndoPoint();
            selectedEdge.value()->setArrowMode(showEdgeArrowAction->isChecked() ? EdgeModel::ArrowMode::Single : EdgeModel::ArrowMode::Hidden);
        }
    });
    showEdgeArrowAction->setCheckable(true);

    const auto doubleArrowAction(new QAction(tr("Double arrow"), this));
    QObject::connect(doubleArrowAction, &QAction::triggered, this, [=] {
        if (const auto selectedEdge = SC::instance().applicationService()->selectedEdge(); selectedEdge.has_value()) {
            SC::instance().applicationService()->saveUndoPoint();
            selectedEdge.value()->setArrowMode(doubleArrowAction->isChecked() ? EdgeModel::ArrowMode::Double : EdgeModel::ArrowMode::Single);
        }
    });
    doubleArrowAction->setCheckable(true);

    const auto dashedLineAction(new QAction(tr("Dashed line"), this));
    QObject::connect(dashedLineAction, &QAction::triggered, this, [=] {
        if (const auto selectedEdge = SC::instance().applicationService()->selectedEdge(); selectedEdge.has_value()) {
            SC::instance().applicationService()->saveUndoPoint();
            selectedEdge.value()->setDashedLine(dashedLineAction->isChecked());
        }
    });
    dashedLineAction->setCheckable(true);

    const auto deleteEdgeAction(new QAction(tr("Delete edge"), this));
    QObject::connect(deleteEdgeAction, &QAction::triggered, this, [=] {
        if (SC::instance().applicationService()->edgeSelectionGroupSize()) {
            SC::instance().applicationService()->saveUndoPoint();
            SC::instance().applicationService()->performEdgeAction({ EdgeAction::Type::Delete });
        }
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
        if (const auto selectedEdge = SC::instance().applicationService()->selectedEdge(); selectedEdge.has_value()) {
            changeEdgeDirectionAction->setEnabled(selectedEdge.value()->arrowMode() != EdgeModel::ArrowMode::Double && selectedEdge.value()->arrowMode() != EdgeModel::ArrowMode::Hidden);
            dashedLineAction->setChecked(selectedEdge.value()->dashedLine());
            doubleArrowAction->setChecked(selectedEdge.value()->arrowMode() == EdgeModel::ArrowMode::Double);
            showEdgeArrowAction->setChecked(selectedEdge.value()->arrowMode() != EdgeModel::ArrowMode::Hidden);
        }
    });

    // Always clear edge selection when the menu closes.
    connect(this, &QMenu::aboutToHide, [=] {
        QTimer::singleShot(0, this, [=] {
            SC::instance().applicationService()->clearEdgeSelectionGroup();
        });
    });
}

} // namespace Menus
