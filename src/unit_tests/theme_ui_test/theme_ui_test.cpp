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
#include "theme_ui_test.hpp"

#include "../../application/factory_themes.hpp"
#include "../../application/settings_proxy.hpp"
#include "../../application/theme_service.hpp"
#include "../../common/test_mode.hpp"
#include "../../domain/theme.hpp"
#include "../../view/dialogs/theme_editor_dialog.hpp"
#include "../../view/dialogs/themes_tab.hpp"
#include "../../view/theme_preview.hpp"
#include "../../view/widgets/color_button.hpp"

#include <QDialogButtonBox>
#include <QDir>
#include <QImage>
#include <QListWidget>
#include <QPushButton>
#include <QSettings>
#include <QTemporaryDir>
#include <QTest>

namespace {

QPushButton * findButton(QWidget & parent, QString text)
{
    for (auto && button : parent.findChildren<QPushButton *>()) {
        if (button->text().startsWith(text)) {
            return button;
        }
    }
    return nullptr;
}

Theme createUserTheme(QString name)
{
    Theme theme;
    theme.name = name;
    theme.backgroundColor = { 1, 2, 3 };
    theme.edgeLabelColor = { 4, 5, 6 };
    theme.edgeWidth = 2.5;
    theme.shadowEffect = { 4, 6, 30, QColor { 1, 2, 3, 40 }, QColor { 5, 6, 7, 80 } };
    return theme;
}

} // namespace

ThemeUiTest::ThemeUiTest()
{
    TestMode::setEnabled(true);
}

ThemeUiTest::~ThemeUiTest() = default;

void ThemeUiTest::init()
{
    QSettings {}.clear();
    m_settingsProxy = std::make_shared<SettingsProxy>();
    m_tempDir = std::make_unique<QTemporaryDir>();
    QVERIFY(m_tempDir->isValid());
}

void ThemeUiTest::cleanup()
{
    m_settingsProxy.reset();
    m_tempDir.reset();
    QSettings {}.clear();
}

std::shared_ptr<ThemeService> ThemeUiTest::createThemeService() const
{
    return std::make_shared<ThemeService>(m_settingsProxy, m_tempDir->path() + QDir::separator() + "themes.json");
}

void ThemeUiTest::test_themePreview_shouldRenderThemeColors()
{
    const QSize size { 96, 48 };
    for (auto && theme : FactoryThemes::themes()) {
        const auto image = ThemePreview::render(theme, size).toImage();

        QCOMPARE(image.size(), size);
        // The top-right corner shows only the canvas
        QCOMPARE(QColor { image.pixel(size.width() - 1, 0) }, theme.backgroundColor);
    }
}

void ThemeUiTest::test_colorButton_shouldStoreColor()
{
    Widgets::ColorButton button { "Test", true };

    button.setColor({ 1, 2, 3, 4 });

    QCOMPARE(button.color(), QColor(1, 2, 3, 4));
    QVERIFY(!button.icon().isNull());
}

void ThemeUiTest::test_themeEditorDialog_unchangedThemeShouldRoundTrip()
{
    auto themes = FactoryThemes::themes();
    themes.push_back(createUserTheme("Custom"));
    for (auto && theme : themes) {
        const Dialogs::ThemeEditorDialog dialog { theme };

        QVERIFY2(dialog.theme() == theme, qPrintable(theme.name));
    }
}

void ThemeUiTest::test_themeEditorDialog_resetShouldRestoreClassic()
{
    Dialogs::ThemeEditorDialog dialog { createUserTheme("Custom") };
    const auto buttonBox = dialog.findChild<QDialogButtonBox *>();
    QVERIFY(buttonBox);

    buttonBox->button(QDialogButtonBox::Reset)->click();

    QVERIFY(dialog.theme().isVisuallyEqual(Theme {}));
    QCOMPARE(dialog.theme().name, QString { "Custom" });
}

void ThemeUiTest::test_themeEditorDialog_applyShouldEmitEditedTheme()
{
    Dialogs::ThemeEditorDialog dialog { createUserTheme("Custom") };
    const auto buttonBox = dialog.findChild<QDialogButtonBox *>();
    QVERIFY(buttonBox);
    std::vector<Theme> appliedThemes;
    connect(&dialog, &Dialogs::ThemeEditorDialog::applied, this, [&appliedThemes](const Theme & theme) {
        appliedThemes.push_back(theme);
    });
    buttonBox->button(QDialogButtonBox::Reset)->click();

    buttonBox->button(QDialogButtonBox::Apply)->click();

    QCOMPARE(appliedThemes.size(), size_t { 1 });
    QVERIFY(appliedThemes.at(0) == dialog.theme());
    QVERIFY(appliedThemes.at(0).isVisuallyEqual(Theme {}));
    QCOMPARE(dialog.result(), int { QDialog::Rejected });
}

void ThemeUiTest::test_themesTab_shouldListAllThemes()
{
    const auto themeService = createThemeService();
    QVERIFY(themeService->saveUserTheme(createUserTheme("Mine")));

    Dialogs::ThemesTab themesTab { "Themes", themeService };

    const auto list = themesTab.findChild<QListWidget *>();
    QVERIFY(list);
    QCOMPARE(static_cast<size_t>(list->count()), themeService->themes().size());
    for (int i = 0; i < list->count(); i++) {
        QVERIFY(!list->item(i)->icon().isNull());
    }
}

void ThemeUiTest::test_themesTab_factoryThemesShouldNotBeEditable()
{
    Dialogs::ThemesTab themesTab { "Themes", createThemeService() };

    themesTab.selectTheme("Black");

    QCOMPARE(themesTab.selectedThemeName(), QString { "Black" });
    QVERIFY(findButton(themesTab, "Apply")->isEnabled());
    QVERIFY(findButton(themesTab, "Duplicate")->isEnabled());
    QVERIFY(!findButton(themesTab, "Edit")->isEnabled());
    QVERIFY(!findButton(themesTab, "Rename")->isEnabled());
    QVERIFY(!findButton(themesTab, "Delete")->isEnabled());
}

void ThemeUiTest::test_themesTab_userThemesShouldBeEditable()
{
    const auto themeService = createThemeService();
    QVERIFY(themeService->saveUserTheme(createUserTheme("Mine")));
    Dialogs::ThemesTab themesTab { "Themes", themeService };

    themesTab.selectTheme("Mine");

    QCOMPARE(themesTab.selectedThemeName(), QString { "Mine" });
    QVERIFY(findButton(themesTab, "Edit")->isEnabled());
    QVERIFY(findButton(themesTab, "Rename")->isEnabled());
    QVERIFY(findButton(themesTab, "Delete")->isEnabled());
}

void ThemeUiTest::test_themesTab_shouldMarkDefaultTheme()
{
    const auto themeService = createThemeService();
    QVERIFY(themeService->setDefaultThemeName("Paper"));
    Dialogs::ThemesTab themesTab { "Themes", themeService };

    themesTab.selectTheme("White");
    findButton(themesTab, "Set as default")->click();

    QCOMPARE(themeService->defaultThemeName(), QString { "White" });
    const auto list = themesTab.findChild<QListWidget *>();
    for (int i = 0; i < list->count(); i++) {
        const auto item = list->item(i);
        QCOMPARE(item->font().bold(), item->data(Qt::UserRole).toString() == "White");
    }
    QCOMPARE(themesTab.selectedThemeName(), QString { "White" });
}

QTEST_MAIN(ThemeUiTest)
