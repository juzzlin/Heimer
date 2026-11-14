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
#ifndef THEME_SERVICE_TEST_HPP
#define THEME_SERVICE_TEST_HPP

#include "../unit_test_base.hpp"

#include <memory>

class QTemporaryDir;
class SettingsProxy;
class ThemeService;

class ThemeServiceTest : public UnitTestBase
{
    Q_OBJECT

public:
    ThemeServiceTest();

    ~ThemeServiceTest() override;

private slots:

    void init();

    void cleanup();

    void test_factoryThemes_shouldContainExpectedThemesWithClassicFirst();

    void test_factoryThemes_shouldBeVisuallyDistinct();

    void test_saveUserTheme_shouldPersist();

    void test_saveUserTheme_shouldReplaceThemeOfSameName();

    void test_saveUserTheme_shouldRejectFactoryAndEmptyNames();

    void test_removeUserTheme_shouldRemoveAndResetDefault();

    void test_renameUserTheme_shouldRenameAndUpdateDefault();

    void test_renameUserTheme_shouldRejectTakenName();

    void test_setDefaultThemeName_shouldRejectUnknownTheme();

    void test_defaultTheme_shouldReturnNamedTheme();

    void test_defaultTheme_withoutNameShouldReturnLegacyTheme();

    void test_migrateLegacyDefaults_classicDefaultsShouldSelectClassic();

    void test_migrateLegacyDefaults_customDefaultsShouldCreateUserTheme();

    void test_migrateLegacyDefaults_shouldBeDoneOnlyOnce();

    void test_migrateLegacyDefaults_shouldNotOverwriteExistingTheme();

    void test_load_invalidFileShouldBeKeptAsBackup();

private:
    QString themesFilePath() const;

    std::unique_ptr<ThemeService> createThemeService() const;

    std::unique_ptr<QTemporaryDir> m_tempDir;

    std::shared_ptr<SettingsProxy> m_settingsProxy;
};

#endif // THEME_SERVICE_TEST_HPP
