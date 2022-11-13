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

#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "edge.hpp"

#include <QSize>

namespace Settings {

namespace V1 {

bool loadAutosave();

void saveAutosave(bool autosave);

Qt::CheckState loadAutoSnapState();

void saveAutoSnapState(int state);

EdgeModel::ArrowMode loadEdgeArrowMode(EdgeModel::ArrowMode defaultMode);

void saveEdgeArrowMode(EdgeModel::ArrowMode mode);

bool loadReversedEdgeDirection(bool defaultDirection);

void saveReversedEdgeDirection(bool reversed);

int loadGridSize();

void saveGridSize(int value);

Qt::CheckState loadGridVisibleState();

void saveGridVisibleState(int state);

QString loadRecentPath();

void saveRecentPath(QString path);

QString loadRecentImagePath();

void saveRecentImagePath(QString path);

bool loadSelectNodeGroupByIntersection();

void saveSelectNodeGroupByIntersection(bool selectNodeGroupByIntersection);

QSize loadWindowSize(QSize defaultSize);

void saveWindowSize(QSize size);

bool loadFullScreen();

void saveFullScreen(bool fullScreen);

} // namespace V1

namespace V2 {

bool getBoolean(QString group, QString key, bool defaultValue);

void setBoolean(QString group, QString key, bool value);

QColor getColor(QString group, QString key, QColor defaultValue);

void setColor(QString group, QString key, QColor value);

double getNumber(QString group, QString key, double defaultValue);

void setNumber(QString group, QString key, double value);

} // namespace V2

} // namespace Settings

#endif // SETTINGS_HPP
