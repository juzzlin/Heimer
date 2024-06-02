// This file is part of Heimer.
// Copyright (C) 2018 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <QColor>
#include <QString>
#include <QVector>

#include <chrono>

#include "../infra/io/alz_file_io_version.hpp"

namespace Constants {

namespace Application {

IO::AlzFormatVersion alzFormatVersion();

QString applicationName();

QString applicationPackageType();

QString applicationVersion();

QString copyright();

QString fileExtension();

QString qSettingsCompanyName();

QString qSettingsSoftwareName();

QString releasesUrl();

QString supportSiteUrl();

QString translationsResourceBase();

QString webSiteUrl();

} // namespace Application

namespace Settings {

QString defaultsSettingGroup();

double defaultArrowSize();

double defaultEdgeWidth();

int defaultShadowEffectOffset();

int defaultShadowEffectBlurRadius();

int defaultSelectedItemShadowEffectBlurRadius();

QColor defaultShadowEffectShadowColor();

QColor defaultShadowEffectSelectedItemShadowColor();

} // namespace Settings

namespace Edge {

static const double ARROW_SIZE_STEP = 5;

static const double MIN_ARROW_SIZE = 5;

static const double MAX_ARROW_SIZE = 99;

static const double MIN_EDGE_WIDTH = 0.1;

static const double MAX_EDGE_WIDTH = 5.0;

static const double EDGE_WIDTH_STEP = 0.25;

static const int TEXT_EDIT_HOR_PADDING = 3;

static const int TEXT_EDIT_VER_PADDING = 1;

} // namespace Edge

namespace MindMap {

namespace Defaults {

static const QColor BACKGROUND_COLOR { 0xba, 0xbd, 0xb6 };

static const QColor EDGE_COLOR { 0, 0, 0, 200 };

static const QColor GRID_COLOR { Qt::gray };

static const QColor NODE_COLOR { Qt::white };

static const QColor NODE_TEXT_COLOR { Qt::black };

static const int TEXT_SIZE = 14;

} // namespace Defaults

} // namespace MindMap

namespace Misc {

QString threeDots();

} // namespace Misc

namespace Node {

namespace Defaults {

static const int CORNER_RADIUS = 5;

} // namespace Defaults

static const int HIDE_HANDLES_DISTANCE = 32;

static const int MAX_CORNER_RADIUS = 20;

static const int MIN_HEIGHT = 75;

static const int MIN_WIDTH = 200;

} // namespace Node

namespace RecentFiles {

static const int MAX_FILES = 8;

} // namespace RecentFiles

namespace LayoutOptimizer {

namespace Defaults {

static const int MIN_EDGE_LENGTH = 100;

static const double ASPECT_RATIO = 1.0;

} // namespace Defaults

static const double MIN_EDGE_LENGTH = 10;

static const double MAX_EDGE_LENGTH = 1000;

static const double MIN_ASPECT_RATIO = 0.01;

static const double MAX_ASPECT_RATIO = 100;

} // namespace LayoutOptimizer

namespace Scene {

static const double ADJUSTMENT_MARGIN = .25;

static const QColor BARRIER_COLOR { 255, 0, 0, 128 };

static const int INITIAL_SIZE = 10000;

} // namespace Scene

namespace View {

int minTextSize();

int maxTextSize();

std::chrono::milliseconds tooQuickActionDelay();

double zoomSensitivity();

} // namespace View

} // namespace Constants

#endif // CONSTANTS_HPP
