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
#include "constants.hpp"

#include <QSettings>
#include <QStandardPaths>
#include <QTimer>

#include <memory>

namespace {

const auto settingsGroupApplication = "Application";

const auto settingsGroupDefaults = "Defaults";

const auto settingsGroupMainWindow = "MainWindow";

const auto recentImagePathKey = "recentImagePath";

const auto edgeArrowModeKey = "edgeArrowMode";

const auto gridSizeKey = "gridSize";

const auto gridVisibleStateKey = "gridVisibleState";

const auto recentPathKey = "recentPath";

const auto reversedEdgeDirectionKey = "reversedEdgeDirection";

const auto selectNodeGroupByIntersectionKey = "selectNodeGroupByIntersection";

const auto windowFullScreenKey = "fullScreen";

const auto windowSizeKey = "size";

} // namespace

Edge::ArrowMode Settings::loadEdgeArrowMode(Edge::ArrowMode defaultMode)
{
    QSettings settings;
    settings.beginGroup(settingsGroupDefaults);
    const auto mode = static_cast<Edge::ArrowMode>(settings.value(edgeArrowModeKey, static_cast<int>(defaultMode)).toInt());
    settings.endGroup();
    return mode;
}

void Settings::saveEdgeArrowMode(Edge::ArrowMode mode)
{
    QSettings settings;
    settings.beginGroup(settingsGroupDefaults);
    settings.setValue(edgeArrowModeKey, static_cast<int>(mode));
    settings.endGroup();
}

bool Settings::loadReversedEdgeDirection(bool defaultDirection)
{
    QSettings settings;
    settings.beginGroup(settingsGroupDefaults);
    const auto direction = settings.value(reversedEdgeDirectionKey, defaultDirection).toBool();
    settings.endGroup();
    return direction;
}

void Settings::saveReversedEdgeDirection(bool reversed)
{
    QSettings settings;
    settings.beginGroup(settingsGroupDefaults);
    settings.setValue(reversedEdgeDirectionKey, reversed);
    settings.endGroup();
}

int Settings::loadGridSize()
{
    QSettings settings;
    settings.beginGroup(settingsGroupMainWindow);
    const auto gridSize = settings.value(gridSizeKey, 0).toInt();
    settings.endGroup();
    return gridSize;
}

void Settings::saveGridSize(int value)
{
    static std::unique_ptr<QTimer> gridSizeTimer;
    static int sGridSizeValue;
    sGridSizeValue = value;
    if (!gridSizeTimer) {
        gridSizeTimer = std::make_unique<QTimer>();
        gridSizeTimer->setSingleShot(true);
        gridSizeTimer->setInterval(Constants::View::TOO_QUICK_ACTION_DELAY_MS);
        gridSizeTimer->connect(gridSizeTimer.get(), &QTimer::timeout, [&]() {
            QSettings settings;
            settings.beginGroup(settingsGroupMainWindow);
            settings.setValue(gridSizeKey, sGridSizeValue);
            settings.endGroup();
        });
    }
    if (gridSizeTimer->isActive()) {
        gridSizeTimer->stop();
    }
    gridSizeTimer->start();
}

Qt::CheckState Settings::loadGridVisibleState()
{
    QSettings settings;
    settings.beginGroup(settingsGroupMainWindow);
    const auto gridState = settings.value(gridVisibleStateKey, Qt::Unchecked).toInt();
    settings.endGroup();
    return static_cast<Qt::CheckState>(gridState);
}

void Settings::saveGridVisibleState(int state)
{
    QSettings settings;
    settings.beginGroup(settingsGroupMainWindow);
    settings.setValue(gridVisibleStateKey, state);
    settings.endGroup();
}

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

bool Settings::loadFullScreen()
{
    QSettings settings;
    settings.beginGroup(settingsGroupMainWindow);
    const auto fullScreen = settings.value(windowFullScreenKey, false).toBool();
    settings.endGroup();
    return fullScreen;
}

void Settings::saveFullScreen(bool fullScreen)
{
    QSettings settings;
    settings.beginGroup(settingsGroupMainWindow);
    settings.setValue(windowFullScreenKey, fullScreen);
    settings.endGroup();
}

bool Settings::loadSelectNodeGroupByIntersection()
{
    QSettings settings;
    settings.beginGroup(settingsGroupDefaults);
    const auto fullScreen = settings.value(selectNodeGroupByIntersectionKey, false).toBool();
    settings.endGroup();
    return fullScreen;
}

void Settings::saveSelectNodeGroupByIntersection(bool selectNodeGroupByIntersection)
{
    QSettings settings;
    settings.beginGroup(settingsGroupDefaults);
    settings.setValue(selectNodeGroupByIntersectionKey, selectNodeGroupByIntersection);
    settings.endGroup();
}
