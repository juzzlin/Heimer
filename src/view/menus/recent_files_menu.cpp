// This file is part of Heimer.
// Copyright (C) 2020 Jussi Lind <jussi.lind@iki.fi>
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

#include "recent_files_menu.hpp"

#include "../../application/recent_files_manager.hpp"
#include "../../application/service_container.hpp"
#include "../../common/utils.hpp"

#include <functional>

namespace Menus {

RecentFilesMenu::RecentFilesMenu(QWidget * parent)
  : QMenu(parent)
{
    connect(this, &QMenu::aboutToShow, [=] {
        for (auto && action : actions()) {
            removeAction(action);
        }
        for (auto && filePath : SIC::instance().recentFilesManager()->recentFiles()) {
            const auto action = addAction(filePath);
            const auto handler = std::bind([=](QString filePath) {
                SIC::instance().recentFilesManager()->setSelectedFile(filePath);
                fileSelected(filePath);
            },
                                           action->text());
            action->setEnabled(Utils::fileExists(filePath));
            connect(action, &QAction::triggered, handler);
        }
    });
}

} // namespace Menus
