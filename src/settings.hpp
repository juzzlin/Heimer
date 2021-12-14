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

bool loadAutosave();

void saveAutosave(bool autosave);

Qt::CheckState loadAutoSnapState();

void saveAutoSnapState(int state);

Edge::ArrowMode loadEdgeArrowMode(Edge::ArrowMode defaultMode);

void saveEdgeArrowMode(Edge::ArrowMode mode);

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

} // namespace Settings

#endif // SETTINGS_HPP
