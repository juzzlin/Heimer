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

#include "language_menu_test.hpp"

#include "../../application/language_service.hpp"
#include "../../common/constants.hpp"
#include "../../common/test_mode.hpp"
#include "../../view/menus/language_menu.hpp"

#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QSettings>
#include <QSignalSpy>
#include <QTest>

LanguageMenuTest::LanguageMenuTest()
{
    TestMode::setEnabled(true);
}

LanguageMenuTest::~LanguageMenuTest() = default;

void LanguageMenuTest::cleanupTestCase()
{
    QSettings {}.clear();
}

void LanguageMenuTest::test_nonNativeMenuBar_shouldAddLanguageMenuToNonNativeCornerMenuBar()
{
    QMenuBar menuBar;
    menuBar.setNativeMenuBar(false);
    LanguageService languageService;

    const auto languageMenu = Menus::addLanguageMenu(menuBar, false, languageService);

    QVERIFY(languageMenu);
    QVERIFY(menuBar.actions().isEmpty());

    const auto cornerMenuBar = qobject_cast<QMenuBar *>(menuBar.cornerWidget(Qt::TopRightCorner));
    QVERIFY(cornerMenuBar);
    QVERIFY(!cornerMenuBar->isNativeMenuBar());
    QCOMPARE(cornerMenuBar->actions().size(), 1);
    QCOMPARE(cornerMenuBar->actions().at(0)->menu(), languageMenu);
}

void LanguageMenuTest::test_nonNativeMenuBar_shouldReuseCornerMenuBar()
{
    QMenuBar menuBar;
    menuBar.setNativeMenuBar(false);
    LanguageService languageService;

    Menus::addLanguageMenu(menuBar, false, languageService);
    const auto cornerMenuBar = menuBar.cornerWidget(Qt::TopRightCorner);
    const auto languageMenu = Menus::addLanguageMenu(menuBar, false, languageService); // E.g. retranslate

    QCOMPARE(menuBar.cornerWidget(Qt::TopRightCorner), cornerMenuBar);
    QCOMPARE(menuBar.findChildren<QMenuBar *>().size(), 1);
    QCOMPARE(cornerMenuBar->actions().size(), 1);
    QCOMPARE(cornerMenuBar->actions().at(0)->menu(), languageMenu);
}

void LanguageMenuTest::test_nativeMenuBar_shouldAddLanguageMenuAsTopLevelMenu()
{
    // Regression test for https://github.com/juzzlin/Heimer/issues/267
    QMenuBar menuBar;
    menuBar.addMenu("&File");
    menuBar.addMenu("&Help");
    LanguageService languageService;

    const auto languageMenu = Menus::addLanguageMenu(menuBar, true, languageService);

    QVERIFY(languageMenu);
    QCOMPARE(menuBar.actions().size(), 3);
    QCOMPARE(menuBar.actions().at(0)->text(), QString { "&File" });
    QCOMPARE(menuBar.actions().at(1)->text(), QString { "&Help" });
    QCOMPARE(menuBar.actions().at(2)->menu(), languageMenu);
}

void LanguageMenuTest::test_nativeMenuBar_shouldNotCreateAdditionalMenuBars()
{
    // Regression test for https://github.com/juzzlin/Heimer/issues/267:
    // A second (native) menu bar replaced the main menu bar in global menus.
    QMenuBar menuBar;
    LanguageService languageService;

    Menus::addLanguageMenu(menuBar, true, languageService);

    QVERIFY(!menuBar.cornerWidget(Qt::TopRightCorner));
    QVERIFY(menuBar.findChildren<QMenuBar *>().isEmpty());
}

void LanguageMenuTest::test_menu_titleShouldBeActiveLanguage()
{
    QMenuBar menuBar;
    LanguageService languageService;
    languageService.setActiveLanguage("fi");

    QCOMPARE(Menus::addLanguageMenu(menuBar, true, languageService)->title(), QString { "FI" });
    QCOMPARE(Menus::addLanguageMenu(menuBar, false, languageService)->title(), QString { "FI" });
}

void LanguageMenuTest::test_menu_shouldListOtherSupportedLanguages()
{
    QMenuBar menuBar;
    LanguageService languageService;
    languageService.setActiveLanguage("fi");

    const auto languageMenu = Menus::addLanguageMenu(menuBar, true, languageService);

    QStringList actual;
    for (auto && action : languageMenu->actions()) {
        actual << action->text();
    }

    QStringList expected;
    for (auto && language : Constants::Application::supportedLanguages()) {
        if (language != "fi") {
            expected << QString::fromStdString(language).toUpper();
        }
    }

    QCOMPARE(actual, expected);
}

void LanguageMenuTest::test_menu_triggeringLanguageActionShouldChangeActiveLanguage()
{
    QMenuBar menuBar;
    LanguageService languageService;
    languageService.setActiveLanguage("fi");
    QSignalSpy spy { &languageService, &LanguageService::activeLanguageChanged };

    const auto languageMenu = Menus::addLanguageMenu(menuBar, true, languageService);
    QAction * germanAction = nullptr;
    for (auto && action : languageMenu->actions()) {
        if (action->text() == "DE") {
            germanAction = action;
        }
    }
    QVERIFY(germanAction);

    germanAction->trigger();

    QCOMPARE(languageService.activeLanguage(), QString { "de" });
    QCOMPARE(spy.size(), 1);
    QCOMPARE(spy.at(0).at(0).toString(), QString { "de" });
}

QTEST_MAIN(LanguageMenuTest)
