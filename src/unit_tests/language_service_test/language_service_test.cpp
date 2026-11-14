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

#include "language_service_test.hpp"

#include "../../application/language_service.hpp"
#include "../../application/service_container.hpp"
#include "../../application/settings_proxy.hpp"
#include "../../common/constants.hpp"
#include "../../common/test_mode.hpp"

#include <QCoreApplication>
#include <QSettings>
#include <QTest>

namespace {
// Not a supported language and not a language Qt would have translations for
const QString unsupportedLanguage { "xx" };
} // namespace

LanguageServiceTest::LanguageServiceTest()
{
    TestMode::setEnabled(true);

    // The translations are in a resource of the static library, which is not
    // automatically linked into the test as nothing refers to it.
    Q_INIT_RESOURCE(translations);
}

void LanguageServiceTest::cleanupTestCase()
{
    QSettings {}.clear();
}

void LanguageServiceTest::test_initializeTranslations_supportedLanguage_shouldBeActivated()
{
    LanguageService languageService;
    languageService.setCommandLineLanguage("fi");

    languageService.initializeTranslations(*QCoreApplication::instance());

    QCOMPARE(languageService.activeLanguage(), QString { "fi" });
}

void LanguageServiceTest::test_initializeTranslations_unsupportedLanguage_shouldFallBackToDefaultLanguage()
{
    // Without a fallback the language menu would have an empty title, see MainMenu
    LanguageService languageService;
    languageService.setCommandLineLanguage(unsupportedLanguage);

    languageService.initializeTranslations(*QCoreApplication::instance());

    QCOMPARE(languageService.activeLanguage(), Constants::Application::defaultLanguage());
}

void LanguageServiceTest::test_initializeTranslations_unsupportedLanguage_shouldNotSaveFallbackLanguage()
{
    const auto savedLanguage = SC::instance().settingsProxy()->userLanguage();
    LanguageService languageService;
    languageService.setCommandLineLanguage(unsupportedLanguage);

    languageService.initializeTranslations(*QCoreApplication::instance());

    QCOMPARE(SC::instance().settingsProxy()->userLanguage(), savedLanguage);
}

void LanguageServiceTest::test_selectableLanguagesWithoutActiveLanguage_shouldNotContainActiveLanguage()
{
    LanguageService languageService;
    languageService.setActiveLanguage("de");

    const auto languages = languageService.selectableLanguagesWithoutActiveLanguage();

    QVERIFY(!languages.contains("de"));
    QCOMPARE(static_cast<size_t>(languages.size()), Constants::Application::supportedLanguages().size() - 1);
}

QTEST_GUILESS_MAIN(LanguageServiceTest)
