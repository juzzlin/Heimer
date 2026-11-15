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

#include "recent_files_manager_test.hpp"

#include "../../application/recent_files_manager.hpp"
#include "../../common/constants.hpp"
#include "../../infra/settings.hpp"
#include "../test_settings.hpp"

#include <QDir>
#include <QTest>

void RecentFilesManagerTest::initTestCase()
{
    // Never touch the real user settings
    TestSettings::isolate();
}

void RecentFilesManagerTest::init()
{
    Settings::Custom::saveRecentFiles({});
}

void RecentFilesManagerTest::test_addRecentFile_shouldPutFileFirst()
{
    RecentFilesManager rfm;
    QVERIFY(!rfm.hasRecentFiles());
    QVERIFY(!rfm.recentFile().has_value());

    rfm.addRecentFile("/maps/a.alz");
    rfm.addRecentFile("/maps/b.alz");

    QVERIFY(rfm.hasRecentFiles());
    QCOMPARE(rfm.recentFile().value(), QString { "/maps/b.alz" });
    QCOMPARE(rfm.recentFiles(), (QStringList { "/maps/b.alz", "/maps/a.alz" }));
}

void RecentFilesManagerTest::test_addRecentFile_shouldNotDuplicateFiles()
{
    RecentFilesManager rfm;
    rfm.addRecentFile("/maps/a.alz");
    rfm.addRecentFile("/maps/b.alz");
    rfm.addRecentFile("/maps/a.alz");

    QCOMPARE(rfm.recentFiles(), (QStringList { "/maps/a.alz", "/maps/b.alz" }));
}

void RecentFilesManagerTest::test_addRecentFile_shouldLimitFileCount()
{
    RecentFilesManager rfm;
    const int fileCount = Constants::Misc::maxRecentFileCount() + 3;
    for (int i = 0; i < fileCount; i++) {
        rfm.addRecentFile(QString { "/maps/%1.alz" }.arg(i));
    }

    QCOMPARE(rfm.recentFiles().size(), Constants::Misc::maxRecentFileCount());
    QCOMPARE(rfm.recentFiles().first(), QString { "/maps/%1.alz" }.arg(fileCount - 1));
    QCOMPARE(rfm.recentFiles().last(), QString { "/maps/%1.alz" }.arg(fileCount - Constants::Misc::maxRecentFileCount()));
}

void RecentFilesManagerTest::test_addRecentFile_shouldStoreAbsolutePath()
{
    RecentFilesManager rfm;
    rfm.addRecentFile("relative.alz");

    QCOMPARE(rfm.recentFile().value(), QDir::current().absoluteFilePath("relative.alz"));
}

void RecentFilesManagerTest::test_recentFiles_shouldBeLoadedFromSettings()
{
    {
        RecentFilesManager rfm;
        rfm.addRecentFile("/maps/a.alz");
        rfm.addRecentFile("/maps/b.alz");
    }

    const RecentFilesManager rfm;
    QCOMPARE(rfm.recentFiles(), (QStringList { "/maps/b.alz", "/maps/a.alz" }));
}

void RecentFilesManagerTest::test_selectedFile_shouldBeStored()
{
    RecentFilesManager rfm;
    QVERIFY(rfm.selectedFile().isEmpty());

    rfm.setSelectedFile("/maps/a.alz");

    QCOMPARE(rfm.selectedFile(), QString { "/maps/a.alz" });
}

QTEST_GUILESS_MAIN(RecentFilesManagerTest)
