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
#ifndef THEME_JSON_TEST_HPP
#define THEME_JSON_TEST_HPP

#include <QObject>

class ThemeJsonTest : public QObject
{
    Q_OBJECT

private slots:

    void test_roundTrip_shouldPreserveAllProperties();

    void test_fromJson_missingPropertiesShouldKeepDefaults();

    void test_fromJson_invalidValuesShouldKeepDefaults();

    void test_document_roundTripShouldPreserveThemesAndOrder();

    void test_fromDocument_invalidDocumentShouldReturnNullopt();

    void test_fromDocument_themesWithoutNameShouldBeSkipped();
};

#endif // THEME_JSON_TEST_HPP
