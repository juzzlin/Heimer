// This file is part of Heimer.
// Copyright (C) 2020 Jussi Lind <jussi.lind@iki.fi>
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

#include "settings.hpp"

#include <QSettings>
#include <QStandardPaths>

namespace {
const auto settingsGroupApplication = "Application";
const auto settingsGroupMainWindow = "MainWindow";
const auto recentImagePathKey = "recentImagePath";
const auto recentPathKey = "recentPath";
const auto windowSizeKey = "size";
} // namespace

QString Settings::loadRecentPath()
{
    QSettings settings;
    settings.beginGroup(settingsGroupApplication);
    const auto path = settings.value(recentPathKey, QStandardPaths::writableLocation(QStandardPaths::HomeLocation)).toString();
    settings.endGroup();
    return path;
}

void Settings::saveRecentPath(QString path)
{
    QSettings settings;
    settings.beginGroup(settingsGroupApplication);
    settings.setValue(recentPathKey, path);
    settings.endGroup();
}

QString Settings::loadRecentImagePath()
{
    QSettings settings;
    settings.beginGroup(settingsGroupApplication);
    const QString path = settings.value(recentImagePathKey, QStandardPaths::writableLocation(QStandardPaths::HomeLocation)).toString();
    settings.endGroup();
    return path;
}

void Settings::saveRecentImagePath(QString path)
{
    QSettings settings;
    settings.beginGroup(settingsGroupApplication);
    settings.setValue(recentImagePathKey, path);
    settings.endGroup();
}

QSize Settings::loadWindowSize(QSize defaultSize)
{
    QSettings settings;
    settings.beginGroup(settingsGroupMainWindow);
    const auto size = settings.value(windowSizeKey, defaultSize).toSize();
    settings.endGroup();
    return size;
}

void Settings::saveWindowSize(QSize size)
{
    QSettings settings;
    settings.beginGroup(settingsGroupMainWindow);
    settings.setValue(windowSizeKey, size);
    settings.endGroup();
}
