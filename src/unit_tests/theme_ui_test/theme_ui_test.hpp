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
#ifndef THEME_UI_TEST_HPP
#define THEME_UI_TEST_HPP

#include "../unit_test_base.hpp"

#include <memory>

class QTemporaryDir;
class SettingsProxy;
class ThemeService;

class ThemeUiTest : public UnitTestBase
{
    Q_OBJECT

public:
    ThemeUiTest();

    ~ThemeUiTest() override;

private slots:

    void init();

    void cleanup();

    void test_themePreview_shouldRenderThemeColors();

    void test_colorButton_shouldStoreColor();

    void test_themeEditorDialog_unchangedThemeShouldRoundTrip();

    void test_themeEditorDialog_resetShouldRestoreClassic();

    void test_themesTab_shouldListAllThemes();

    void test_themesTab_factoryThemesShouldNotBeEditable();

    void test_themesTab_userThemesShouldBeEditable();

    void test_themesTab_shouldMarkDefaultTheme();

private:
    std::shared_ptr<ThemeService> createThemeService() const;

    std::unique_ptr<QTemporaryDir> m_tempDir;

    std::shared_ptr<SettingsProxy> m_settingsProxy;
};

#endif // THEME_UI_TEST_HPP
