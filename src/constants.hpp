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

namespace Constants {

namespace Application {

static constexpr auto APPLICATION_NAME = "Heimer";

#ifndef PACKAGE_TYPE
static constexpr auto APPLICATION_PACKAGE_TYPE = "N/A";
#else
static constexpr auto APPLICATION_PACKAGE_TYPE = PACKAGE_TYPE;
#endif

static constexpr auto APPLICATION_VERSION = VERSION;

static constexpr auto COPYRIGHT = "Copyright (c) 2018-2021 Jussi Lind";

static constexpr auto FILE_EXTENSION = ".alz";

static constexpr auto QSETTINGS_COMPANY_NAME = "Heimer";

static constexpr auto WEB_SITE_URL = "http://juzzlin.github.io/Heimer";

static constexpr auto QSETTINGS_SOFTWARE_NAME = APPLICATION_NAME;

static constexpr auto SUPPORT_SITE_URL = "https://www.patreon.com/juzzlin";

static constexpr auto TRANSLATIONS_RESOURCE_BASE = ":/translations/heimer_";

} // namespace Application

namespace Edge {

static const double ARROW_LENGTH = 10;

static const double ARROW_OPENING = 150;

static const double CORNER_RADIUS_SCALE = 0.3;

static const QColor DOT_COLOR { 255, 0, 0, 192 };

static const int DOT_DURATION = 2000;

static const int DOT_RADIUS = 10;

static const QColor LABEL_COLOR { 0xff, 0xee, 0xaa };

static const int LABEL_DURATION = 2000;

static const double MIN_SIZE = 0.1;

static const double MAX_SIZE = 5.0;

static const double STEP = 0.1;

static const int TEXT_EDIT_ANIMATION_DURATION = 150;

static const int TEXT_EDIT_DURATION = 2000;

static const double WIDTH_SCALE = 0.5;

} // namespace Edge

namespace Export {

namespace Png {

static const QString FILE_EXTENSION = ".png";

static const int MIN_IMAGE_SIZE = 0;

static const int MAX_IMAGE_SIZE = 99999;

} // namespace Png

namespace Svg {

static const QString FILE_EXTENSION = ".svg";

} // namespace Svg

} // namespace Export

namespace Grid {

static const int MIN_SIZE = 0;

static const int MAX_SIZE = 500;

} // namespace Grid

namespace MindMap {

static const QColor DEFAULT_BACKGROUND_COLOR { 0xba, 0xbd, 0xb6 };

static const QColor DEFAULT_EDGE_COLOR { 0, 0, 0, 200 };

static const QColor DEFAULT_GRID_COLOR { Qt::gray };

static const double DEFAULT_EDGE_WIDTH = 2.0;

static const int DEFAULT_TEXT_SIZE = 11;

} // namespace MindMap

namespace Node {

static const int DEFAULT_CORNER_RADIUS = 5;

static const int HANDLE_ANIMATION_DURATION = 150;

static const double HANDLE_OPACITY = 0.8;

static const int HANDLE_RADIUS = 28;

static const int HANDLE_RADIUS_MEDIUM = 24;

static const int HANDLE_RADIUS_SMALL = 20;

static const int MARGIN = 10;

static const int MAX_CORNER_RADIUS = 20;

static const int MIN_HEIGHT = 75;

static const int MIN_WIDTH = 200;

static const QColor TEXT_EDIT_BACKGROUND_COLOR { 0x00, 0x00, 0x00, 0x10 };

} // namespace Node

namespace RecentFiles {

static const int MAX_FILES = 8;

static const QString QSETTINGS_ARRAY_KEY = "recentFilesArray";

static const QString QSETTINGS_FILE_PATH_KEY = "filePath";

} // namespace RecentFiles

namespace LayoutOptimizer {

static const int DEFAULT_MIN_EDGE_LENGTH = 100;

static const double MIN_EDGE_LENGTH = 10;

static const double MAX_EDGE_LENGTH = 250;

static const double DEFAULT_ASPECT_RATIO = 1.0;

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

static const int MAX_SIZE = 24;

} // namespace Text

namespace View {

static const int CLICK_TOLERANCE = 5;

static const double DRAG_NODE_OPACITY = 0.5;

static const int TOO_QUICK_ACTION_DELAY_MS = 500;

static const double ZOOM_MAX = 2.0;

static const double ZOOM_MIN = .02;

static const double ZOOM_SENSITIVITY = 1.1;

} // namespace View

} // namespace Constants

#endif // CONSTANTS_HPP
