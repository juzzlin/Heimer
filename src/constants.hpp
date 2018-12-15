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

namespace Edge {

static const double ARROW_LENGTH = 10;

static const double ARROW_OPENING = 150;

static const QColor DOT_COLOR{255, 0, 0, 192};

static const int DOT_DURATION = 2000;

static const int DOT_RADIUS = 10;

static const QColor LABEL_COLOR{0xff, 0xee, 0xaa};

static const int LABEL_DURATION = 2000;

static const double MIN_SIZE = 0.1;

static const double MAX_SIZE = 5.0;

static const double STEP = 0.1;

} // Edge

namespace Node {

static const int HANDLE_RADIUS = 28;

static const int HANDLE_RADIUS_SMALL = 20;

static const int MARGIN = 10;

static const int MIN_HEIGHT = 75;

static const int MIN_WIDTH = 200;

} // Node

namespace Text {

static const int MIN_SIZE = 6;

static const int MAX_SIZE = 24;

} // Grid

} // Constants

#endif // CONSTANTS_HPP
