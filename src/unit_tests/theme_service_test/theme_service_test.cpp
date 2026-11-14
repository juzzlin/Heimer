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
#include "theme_service_test.hpp"

#include "../../application/factory_themes.hpp"
#include "../../application/settings_proxy.hpp"
#include "../../application/theme_service.hpp"
#include "../../common/test_mode.hpp"

#include <QDir>
#include <QFile>
#include <QSettings>
#include <QTemporaryDir>
#include <QTest>

namespace {

Theme createUserTheme(QString name, QColor backgroundColor = { 1, 2, 3 })
{
    Theme theme;
    theme.name = name;
    theme.backgroundColor = backgroundColor;
    return theme;
}

} // namespace

ThemeServiceTest::ThemeServiceTest()
{
    TestMode::setEnabled(true);
}

ThemeServiceTest::~ThemeServiceTest() = default;

void ThemeServiceTest::init()
{
    QSettings {}.clear();
    m_settingsProxy = std::make_shared<SettingsProxy>();
    m_tempDir = std::make_unique<QTemporaryDir>();
    QVERIFY(m_tempDir->isValid());
}

void ThemeServiceTest::cleanup()
{
    m_settingsProxy.reset();
    m_tempDir.reset();
    QSettings {}.clear();
}

QString ThemeServiceTest::themesFilePath() const
{
    return m_tempDir->path() + QDir::separator() + "themes.json";
}

std::unique_ptr<ThemeService> ThemeServiceTest::createThemeService() const
{
    return std::make_unique<ThemeService>(m_settingsProxy, themesFilePath());
}

void ThemeServiceTest::test_factoryThemes_shouldContainExpectedThemesWithClassicFirst()
{
    const auto themeService = createThemeService();

    QStringList names;
    for (auto && theme : themeService->factoryThemes()) {
        names << theme.name;
        QVERIFY(themeService->isFactoryTheme(theme.name));
        QVERIFY(!themeService->isUserTheme(theme.name));
    }

    QCOMPARE(names, QStringList({ "Classic", "High Contrast", "Black", "White", "Accent", "Paper" }));
    QVERIFY(themeService->factoryThemes().at(0).isVisuallyEqual(Theme {}));
}

void ThemeServiceTest::test_factoryThemes_shouldBeVisuallyDistinct()
{
    const auto themes = createThemeService()->factoryThemes();

    for (size_t i = 0; i < themes.size(); i++) {
        for (size_t j = i + 1; j < themes.size(); j++) {
            QVERIFY2(!themes.at(i).isVisuallyEqual(themes.at(j)), qPrintable(themes.at(i).name + " == " + themes.at(j).name));
        }
    }
}

void ThemeServiceTest::test_saveUserTheme_shouldPersist()
{
    const auto theme = createUserTheme("Mine");
    QVERIFY(createThemeService()->saveUserTheme(theme));

    const auto themeService = createThemeService();

    QVERIFY(themeService->isUserTheme("Mine"));
    QCOMPARE(themeService->userThemes().size(), size_t { 1 });
    QVERIFY(themeService->theme("Mine") == theme);
    QCOMPARE(themeService->themes().size(), themeService->factoryThemes().size() + 1);
}

void ThemeServiceTest::test_saveUserTheme_shouldReplaceThemeOfSameName()
{
    const auto themeService = createThemeService();
    QVERIFY(themeService->saveUserTheme(createUserTheme("Mine", { 1, 1, 1 })));

    QVERIFY(themeService->saveUserTheme(createUserTheme("Mine", { 2, 2, 2 })));

    QCOMPARE(themeService->userThemes().size(), size_t { 1 });
    QCOMPARE(themeService->theme("Mine")->backgroundColor, QColor(2, 2, 2));
}

void ThemeServiceTest::test_saveUserTheme_shouldRejectFactoryAndEmptyNames()
{
    const auto themeService = createThemeService();

    QVERIFY(!themeService->saveUserTheme(createUserTheme(FactoryThemes::classicName())));
    QVERIFY(!themeService->saveUserTheme(createUserTheme("")));
    QVERIFY(!themeService->saveUserTheme(createUserTheme("   ")));

    QVERIFY(themeService->userThemes().empty());
    QVERIFY(themeService->theme(FactoryThemes::classicName())->isVisuallyEqual(Theme {}));
}

void ThemeServiceTest::test_removeUserTheme_shouldRemoveAndResetDefault()
{
    const auto themeService = createThemeService();
    QVERIFY(themeService->saveUserTheme(createUserTheme("Mine")));
    QVERIFY(themeService->setDefaultThemeName("Mine"));

    QVERIFY(themeService->removeUserTheme("Mine"));

    QVERIFY(!themeService->theme("Mine").has_value());
    QCOMPARE(themeService->defaultThemeName(), FactoryThemes::classicName());
    QVERIFY(!themeService->removeUserTheme("Mine"));
    QVERIFY(!themeService->removeUserTheme(FactoryThemes::classicName()));
    QVERIFY(createThemeService()->userThemes().empty());
}

void ThemeServiceTest::test_renameUserTheme_shouldRenameAndUpdateDefault()
{
    const auto themeService = createThemeService();
    QVERIFY(themeService->saveUserTheme(createUserTheme("Mine")));
    QVERIFY(themeService->setDefaultThemeName("Mine"));

    QVERIFY(themeService->renameUserTheme("Mine", "Ours"));

    QVERIFY(!themeService->theme("Mine").has_value());
    QVERIFY(themeService->theme("Ours").has_value());
    QCOMPARE(themeService->defaultThemeName(), QString { "Ours" });
    QVERIFY(createThemeService()->isUserTheme("Ours"));
}

void ThemeServiceTest::test_renameUserTheme_shouldRejectTakenName()
{
    const auto themeService = createThemeService();
    QVERIFY(themeService->saveUserTheme(createUserTheme("Mine")));
    QVERIFY(themeService->saveUserTheme(createUserTheme("Other")));

    QVERIFY(!themeService->renameUserTheme("Mine", "Other"));
    QVERIFY(!themeService->renameUserTheme("Mine", FactoryThemes::classicName()));
    QVERIFY(!themeService->renameUserTheme("Mine", ""));
    QVERIFY(!themeService->renameUserTheme(FactoryThemes::classicName(), "New"));

    QVERIFY(themeService->isUserTheme("Mine"));
}

void ThemeServiceTest::test_setDefaultThemeName_shouldRejectUnknownTheme()
{
    const auto themeService = createThemeService();

    QVERIFY(!themeService->setDefaultThemeName("Unknown"));
    QVERIFY(themeService->defaultThemeName().isEmpty());
}

void ThemeServiceTest::test_defaultTheme_shouldReturnNamedTheme()
{
    const auto themeService = createThemeService();

    QVERIFY(themeService->setDefaultThemeName("Paper"));

    QCOMPARE(themeService->defaultTheme().name, QString { "Paper" });
}

void ThemeServiceTest::test_defaultTheme_withoutNameShouldReturnLegacyTheme()
{
    m_settingsProxy->setBackgroundColor({ 9, 8, 7 });
    const auto themeService = createThemeService();

    QCOMPARE(themeService->defaultTheme().backgroundColor, QColor(9, 8, 7));
}

void ThemeServiceTest::test_migrateLegacyDefaults_classicDefaultsShouldSelectClassic()
{
    const auto themeService = createThemeService();

    themeService->migrateLegacyDefaults();

    QCOMPARE(themeService->defaultThemeName(), FactoryThemes::classicName());
    QVERIFY(themeService->userThemes().empty());
    QVERIFY(m_settingsProxy->legacyDefaultsMigrated());
}

void ThemeServiceTest::test_migrateLegacyDefaults_customDefaultsShouldCreateUserTheme()
{
    m_settingsProxy->setBackgroundColor({ 9, 8, 7 });
    const auto themeService = createThemeService();

    themeService->migrateLegacyDefaults();

    QCOMPARE(themeService->defaultThemeName(), ThemeService::migratedThemeName());
    QVERIFY(themeService->isUserTheme(ThemeService::migratedThemeName()));
    QCOMPARE(themeService->defaultTheme().backgroundColor, QColor(9, 8, 7));
}

void ThemeServiceTest::test_migrateLegacyDefaults_shouldBeDoneOnlyOnce()
{
    const auto themeService = createThemeService();
    themeService->migrateLegacyDefaults();
    QVERIFY(themeService->setDefaultThemeName("Black"));
    m_settingsProxy->setBackgroundColor({ 9, 8, 7 });

    themeService->migrateLegacyDefaults();

    QCOMPARE(themeService->defaultThemeName(), QString { "Black" });
    QVERIFY(themeService->userThemes().empty());
}

void ThemeServiceTest::test_migrateLegacyDefaults_shouldNotOverwriteExistingTheme()
{
    const auto themeService = createThemeService();
    const auto existingTheme = createUserTheme(ThemeService::migratedThemeName(), { 1, 1, 1 });
    QVERIFY(themeService->saveUserTheme(existingTheme));
    m_settingsProxy->setBackgroundColor({ 9, 8, 7 });

    themeService->migrateLegacyDefaults();

    QVERIFY(themeService->theme(ThemeService::migratedThemeName()) == existingTheme);
    const auto migratedName = ThemeService::migratedThemeName() + " 2";
    QCOMPARE(themeService->defaultThemeName(), migratedName);
    QCOMPARE(themeService->theme(migratedName)->backgroundColor, QColor(9, 8, 7));
}

void ThemeServiceTest::test_load_invalidFileShouldBeKeptAsBackup()
{
    {
        QFile file { themesFilePath() };
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.write("{ broken");
    }

    const auto themeService = createThemeService();
    QVERIFY(themeService->userThemes().empty());
    QVERIFY(themeService->saveUserTheme(createUserTheme("Mine")));

    QFile backup { themesFilePath() + ".invalid" };
    QVERIFY(backup.open(QIODevice::ReadOnly));
    QCOMPARE(backup.readAll(), QByteArray { "{ broken" });
}

QTEST_GUILESS_MAIN(ThemeServiceTest)
