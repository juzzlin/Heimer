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

#include "tool_bar_test.hpp"

#include "../../common/test_mode.hpp"
#include "../../infra/settings.hpp"
#include "../../view/menus/tool_bar.hpp"
#include "../test_settings.hpp"

#include <QCheckBox>
#include <QSettings>
#include <QSignalSpy>
#include <QTest>

namespace {

QCheckBox * findCheckBox(Menus::ToolBar & toolBar, QString text)
{
    for (auto && checkBox : toolBar.findChildren<QCheckBox *>()) {
        if (checkBox->text() == text) {
            return checkBox;
        }
    }
    return nullptr;
}

QCheckBox * showGridCheckBox(Menus::ToolBar & toolBar)
{
    return findCheckBox(toolBar, "Show grid");
}

QCheckBox * autoSnapCheckBox(Menus::ToolBar & toolBar)
{
    return findCheckBox(toolBar, "Auto snap");
}

} // namespace

void ToolBarTest::initTestCase()
{
    TestMode::setEnabled(true);

    // Never touch the real user settings
    TestSettings::isolate();
}

void ToolBarTest::init()
{
    QSettings {}.clear();
}

void ToolBarTest::cleanupTestCase()
{
    QSettings {}.clear();
}

void ToolBarTest::test_showGrid_checkingShouldEmitCheckedAndSaveState()
{
    Menus::ToolBar toolBar;
    const auto checkBox = showGridCheckBox(toolBar);
    QVERIFY(checkBox);
    QSignalSpy spy { &toolBar, &Menus::ToolBar::gridVisibleChanged };

    checkBox->setChecked(true);

    QCOMPARE(spy.size(), 1);
    QCOMPARE(spy.at(0).at(0).toInt(), static_cast<int>(Qt::Checked));
    QCOMPARE(Settings::Custom::loadGridVisibleState(), Qt::Checked);
}

void ToolBarTest::test_showGrid_uncheckingShouldEmitUncheckedAndSaveState()
{
    Menus::ToolBar toolBar;
    const auto checkBox = showGridCheckBox(toolBar);
    QVERIFY(checkBox);
    checkBox->setChecked(true);
    QSignalSpy spy { &toolBar, &Menus::ToolBar::gridVisibleChanged };

    checkBox->setChecked(false);

    QCOMPARE(spy.size(), 1);
    QCOMPARE(spy.at(0).at(0).toInt(), static_cast<int>(Qt::Unchecked));
    QCOMPARE(Settings::Custom::loadGridVisibleState(), Qt::Unchecked);
}

void ToolBarTest::test_autoSnap_togglingShouldSaveState()
{
    Menus::ToolBar toolBar;
    const auto checkBox = autoSnapCheckBox(toolBar);
    QVERIFY(checkBox);

    checkBox->setChecked(true);
    QCOMPARE(Settings::Custom::loadAutoSnapState(), Qt::Checked);
    QVERIFY(toolBar.autoSnapEnabled());

    checkBox->setChecked(false);
    QCOMPARE(Settings::Custom::loadAutoSnapState(), Qt::Unchecked);
    QVERIFY(!toolBar.autoSnapEnabled());
}

void ToolBarTest::test_loadSettings_shouldRestoreCheckBoxStates()
{
    Settings::Custom::saveAutoSnapState(Qt::Checked);
    Settings::Custom::saveGridVisibleState(Qt::Checked);

    Menus::ToolBar toolBar;
    toolBar.loadSettings();

    QCOMPARE(autoSnapCheckBox(toolBar)->checkState(), Qt::Checked);
    QCOMPARE(showGridCheckBox(toolBar)->checkState(), Qt::Checked);
}

void ToolBarTest::test_loadSettings_shouldEmitGridVisibleChangedWhenGridIsVisible()
{
    // The editor view gets the initial grid visibility via this signal
    Settings::Custom::saveGridVisibleState(Qt::Checked);

    Menus::ToolBar toolBar;
    QSignalSpy spy { &toolBar, &Menus::ToolBar::gridVisibleChanged };
    toolBar.loadSettings();

    QCOMPARE(spy.size(), 1);
    QCOMPARE(spy.at(0).at(0).toInt(), static_cast<int>(Qt::Checked));
}

QTEST_MAIN(ToolBarTest)
