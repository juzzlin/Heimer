// This file is part of Heimer.
// Copyright (C) 2026 Jussi Lind <jussi.lind@iki.fi>
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

#include "language_menu.hpp"

#include "../../application/language_service.hpp"

#include <QAction>
#include <QMenu>
#include <QMenuBar>

namespace Menus {

QMenu * addLanguageMenu(QMenuBar & menuBar, bool nativeMenuBar, LanguageService & languageService)
{
    const auto title = languageService.activeLanguage().toUpper();

    QMenu * languageMenu = nullptr;
    if (nativeMenuBar) {
        languageMenu = menuBar.addMenu(title);
    } else {
        auto cornerMenuBar = qobject_cast<QMenuBar *>(menuBar.cornerWidget(Qt::TopRightCorner));
        if (!cornerMenuBar) {
            cornerMenuBar = new QMenuBar { &menuBar };
            cornerMenuBar->setNativeMenuBar(false);
            menuBar.setCornerWidget(cornerMenuBar, Qt::TopRightCorner);
        }
        // Delete the old language menu, if any. Use deleteLater(), because this is typically
        // called due to a language change triggered by an action of the old menu itself.
        const auto oldMenus = cornerMenuBar->findChildren<QMenu *>(QString {}, Qt::FindDirectChildrenOnly);
        cornerMenuBar->clear();
        for (auto && menu : oldMenus) {
            menu->deleteLater();
        }
        languageMenu = cornerMenuBar->addMenu(title);
    }

    for (auto && language : languageService.selectableLanguagesWithoutActiveLanguage()) {
        const auto languageAction = new QAction { language.toUpper(), languageMenu };
        languageMenu->addAction(languageAction);
        QObject::connect(languageAction, &QAction::triggered, &languageService, [&languageService, language] {
            languageService.setActiveLanguage(language);
        });
    }

    return languageMenu;
}

} // namespace Menus
