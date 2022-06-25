// This file is part of Heimer.
// Copyright (C) 2022 Jussi Lind <jussi.lind@iki.fi>
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

#include "scene_color_dialog.hpp"

#include "../mediator.hpp"
#include "../node_action.hpp"

SceneColorDialog::SceneColorDialog(Role role, std::shared_ptr<Mediator> mediator)
  : ColorDialog(role)
{
    connect(this, &QColorDialog::colorSelected, [=](QColor color) {
        if (!color.isValid()) {
            return;
        }
        switch (role) {
        case Role::Background:
            mediator->setBackgroundColor(color);
            break;
        case Role::Edge:
            mediator->setEdgeColor(color);
            break;
        case Role::Grid:
            mediator->setGridColor(color);
            break;
        case Role::Node:
            mediator->performNodeAction({ NodeAction::Type::SetNodeColor, color });
            break;
        case Role::Text:
            mediator->performNodeAction({ NodeAction::Type::SetTextColor, color });
            break;
        case Role::ShadowColor:
        case Role::SelectedItemShadowColor:
            break;
        }
    });
}
