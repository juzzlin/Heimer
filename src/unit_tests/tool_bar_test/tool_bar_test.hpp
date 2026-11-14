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

#ifndef TOOL_BAR_TEST_HPP
#define TOOL_BAR_TEST_HPP

#include <QObject>

class ToolBarTest : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase();

    void init();

    void cleanupTestCase();

    void test_showGrid_checkingShouldEmitCheckedAndSaveState();

    void test_showGrid_uncheckingShouldEmitUncheckedAndSaveState();

    void test_autoSnap_togglingShouldSaveState();

    void test_loadSettings_shouldRestoreCheckBoxStates();

    void test_loadSettings_shouldEmitGridVisibleChangedWhenGridIsVisible();
};

#endif // TOOL_BAR_TEST_HPP
