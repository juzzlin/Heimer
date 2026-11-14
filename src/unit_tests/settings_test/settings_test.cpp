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
#include "settings_test.hpp"

#include "../../infra/settings.hpp"

#include <QFont>
#include <QSettings>
#include <QStandardPaths>
#include <QTest>

static const auto group = "SettingsTest";

void SettingsTest::initTestCase()
{
    // Never touch the real user settings
    QStandardPaths::setTestModeEnabled(true);
}

void SettingsTest::init()
{
    QSettings {}.clear();
}

void SettingsTest::cleanupTestCase()
{
    QSettings {}.clear();
}

void SettingsTest::test_getFont_defaultValueShouldKeepWeight()
{
    for (auto && weight : { QFont::Light, QFont::Normal, QFont::Bold }) {
        QFont defaultFont;
        defaultFont.setWeight(weight);

        const auto font = Settings::Generic::getFont(group, "notSaved", defaultFont);

        QCOMPARE(font.weight(), weight);
    }
}

void SettingsTest::test_getFont_shouldReturnSavedFont()
{
    QFont savedFont;
    savedFont.setFamily("Serif");
    savedFont.setWeight(QFont::DemiBold);
    savedFont.setItalic(true);
    Settings::Generic::setFont(group, "font", savedFont);

    const auto font = Settings::Generic::getFont(group, "font", {});

    QCOMPARE(font.family(), savedFont.family());
    QCOMPARE(font.weight(), QFont::DemiBold);
    QCOMPARE(font.italic(), true);
}

QTEST_GUILESS_MAIN(SettingsTest)
