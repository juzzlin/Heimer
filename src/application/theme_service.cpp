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

#include "theme_service.hpp"

#include "../infra/theme_json.hpp"
#include "factory_themes.hpp"
#include "settings_proxy.hpp"

#include "simple_logger.hpp"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QStandardPaths>

#include <algorithm>

static const auto TAG = "ThemeService";

ThemeService::ThemeService(SettingsProxyS settingsProxy, QString userThemesFilePath)
  : m_settingsProxy(settingsProxy)
  , m_userThemesFilePath(userThemesFilePath)
{
    load();
}

QString ThemeService::defaultUserThemesFilePath()
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QDir::separator() + "themes.json";
}

std::vector<Theme> ThemeService::factoryThemes() const
{
    return FactoryThemes::themes();
}

std::vector<Theme> ThemeService::userThemes() const
{
    return m_userThemes;
}

std::vector<Theme> ThemeService::themes() const
{
    auto themes = factoryThemes();
    themes.insert(themes.end(), m_userThemes.begin(), m_userThemes.end());
    return themes;
}

std::optional<Theme> ThemeService::theme(const QString & name) const
{
    for (auto && theme : themes()) {
        if (theme.name == name) {
            return theme;
        }
    }
    return {};
}

bool ThemeService::isFactoryTheme(const QString & name) const
{
    const auto factoryThemes = this->factoryThemes();
    return std::any_of(factoryThemes.begin(), factoryThemes.end(), [&name](auto && theme) {
        return theme.name == name;
    });
}

bool ThemeService::isUserTheme(const QString & name) const
{
    return std::any_of(m_userThemes.begin(), m_userThemes.end(), [&name](auto && theme) {
        return theme.name == name;
    });
}

std::vector<Theme>::iterator ThemeService::findUserTheme(const QString & name)
{
    return std::find_if(m_userThemes.begin(), m_userThemes.end(), [&name](auto && theme) {
        return theme.name == name;
    });
}

bool ThemeService::saveUserTheme(const Theme & theme)
{
    if (theme.name.trimmed().isEmpty() || isFactoryTheme(theme.name)) {
        juzzlin::L(TAG).warning() << "Invalid user theme name: '" << theme.name.toStdString() << "'";
        return false;
    }

    if (const auto iter = findUserTheme(theme.name); iter != m_userThemes.end()) {
        *iter = theme;
    } else {
        m_userThemes.push_back(theme);
    }

    return save();
}

bool ThemeService::removeUserTheme(const QString & name)
{
    const auto iter = findUserTheme(name);
    if (iter == m_userThemes.end()) {
        return false;
    }

    m_userThemes.erase(iter);

    if (defaultThemeName() == name) {
        m_settingsProxy->setDefaultThemeName(FactoryThemes::classicName());
    }

    return save();
}

bool ThemeService::renameUserTheme(const QString & name, const QString & newName)
{
    const auto iter = findUserTheme(name);
    if (iter == m_userThemes.end() || newName.trimmed().isEmpty() || theme(newName)) {
        return false;
    }

    iter->name = newName;

    if (defaultThemeName() == name) {
        m_settingsProxy->setDefaultThemeName(newName);
    }

    return save();
}

QString ThemeService::defaultThemeName() const
{
    return m_settingsProxy->defaultThemeName();
}

bool ThemeService::setDefaultThemeName(const QString & name)
{
    if (!theme(name)) {
        return false;
    }

    m_settingsProxy->setDefaultThemeName(name);
    return true;
}

Theme ThemeService::defaultTheme() const
{
    if (const auto name = defaultThemeName(); !name.isEmpty()) {
        if (const auto theme = this->theme(name); theme) {
            return *theme;
        }
        juzzlin::L(TAG).warning() << "Default theme '" << name.toStdString() << "' not found";
    }

    return m_settingsProxy->legacyDefaultTheme();
}

QString ThemeService::migratedThemeName()
{
    return "My defaults";
}

void ThemeService::migrateLegacyDefaults()
{
    if (m_settingsProxy->legacyDefaultsMigrated()) {
        return;
    }

    if (defaultThemeName().isEmpty()) {
        auto legacyTheme = m_settingsProxy->legacyDefaultTheme();
        if (Theme classic; legacyTheme.isVisuallyEqual(classic)) {
            m_settingsProxy->setDefaultThemeName(FactoryThemes::classicName());
        } else {
            legacyTheme.name = migratedThemeName();
            for (int i = 2; theme(legacyTheme.name); i++) {
                legacyTheme.name = migratedThemeName() + " " + QString::number(i);
            }
            if (saveUserTheme(legacyTheme)) {
                m_settingsProxy->setDefaultThemeName(legacyTheme.name);
                juzzlin::L(TAG).info() << "Legacy default settings saved as theme '" << legacyTheme.name.toStdString() << "'";
            } else {
                juzzlin::L(TAG).error() << "Could not save legacy default settings as a theme";
                return;
            }
        }
    }

    m_settingsProxy->setLegacyDefaultsMigrated(true);
}

void ThemeService::load()
{
    QFile file { m_userThemesFilePath };
    if (!file.exists()) {
        return;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        juzzlin::L(TAG).error() << "Could not open '" << m_userThemesFilePath.toStdString() << "'";
        return;
    }

    if (const auto themes = ThemeJson::fromDocument(file.readAll()); themes) {
        m_userThemes.clear();
        for (auto && theme : *themes) {
            if (!isFactoryTheme(theme.name) && !isUserTheme(theme.name)) {
                m_userThemes.push_back(theme);
            }
        }
        juzzlin::L(TAG).info() << "Loaded " << m_userThemes.size() << " user theme(s)";
    } else {
        // Don't overwrite the invalid file on the next save
        file.close();
        const auto backupPath = m_userThemesFilePath + ".invalid";
        QFile::remove(backupPath);
        QFile::rename(m_userThemesFilePath, backupPath);
        juzzlin::L(TAG).error() << "Invalid user themes file moved to '" << backupPath.toStdString() << "'";
    }
}

bool ThemeService::save() const
{
    QDir {}.mkpath(QFileInfo { m_userThemesFilePath }.absolutePath());
    QSaveFile file { m_userThemesFilePath };
    if (!file.open(QIODevice::WriteOnly)) {
        juzzlin::L(TAG).error() << "Could not open '" << m_userThemesFilePath.toStdString() << "' for writing";
        return false;
    }

    file.write(ThemeJson::toDocument(m_userThemes));
    if (!file.commit()) {
        juzzlin::L(TAG).error() << "Could not write '" << m_userThemesFilePath.toStdString() << "'";
        return false;
    }

    return true;
}
