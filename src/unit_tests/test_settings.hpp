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

#ifndef TEST_SETTINGS_HPP
#define TEST_SETTINGS_HPP

#include <QSettings>
#include <QStandardPaths>
#include <QTemporaryDir>

namespace TestSettings {

//! Stores the settings and the application data of the test process in a temporary directory.
//! Tests never touch the real user settings this way, and they work also if HOME is not writable,
//! e.g. in a CI container, and if several test runs execute in parallel.
//! Must be called before any settings are created.
inline void isolate()
{
    QStandardPaths::setTestModeEnabled(true);

    static const QTemporaryDir settingsDir;
    if (settingsDir.isValid()) {
        QSettings::setPath(QSettings::NativeFormat, QSettings::UserScope, settingsDir.path());
        QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, settingsDir.path());
    }
}

} // namespace TestSettings

#endif // TEST_SETTINGS_HPP
