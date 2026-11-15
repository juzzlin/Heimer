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

#ifndef RECENT_FILES_MANAGER_TEST_HPP
#define RECENT_FILES_MANAGER_TEST_HPP

#include <QObject>

class RecentFilesManagerTest : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase();
    void init();

    void test_addRecentFile_shouldPutFileFirst();
    void test_addRecentFile_shouldNotDuplicateFiles();
    void test_addRecentFile_shouldLimitFileCount();
    void test_addRecentFile_shouldStoreAbsolutePath();

    void test_recentFiles_shouldBeLoadedFromSettings();

    void test_selectedFile_shouldBeStored();
};

#endif // RECENT_FILES_MANAGER_TEST_HPP
