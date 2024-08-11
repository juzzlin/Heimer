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

namespace Menus {

EdgeContextMenu::EdgeContextMenu(QWidget * parent)
  : QMenu(parent)
{
    initialize();
}

void EdgeContextMenu::initialize()
{
    createActions();

    populateMenuWithActions();

    connectVisibilitySignals();
}

void EdgeContextMenu::createActions()
{
    m_changeEdgeDirectionAction = new QAction(tr("Change direction"), this);
    QObject::connect(m_changeEdgeDirectionAction, &QAction::triggered, this, [=] {
        if (const auto selectedEdge = SC::instance().applicationService()->selectedEdge(); selectedEdge.has_value()) {
            SC::instance().applicationService()->saveUndoPoint();
            selectedEdge.value()->setReversed(!selectedEdge.value()->reversed());
        }
    });

    using SceneItems::EdgeModel;

    m_showEdgeArrowAction = new QAction(tr("Show arrow"), this);
    QObject::connect(m_showEdgeArrowAction, &QAction::triggered, this, [=] {
        if (const auto selectedEdge = SC::instance().applicationService()->selectedEdge(); selectedEdge.has_value()) {
            SC::instance().applicationService()->saveUndoPoint();
            selectedEdge.value()->setArrowMode(m_showEdgeArrowAction->isChecked() ? EdgeModel::ArrowMode::Single : EdgeModel::ArrowMode::Hidden);
        }
    });
    m_showEdgeArrowAction->setCheckable(true);

    m_doubleArrowAction = new QAction(tr("Double arrow"), this);
    QObject::connect(m_doubleArrowAction, &QAction::triggered, this, [=] {
        if (const auto selectedEdge = SC::instance().applicationService()->selectedEdge(); selectedEdge.has_value()) {
            SC::instance().applicationService()->saveUndoPoint();
            selectedEdge.value()->setArrowMode(m_doubleArrowAction->isChecked() ? EdgeModel::ArrowMode::Double : EdgeModel::ArrowMode::Single);
        }
    });
    m_doubleArrowAction->setCheckable(true);

    m_dashedLineAction = new QAction(tr("Dashed line"), this);
    QObject::connect(m_dashedLineAction, &QAction::triggered, this, [=] {
        if (const auto selectedEdge = SC::instance().applicationService()->selectedEdge(); selectedEdge.has_value()) {
            SC::instance().applicationService()->saveUndoPoint();
            selectedEdge.value()->setDashedLine(m_dashedLineAction->isChecked());
        }
    });
    m_dashedLineAction->setCheckable(true);

    m_deleteEdgeAction = new QAction(tr("Delete edge"), this);
    QObject::connect(m_deleteEdgeAction, &QAction::triggered, this, [=] {
        if (SC::instance().applicationService()->edgeSelectionGroupSize()) {
            SC::instance().applicationService()->saveUndoPoint();
            SC::instance().applicationService()->performEdgeAction({ EdgeAction::Type::Delete });
        }
    });
}

void EdgeContextMenu::populateMenuWithActions()
{
    addAction(m_changeEdgeDirectionAction);

    addSeparator();

    addAction(m_showEdgeArrowAction);

    addSeparator();

    addAction(m_doubleArrowAction);

    addSeparator();

    addAction(m_dashedLineAction);

    addSeparator();

    addAction(m_deleteEdgeAction);
}

void EdgeContextMenu::connectVisibilitySignals()
{
    using SceneItems::EdgeModel;

    // Set correct edge config when the menu opens.
    connect(this, &QMenu::aboutToShow, this, [=] {
        if (const auto selectedEdge = SC::instance().applicationService()->selectedEdge(); selectedEdge.has_value()) {
            m_changeEdgeDirectionAction->setEnabled(selectedEdge.value()->arrowMode() != EdgeModel::ArrowMode::Double && selectedEdge.value()->arrowMode() != EdgeModel::ArrowMode::Hidden);
            m_dashedLineAction->setChecked(selectedEdge.value()->dashedLine());
            m_doubleArrowAction->setChecked(selectedEdge.value()->arrowMode() == EdgeModel::ArrowMode::Double);
            m_showEdgeArrowAction->setChecked(selectedEdge.value()->arrowMode() != EdgeModel::ArrowMode::Hidden);
        }
    });

    // Always clear edge selection when the menu closes.
    connect(this, &QMenu::aboutToHide, [=] {
        QTimer::singleShot(0, this, [=] {
            SC::instance().applicationService()->clearEdgeSelectionGroup();
        });
    });
}

void EdgeContextMenu::changeEvent(QEvent * event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate();
    }
}

void EdgeContextMenu::retranslate()
{
    clear();

    initialize();
}

} // namespace Menus
