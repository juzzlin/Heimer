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

#ifndef THEMES_TAB_HPP
#define THEMES_TAB_HPP

#include "settings_tab_base.hpp"

#include <optional>

class QListWidget;
class QPushButton;
struct Theme;

namespace Dialogs {

//! Lists the theme presets and allows applying and managing them.
//! A newly selected theme is applied to the current mind map with Apply/OK of the settings dialog.
//! The management actions take effect immediately, i.e. Cancel of the settings dialog doesn't revert them.
class ThemesTab : public SettingsTabBase
{
    Q_OBJECT

public:
    ThemesTab(QString name, ThemeServiceS themeService, QWidget * parent = nullptr);

    //! Applies the selected theme to the current mind map if the user has selected a theme since the last apply.
    bool apply() override;

    //! \return The name of the selected theme or an empty string.
    QString selectedThemeName() const;

    void selectTheme(const QString & name);

private:
    bool applySelectedTheme();

    //! Applies the theme to the current mind map. Asks about individual node colors using dialogParent.
    //! \return false if the user cancelled.
    bool applyTheme(const Theme & theme, QWidget * dialogParent);

    std::optional<QString> askNewThemeName(QString title, QString initialName);

    void createThemeFromCurrentMindMap();

    void deleteSelectedTheme();

    void duplicateSelectedTheme();

    void editSelectedTheme();

    void initWidgets();

    void refresh(QString selectedName);

    void renameSelectedTheme();

    void setSelectedThemeAsDefault();

    void updateButtons();

    ThemeServiceS m_themeService;

    QListWidget * m_themeList;

    QPushButton * m_setDefaultButton;

    QPushButton * m_newButton;

    QPushButton * m_duplicateButton;

    QPushButton * m_editButton;

    QPushButton * m_renameButton;

    QPushButton * m_deleteButton;

    //! True if the user has selected a theme that hasn't been applied yet
    bool m_themeSelectionChanged = false;

    //! True while the list is being rebuilt, so that programmatic selections are not user selections
    bool m_refreshing = false;
};

} // namespace Dialogs

#endif // THEMES_TAB_HPP
