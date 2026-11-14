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

#ifndef LANGUAGE_SERVICE_TEST_HPP
#define LANGUAGE_SERVICE_TEST_HPP

#include "../unit_test_base.hpp"

class LanguageServiceTest : public UnitTestBase
{
    Q_OBJECT

public:
    LanguageServiceTest();

private slots:

    void cleanupTestCase();

    void test_initializeTranslations_supportedLanguage_shouldBeActivated();

    void test_initializeTranslations_unsupportedLanguage_shouldFallBackToDefaultLanguage();

    void test_initializeTranslations_unsupportedLanguage_shouldNotSaveFallbackLanguage();

    void test_selectableLanguagesWithoutActiveLanguage_shouldNotContainActiveLanguage();
};

#endif // LANGUAGE_SERVICE_TEST_HPP
