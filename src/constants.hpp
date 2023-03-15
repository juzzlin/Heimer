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
#include <QVector>

#include "io/alz_file_io_version.hpp"

namespace Constants {

namespace Application {

static constexpr auto APPLICATION_NAME = "Heimer";

#ifndef PACKAGE_TYPE
static constexpr auto APPLICATION_PACKAGE_TYPE = "N/A";
#else
static constexpr auto APPLICATION_PACKAGE_TYPE = PACKAGE_TYPE;
#endif

static constexpr auto APPLICATION_VERSION = VERSION;

static constexpr auto ALZ_FORMAT_VERSION = IO::AlzFormatVersion::V2;

static constexpr auto COPYRIGHT = "Copyright (c) 2018-2023 Jussi Lind";

static constexpr auto FILE_EXTENSION = ".alz";

static constexpr auto QSETTINGS_COMPANY_NAME = "Heimer";

static constexpr auto WEB_SITE_URL = "http://juzzlin.github.io/Heimer";

static constexpr auto QSETTINGS_SOFTWARE_NAME = APPLICATION_NAME;

static constexpr auto RELEASES_URL = "https://github.com/juzzlin/Heimer/releases";

static constexpr auto SUPPORT_SITE_URL = "https://paypal.me/juzzlin";

static constexpr auto TRANSLATIONS_RESOURCE_BASE = ":/translations/heimer_";

} // namespace Application

namespace Edge {

namespace Defaults {

static const double ARROW_SIZE = 20;

} // namespace Defaults

static const double ARROW_SIZE_STEP = 5;

static const double ARROW_OPENING = 150;

static const double CORNER_RADIUS_SCALE = 0.3;

static const QVector<qreal> DASH_PATTERN { qreal(5), qreal(5) };

static const QColor DOT_COLOR { 255, 0, 0, 192 };

static const int DOT_DURATION = 2000;

static const int DOT_RADIUS = 10;

static const QColor LABEL_COLOR { 0xff, 0xee, 0xaa };

static const int LABEL_DURATION = 2000;

static const double MIN_ARROW_SIZE = 5;

static const double MAX_ARROW_SIZE = 99;

static const double MIN_EDGE_WIDTH = 0.1;

static const double MAX_EDGE_WIDTH = 5.0;

static const double EDGE_WIDTH_STEP = 0.25;

static const int TEXT_EDIT_ANIMATION_DURATION = 150;

static const int TEXT_EDIT_DURATION = 2000;

static const double WIDTH_SCALE = 0.5;

} // namespace Edge

namespace Export {

namespace Png {

static const auto FILE_EXTENSION = ".png";

static const int MIN_IMAGE_SIZE = 0;

static const int MAX_IMAGE_SIZE = 99999;

} // namespace Png

namespace Svg {

static const auto FILE_EXTENSION = ".svg";

} // namespace Svg

} // namespace Export

namespace Grid {

static const int MIN_SIZE = 0;

static const int MAX_SIZE = 500;

} // namespace Grid

namespace MindMap {

namespace Defaults {

const auto SETTINGS_GROUP = "Defaults";

static const auto ARROW_SIZE_SETTINGS_KEY = "arrowSize";

static const QColor BACKGROUND_COLOR { 0xba, 0xbd, 0xb6 };

static const auto BACKGROUND_COLOR_SETTINGS_KEY = "backgroundColor";

static const QColor EDGE_COLOR { 0, 0, 0, 200 };

static const auto EDGE_COLOR_SETTINGS_KEY = "edgeColor";

static const auto EDGE_WIDTH_SETTINGS_KEY = "edgeWidth";

static const auto FONT_SETTINGS_KEY = "font";

static const QColor GRID_COLOR { Qt::gray };

static const auto GRID_COLOR_SETTINGS_KEY = "gridColor";

static const QColor NODE_COLOR { Qt::white };

static const auto NODE_COLOR_SETTINGS_KEY = "nodeColor";

static const QColor NODE_TEXT_COLOR { Qt::black };

static const auto NODE_TEXT_COLOR_SETTINGS_KEY = "nodeTextColor";

static const double EDGE_WIDTH = 1.0;

static const int TEXT_SIZE = 14;

static const auto TEXT_SIZE_SETTINGS_KEY = "textSize";

} // namespace Defaults

} // namespace MindMap

namespace Misc {

static const auto THREE_DOTS = "...";

} // namespace Misc

namespace Node {

namespace Defaults {

static const int CORNER_RADIUS = 5;

} // namespace Defaults

static const int SCALE_ANIMATION_DURATION = 75;

static const int HANDLE_ANIMATION_DURATION = 150;

static const int HANDLE_VISIBILITY_DURATION = 2500;

static const double HANDLE_OPACITY = 0.75;

static const int HANDLE_RADIUS = 28;

static const int HANDLE_RADIUS_MEDIUM = 24;

static const int HANDLE_RADIUS_SMALL = 20;

static const int HIDE_HANDLES_DISTANCE = 32;

static const int MARGIN = 10;

static const int MAX_CORNER_RADIUS = 20;

static const int MIN_HEIGHT = 75;

static const int MIN_WIDTH = 200;

static const QColor TEXT_EDIT_BACKGROUND_COLOR_DARK { 0x00, 0x00, 0x00, 0x10 };

static const QColor TEXT_EDIT_BACKGROUND_COLOR_LIGHT { 0xff, 0xff, 0xff, 0x10 };

} // namespace Node

namespace RecentFiles {

static const int MAX_FILES = 8;

static const auto QSETTINGS_ARRAY_KEY = "recentFilesArray";

static const auto QSETTINGS_FILE_PATH_KEY = "filePath";

} // namespace RecentFiles

namespace LayoutOptimizer {

namespace Defaults {

static const int MIN_EDGE_LENGTH = 100;

static const double ASPECT_RATIO = 1.0;

} // namespace Defaults

static const double MIN_EDGE_LENGTH = 10;

static const double MAX_EDGE_LENGTH = 250;

static const double MIN_ASPECT_RATIO = 0.1;

static const double MAX_ASPECT_RATIO = 10;

} // namespace LayoutOptimizer

namespace Scene {

static const double ADJUSTMENT_MARGIN = .25;

static const QColor BARRIER_COLOR { 255, 0, 0, 128 };

static const int INITIAL_SIZE = 10000;

} // namespace Scene

namespace Text {

static const int MIN_SIZE = 6;

static const int MAX_SIZE = 72;

} // namespace Text

namespace View {

const auto EDITING_SETTINGS_GROUP = "Editing";

const auto INVERTED_CONTROLS_SETTINGS_KEY = "invertedControls";

static const int CLICK_TOLERANCE = 5;

static const double DRAG_NODE_OPACITY = 0.5;

static const int SHADOW_EFFECT_OPTIMIZATION_MARGIN_FRACTION = 20;

static const int SHADOW_EFFECT_OPTIMIZATION_UPDATE_DELAY_MS = 250;

static const int TEXT_SEARCH_DELAY_MS = 250;

static const int TOO_QUICK_ACTION_DELAY_MS = 500;

static const double ZOOM_MAX = 2.0;

static const double ZOOM_MIN = .02;

static const double ZOOM_SENSITIVITY = 1.1;

} // namespace View

namespace Effects {

namespace Defaults {

static const int SHADOW_EFFECT_OFFSET = 3;

static const int SHADOW_EFFECT_BLUR_RADIUS = 5;

static const int SELECTED_ITEM_SHADOW_EFFECT_BLUR_RADIUS = 50;

static const QColor SHADOW_EFFECT_SHADOW_COLOR = { 96, 96, 96 };

static const QColor SHADOW_EFFECT_SELECTED_ITEM_SHADOW_COLOR = { 255, 0, 0 };

} // namespace Defaults

static const auto EFFECTS_SETTINGS_GROUP = "Effects";

static const int SHADOW_EFFECT_MAX_OFFSET = 10;

static const int SHADOW_EFFECT_MIN_OFFSET = 0;

static const int SHADOW_EFFECT_MAX_BLUR_RADIUS = 100;

static const int SHADOW_EFFECT_MIN_BLUR_RADIUS = 0;

static const auto SHADOW_EFFECT_OFFSET_SETTINGS_KEY = "shadowEffectOffset";

static const auto SHADOW_EFFECT_NORMAL_BLUR_RADIUS_SETTINGS_KEY = "shadowEffectBlurRadius";

static const auto SHADOW_EFFECT_SELECTED_ITEM_BLUR_RADIUS_SETTINGS_KEY = "shadowEffectSelectedItemBlurRadius";

static const auto SHADOW_EFFECT_SHADOW_COLOR_SETTINGS_KEY = "shadowEffectShadowColor";

static const auto SHADOW_EFFECT_SELECTED_ITEM_SHADOW_COLOR_SETTINGS_KEY = "shadowEffectSelectedItemShadowColor";

} // namespace Effects

} // namespace Constants

#endif // CONSTANTS_HPP
