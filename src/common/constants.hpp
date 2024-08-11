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
#include <set>

#include "../infra/io/alz_file_io_version.hpp"

namespace Constants {

namespace Application {

IO::AlzFormatVersion alzFormatVersion();

QString applicationName();

QString applicationPackageType();

QString applicationVersion();

QString copyright();

QString fileExtension();

using LanguageSet = std::set<std::string>;

LanguageSet supportedLanguages();

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

double arrowSizeStep();

double minArrowSize();

double maxArrowSize();

double minEdgeWidth();

double maxEdgeWidth();

double edgeWidthStep();

} // namespace Edge

namespace MindMap {

QColor defaultBackgroundColor();

QColor defaultEdgeColor();

QColor defaultGridColor();

QColor defaultNodeColor();

QColor defaultNodeTextColor();

int defaultTextSize();

} // namespace MindMap

namespace Misc {

QString threeDots();

} // namespace Misc

namespace Node {

int defaultCornerRadius();

int minHeight();

int minWidth();

} // namespace Node

namespace LayoutOptimizer {

double minEdgeLength();

double maxEdgeLength();

double minAspectRatio();

double maxAspectRatio();

} // namespace LayoutOptimizer

namespace View {

int minTextSize();

int maxTextSize();

std::chrono::milliseconds tooQuickActionDelay();

double zoomSensitivity();

} // namespace View

} // namespace Constants

#endif // CONSTANTS_HPP
