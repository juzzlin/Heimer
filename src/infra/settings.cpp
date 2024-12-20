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
#include "../common/constants.hpp"
#include "../common/utils.hpp"

#include <QSettings>
#include <QStandardPaths>
#include <QTimer>

#include <memory>

namespace {

const auto autoloadKey = "autoload";

const auto autosaveKey = "autosave";

const auto autoSnapStateKey = "autoSnap";

const auto edgeArrowModeKey = "edgeArrowMode";

const auto fontFamilyPostfix = "_family";

const auto fontBoldPostfix = "_bold";

const auto fontItalicPostfix = "_italic";

const auto fontOverlinePostfix = "_overline";

const auto fontStrikeOutPostfix = "_strikeOut";

const auto fontUnderlinePostfix = "_underline";

const auto fontWeightPostfix = "_weight";

const auto gridSizeKey = "gridSize";

const auto gridVisibleStateKey = "gridVisibleState";

const auto recentImagePathKey = "recentImagePath";

const auto recentPathKey = "recentPath";

const auto recentFilesArrayKey = "recentFilesArray";

const auto recentFilesFilePathKey = "filePath";

const auto reversedEdgeDirectionKey = "reversedEdgeDirection";

const auto selectNodeGroupByIntersectionKey = "selectNodeGroupByIntersection";

const auto settingGroupApplication = "Application";

const auto settingGroupEditing = "Editing";

const auto settingGroupMainWindow = "MainWindow";

const auto windowFullScreenKey = "fullScreen";

const auto windowSizeKey = "size";

} // namespace

namespace Settings {

namespace Custom {

bool loadAutoload()
{
    QSettings settings;
    settings.beginGroup(settingGroupEditing);
    const auto autoload = settings.value(autoloadKey, false).toBool();
    settings.endGroup();
    return autoload;
}

void saveAutoload(bool autoload)
{
    QSettings settings;
    settings.beginGroup(settingGroupEditing);
    settings.setValue(autoloadKey, autoload);
    settings.endGroup();
}

bool loadAutosave()
{
    QSettings settings;
    settings.beginGroup(settingGroupEditing);
    const auto autosave = settings.value(autosaveKey, false).toBool();
    settings.endGroup();
    return autosave;
}

void saveAutosave(bool autosave)
{
    QSettings settings;
    settings.beginGroup(settingGroupEditing);
    settings.setValue(autosaveKey, autosave);
    settings.endGroup();
}

Qt::CheckState loadAutoSnapState()
{
    QSettings settings;
    settings.beginGroup(settingGroupMainWindow);
    const auto gridState = settings.value(autoSnapStateKey, Qt::Unchecked).toInt();
    settings.endGroup();
    return static_cast<Qt::CheckState>(gridState);
}

void saveAutoSnapState(int state)
{
    QSettings settings;
    settings.beginGroup(settingGroupMainWindow);
    settings.setValue(autoSnapStateKey, state);
    settings.endGroup();
}

SceneItems::EdgeModel::ArrowMode loadEdgeArrowMode(SceneItems::EdgeModel::ArrowMode defaultMode)
{
    QSettings settings;
    settings.beginGroup(Constants::Settings::defaultsSettingGroup());
    const auto mode = static_cast<SceneItems::EdgeModel::ArrowMode>(settings.value(edgeArrowModeKey, static_cast<int>(defaultMode)).toInt());
    settings.endGroup();
    return mode;
}

void saveEdgeArrowMode(SceneItems::EdgeModel::ArrowMode mode)
{
    QSettings settings;
    settings.beginGroup(Constants::Settings::defaultsSettingGroup());
    settings.setValue(edgeArrowModeKey, static_cast<int>(mode));
    settings.endGroup();
}

bool loadReversedEdgeDirection(bool defaultDirection)
{
    QSettings settings;
    settings.beginGroup(Constants::Settings::defaultsSettingGroup());
    const auto direction = settings.value(reversedEdgeDirectionKey, defaultDirection).toBool();
    settings.endGroup();
    return direction;
}

void saveReversedEdgeDirection(bool reversed)
{
    QSettings settings;
    settings.beginGroup(Constants::Settings::defaultsSettingGroup());
    settings.setValue(reversedEdgeDirectionKey, reversed);
    settings.endGroup();
}

int loadGridSize()
{
    QSettings settings;
    settings.beginGroup(settingGroupMainWindow);
    const auto gridSize = settings.value(gridSizeKey, 0).toInt();
    settings.endGroup();
    return gridSize;
}

void saveGridSize(int value)
{
    static std::unique_ptr<QTimer> gridSizeTimer;
    static int sGridSizeValue;
    sGridSizeValue = value;
    if (!gridSizeTimer) {
        gridSizeTimer = std::make_unique<QTimer>();
        gridSizeTimer->setSingleShot(true);
        gridSizeTimer->setInterval(Constants::View::tooQuickActionDelay());
        gridSizeTimer->connect(gridSizeTimer.get(), &QTimer::timeout, gridSizeTimer.get(), [&]() {
            QSettings settings;
            settings.beginGroup(settingGroupMainWindow);
            settings.setValue(gridSizeKey, sGridSizeValue);
            settings.endGroup();
        });
    }
    if (gridSizeTimer->isActive()) {
        gridSizeTimer->stop();
    }
    gridSizeTimer->start();
}

Qt::CheckState loadGridVisibleState()
{
    QSettings settings;
    settings.beginGroup(settingGroupMainWindow);
    return static_cast<Qt::CheckState>(settings.value(gridVisibleStateKey, Qt::Unchecked).toInt());
}

void saveGridVisibleState(int state)
{
    QSettings settings;
    settings.beginGroup(settingGroupMainWindow);
    settings.setValue(gridVisibleStateKey, state);
    settings.endGroup();
}

QString loadRecentPath()
{
    QSettings settings;
    settings.beginGroup(settingGroupApplication);
    const auto path = settings.value(recentPathKey, QStandardPaths::writableLocation(QStandardPaths::HomeLocation)).toString();
    settings.endGroup();
    return path;
}

void saveRecentPath(QString path)
{
    QSettings settings;
    settings.beginGroup(settingGroupApplication);
    settings.setValue(recentPathKey, path);
    settings.endGroup();
}

QStringList loadRecentFiles()
{
    QStringList fileList;
    QSettings settings;
    const int size = settings.beginReadArray(recentFilesArrayKey);
    for (int i = 0; i < size; i++) {
        settings.setArrayIndex(i);
        fileList.push_back(settings.value(recentFilesFilePathKey).toString());
    }
    settings.endArray();
    return fileList;
}

void saveRecentFiles(QStringList fileList)
{
    QSettings settings;
    settings.beginWriteArray(recentFilesArrayKey);
    for (int i = 0; i < fileList.size(); i++) {
        settings.setArrayIndex(i);
        settings.setValue(recentFilesFilePathKey, fileList.at(i));
    }
    settings.endArray();
}

QString loadRecentImagePath()
{
    QSettings settings;
    settings.beginGroup(settingGroupApplication);
    const QString path = settings.value(recentImagePathKey, QStandardPaths::writableLocation(QStandardPaths::HomeLocation)).toString();
    settings.endGroup();
    return path;
}

void saveRecentImagePath(QString path)
{
    QSettings settings;
    settings.beginGroup(settingGroupApplication);
    settings.setValue(recentImagePathKey, path);
    settings.endGroup();
}

QSize loadWindowSize(QSize defaultSize)
{
    QSettings settings;
    settings.beginGroup(settingGroupMainWindow);
    const auto size = settings.value(windowSizeKey, defaultSize).toSize();
    settings.endGroup();
    return size;
}

void saveWindowSize(QSize size)
{
    QSettings settings;
    settings.beginGroup(settingGroupMainWindow);
    settings.setValue(windowSizeKey, size);
    settings.endGroup();
}

bool loadFullScreen()
{
    QSettings settings;
    settings.beginGroup(settingGroupMainWindow);
    const auto fullScreen = settings.value(windowFullScreenKey, false).toBool();
    settings.endGroup();
    return fullScreen;
}

void saveFullScreen(bool fullScreen)
{
    QSettings settings;
    settings.beginGroup(settingGroupMainWindow);
    settings.setValue(windowFullScreenKey, fullScreen);
    settings.endGroup();
}

bool loadSelectNodeGroupByIntersection()
{
    QSettings settings;
    settings.beginGroup(Constants::Settings::defaultsSettingGroup());
    const auto fullScreen = settings.value(selectNodeGroupByIntersectionKey, false).toBool();
    settings.endGroup();
    return fullScreen;
}

void saveSelectNodeGroupByIntersection(bool selectNodeGroupByIntersection)
{
    QSettings settings;
    settings.beginGroup(Constants::Settings::defaultsSettingGroup());
    settings.setValue(selectNodeGroupByIntersectionKey, selectNodeGroupByIntersection);
    settings.endGroup();
}

} // namespace Custom

namespace Generic {

bool getBoolean(QString group, QString key, bool defaultValue)
{
    QSettings settings;
    settings.beginGroup(group);
    const auto val = settings.value(key, defaultValue).toBool();
    settings.endGroup();
    return val;
}

void setBoolean(QString group, QString key, bool value)
{
    QSettings settings;
    settings.beginGroup(group);
    settings.setValue(key, value);
    settings.endGroup();
}

QColor getColor(QString group, QString key, QColor defaultValue)
{
    QSettings settings;
    settings.beginGroup(group);
    const auto val = settings.value(key, defaultValue).value<QColor>();
    settings.endGroup();
    return val;
}

void setColor(QString group, QString key, QColor value)
{
    QSettings settings;
    settings.beginGroup(group);
    settings.setValue(key, value);
    settings.endGroup();
}

double getNumber(QString group, QString key, double defaultValue)
{
    QSettings settings;
    settings.beginGroup(group);
    const auto val = settings.value(key, defaultValue).toDouble();
    settings.endGroup();
    return val;
}

void setNumber(QString group, QString key, double value)
{
    QSettings settings;
    settings.beginGroup(group);
    settings.setValue(key, value);
    settings.endGroup();
}

QString getString(QString group, QString key, QString defaultValue)
{
    QSettings settings;
    settings.beginGroup(group);
    const auto val = settings.value(key, defaultValue).toString();
    settings.endGroup();
    return val;
}

void setString(QString group, QString key, QString value)
{
    QSettings settings;
    settings.beginGroup(group);
    settings.setValue(key, value);
    settings.endGroup();
}

QFont getFont(QString group, QString key, QFont defaultValue)
{
    QSettings settings;
    settings.beginGroup(group);
    auto font = defaultValue;
    font.setFamily(settings.value(key + fontFamilyPostfix, defaultValue.family()).toString());
    font.setBold(settings.value(key + fontBoldPostfix, defaultValue.bold()).toBool());
    font.setItalic(settings.value(key + fontItalicPostfix, defaultValue.italic()).toBool());
    font.setOverline(settings.value(key + fontOverlinePostfix, defaultValue.overline()).toBool());
    font.setStrikeOut(settings.value(key + fontStrikeOutPostfix, defaultValue.strikeOut()).toBool());
    font.setUnderline(settings.value(key + fontUnderlinePostfix, defaultValue.underline()).toBool());
    font.setWeight(Utils::intToFontWeight(settings.value(key + fontWeightPostfix, defaultValue.weight()).toInt()));
    settings.endGroup();
    return font;
}

void setFont(QString group, QString key, QFont value)
{
    QSettings settings;
    settings.beginGroup(group);
    settings.setValue(key + fontFamilyPostfix, value.family());
    settings.setValue(key + fontBoldPostfix, value.bold());
    settings.setValue(key + fontItalicPostfix, value.italic());
    settings.setValue(key + fontOverlinePostfix, value.overline());
    settings.setValue(key + fontStrikeOutPostfix, value.strikeOut());
    settings.setValue(key + fontUnderlinePostfix, value.underline());
    settings.setValue(key + fontWeightPostfix, Utils::fontWeightToInt(value.weight()));
    settings.endGroup();
}

} // namespace Generic

} // namespace Settings
