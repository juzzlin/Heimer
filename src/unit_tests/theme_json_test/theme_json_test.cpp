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
#include "theme_json_test.hpp"

#include "../../infra/theme_json.hpp"

#include <QJsonDocument>
#include <QTest>

namespace {

Theme createTestTheme(QString name)
{
    Theme theme;
    theme.name = name;
    theme.backgroundColor = { 1, 2, 3 };
    theme.gridColor = { 4, 5, 6 };
    theme.nodeColor = { 7, 8, 9 };
    theme.nodeTextColor = { 10, 11, 12 };
    theme.cornerRadius = 13;
    theme.font.setFamily("Serif");
    theme.font.setBold(true);
    theme.textSize = 15;
    theme.edgeColor = { 16, 17, 18 };
    theme.edgeWidth = 1.25;
    theme.arrowSize = 19.5;
    theme.edgeLabelColor = { 20, 21, 22 };
    theme.edgeLabelTextColor = { 23, 24, 25 };
    theme.shadowEffect = { 26, 27, 28, QColor { 29, 30, 31, 32 }, QColor { 33, 34, 35, 36 } };
    return theme;
}

} // namespace

void ThemeJsonTest::test_roundTrip_shouldPreserveAllProperties()
{
    const auto theme = createTestTheme("Test");

    const auto restored = ThemeJson::fromJson(ThemeJson::toJson(theme));

    QVERIFY(restored == theme);
}

void ThemeJsonTest::test_fromJson_missingPropertiesShouldKeepDefaults()
{
    const auto json = QJsonDocument::fromJson(R"({ "name": "Partial", "edgeLabelColor": "#ff112233" })").object();

    const auto theme = ThemeJson::fromJson(json);

    Theme expected;
    expected.name = "Partial";
    expected.edgeLabelColor = { 0x11, 0x22, 0x33 };
    QVERIFY(theme == expected);
}

void ThemeJsonTest::test_fromJson_invalidValuesShouldKeepDefaults()
{
    const auto json = QJsonDocument::fromJson(R"({ "name": "Invalid", "edgeColor": "not a color", "textSize": "big", "shadow": 42 })").object();

    const auto theme = ThemeJson::fromJson(json);

    Theme expected;
    expected.name = "Invalid";
    QVERIFY(theme == expected);
}

void ThemeJsonTest::test_document_roundTripShouldPreserveThemesAndOrder()
{
    const std::vector<Theme> themes = { createTestTheme("B"), createTestTheme("A"), Theme {} };
    auto namedDefault = themes;
    namedDefault.at(2).name = "Defaults";

    const auto restored = ThemeJson::fromDocument(ThemeJson::toDocument(namedDefault));

    QVERIFY(restored.has_value());
    QCOMPARE(restored->size(), size_t { 3 });
    QVERIFY(restored->at(0) == namedDefault.at(0));
    QVERIFY(restored->at(1) == namedDefault.at(1));
    QVERIFY(restored->at(2) == namedDefault.at(2));
}

void ThemeJsonTest::test_fromDocument_invalidDocumentShouldReturnNullopt()
{
    QVERIFY(!ThemeJson::fromDocument("").has_value());
    QVERIFY(!ThemeJson::fromDocument("{ broken").has_value());
    QVERIFY(!ThemeJson::fromDocument("[]").has_value());
    QVERIFY(!ThemeJson::fromDocument(R"({ "themes": 1 })").has_value());
}

void ThemeJsonTest::test_fromDocument_themesWithoutNameShouldBeSkipped()
{
    const auto themes = ThemeJson::fromDocument(R"({ "themes": [ { "textSize": 10 }, { "name": "Named" }, 3 ] })");

    QVERIFY(themes.has_value());
    QCOMPARE(themes->size(), size_t { 1 });
    QCOMPARE(themes->at(0).name, QString { "Named" });
}

QTEST_GUILESS_MAIN(ThemeJsonTest)
