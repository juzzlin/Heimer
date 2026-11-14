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

#ifndef THEME_SERVICE_HPP
#define THEME_SERVICE_HPP

#include "../common/types.hpp"
#include "../domain/theme.hpp"

#include <QString>

#include <optional>
#include <vector>

//! Manages the theme presets: the read-only factory themes and the user themes,
//! which are saved as JSON. Also knows the default theme for new mind maps.
class ThemeService
{
public:
    //! \param userThemesFilePath The JSON file of the user themes.
    ThemeService(SettingsProxyS settingsProxy, QString userThemesFilePath);

    static QString defaultUserThemesFilePath();

    std::vector<Theme> factoryThemes() const;

    std::vector<Theme> userThemes() const;

    //! \return The factory themes followed by the user themes.
    std::vector<Theme> themes() const;

    std::optional<Theme> theme(const QString & name) const;

    bool isFactoryTheme(const QString & name) const;

    bool isUserTheme(const QString & name) const;

    //! Adds a new user theme or replaces the user theme of the same name.
    //! \return false if the name is empty or a name of a factory theme, or if saving fails.
    bool saveUserTheme(const Theme & theme);

    //! \return false if there's no such user theme or if saving fails.
    bool removeUserTheme(const QString & name);

    //! \return false if there's no such user theme, the new name is taken or if saving fails.
    bool renameUserTheme(const QString & name, const QString & newName);

    //! \return The name of the theme for new mind maps or an empty string if not set.
    QString defaultThemeName() const;

    //! \return false if there's no such theme.
    bool setDefaultThemeName(const QString & name);

    //! \return The default theme for new mind maps. If not set, a theme based on the
    //! legacy default settings is returned.
    Theme defaultTheme() const;

    //! Saves the legacy default settings as a user theme and sets it as the default theme,
    //! unless they are equal to the "Classic" theme. Done only once.
    void migrateLegacyDefaults();

    //! The name of the user theme created by migrateLegacyDefaults().
    static QString migratedThemeName();

private:
    void load();

    bool save() const;

    std::vector<Theme>::iterator findUserTheme(const QString & name);

    SettingsProxyS m_settingsProxy;

    QString m_userThemesFilePath;

    std::vector<Theme> m_userThemes;
};

#endif // THEME_SERVICE_HPP
