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

#include "main_menu_test.hpp"

#include "../../common/constants.hpp"
#include "../../common/test_mode.hpp"
#include "../../view/menus/main_menu.hpp"

#include <QAction>
#include <QCoreApplication>
#include <QEvent>
#include <QMenu>
#include <QSettings>
#include <QTest>

namespace {

void changeLanguage(Menus::MainMenu & mainMenu)
{
    QEvent event { QEvent::LanguageChange };
    QCoreApplication::sendEvent(&mainMenu, &event);
    QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
}

QAction * findAction(Menus::MainMenu & mainMenu, QString text)
{
    QAction * found = nullptr;
    for (auto && action : mainMenu.findChildren<QAction *>()) {
        if (action->text() == text) {
            if (found) {
                return nullptr; // Not unique, e.g. leaked old action
            }
            found = action;
        }
    }
    return found;
}

QStringList dynamicActionTexts()
{
    return {
        "&Save",
        "&Save as" + Constants::Misc::threeDots(),
        "Undo",
        "Redo",
        "Connect selected nodes",
        "Disconnect selected nodes"
    };
}

void setDynamicActionsEnabled(Menus::MainMenu & mainMenu, bool enabled)
{
    mainMenu.setSaveActionEnabled(enabled);
    mainMenu.setSaveAsActionEnabled(enabled);
    mainMenu.setUndoActionEnabled(enabled);
    mainMenu.setRedoActionEnabled(enabled);
    mainMenu.setConnectSelectedNodesActionEnabled(enabled);
    mainMenu.setDisconnectSelectedNodesActionEnabled(enabled);
}

void verifyDynamicActionsEnabled(Menus::MainMenu & mainMenu, bool enabled)
{
    for (auto && text : dynamicActionTexts()) {
        const auto action = findAction(mainMenu, text);
        QVERIFY2(action, qPrintable("Action not found or not unique: " + text));
        QVERIFY2(action->isEnabled() == enabled, qPrintable("Wrong enabled state: " + text));
    }
}

QStringList menuStructure(const QMenu & menu, QString indent)
{
    QStringList structure;
    for (auto && action : menu.actions()) {
        structure << indent + action->text();
        if (action->menu()) {
            structure << menuStructure(*action->menu(), indent + "  ");
        }
    }
    return structure;
}

QStringList menuStructure(const Menus::MainMenu & mainMenu)
{
    QStringList structure;
    for (auto && action : mainMenu.actions()) {
        structure << action->text();
        if (action->menu()) {
            structure << menuStructure(*action->menu(), "  ");
        }
    }
    return structure;
}

} // namespace

MainMenuTest::MainMenuTest()
{
    TestMode::setEnabled(true);
}

void MainMenuTest::cleanupTestCase()
{
    QSettings {}.clear();
}

void MainMenuTest::test_retranslate_shouldPreserveEnabledActions()
{
    Menus::MainMenu mainMenu;
    mainMenu.initialize();
    setDynamicActionsEnabled(mainMenu, true);

    changeLanguage(mainMenu);

    verifyDynamicActionsEnabled(mainMenu, true);
}

void MainMenuTest::test_retranslate_shouldPreserveDisabledActions()
{
    Menus::MainMenu mainMenu;
    mainMenu.initialize();
    setDynamicActionsEnabled(mainMenu, false);

    changeLanguage(mainMenu);

    verifyDynamicActionsEnabled(mainMenu, false);
}

void MainMenuTest::test_retranslate_shouldPreserveFullScreenState()
{
    Menus::MainMenu mainMenu;
    mainMenu.initialize();
    mainMenu.triggerFullScreenAction();
    QVERIFY(mainMenu.isFullScreenActionChecked());

    changeLanguage(mainMenu);

    QVERIFY(mainMenu.isFullScreenActionChecked());
}

void MainMenuTest::test_retranslate_shouldPreserveMenuStructure()
{
    Menus::MainMenu mainMenu;
    mainMenu.initialize();
    const auto structure = menuStructure(mainMenu);
    QVERIFY(!structure.isEmpty());

    changeLanguage(mainMenu);

    QCOMPARE(menuStructure(mainMenu), structure);
}

void MainMenuTest::test_retranslate_shouldNotLeakMenusOrActions()
{
    Menus::MainMenu mainMenu;
    mainMenu.initialize();
    const auto objectCount = mainMenu.findChildren<QObject *>().size();

    for (int i = 0; i < 3; i++) {
        changeLanguage(mainMenu);
        QCOMPARE(mainMenu.findChildren<QObject *>().size(), objectCount);
    }
}

QTEST_MAIN(MainMenuTest)
