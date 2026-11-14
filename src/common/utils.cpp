// This file is part of Heimer.
// Copyright (C) 2022 Jussi Lind <jussi.lind@iki.fi>
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

#include "utils.hpp"

#include "constants.hpp"

#include <QDateTime>
#include <QDir>

#include <algorithm>
#include <chrono>
#include <cmath>

size_t Utils::tsMs()
{
    using namespace std::chrono;
    return static_cast<size_t>(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
}

bool Utils::isColorBright(QColor color)
{
    return color.red() * color.red() + color.blue() * color.blue() + color.green() * color.green() > 127 * 127 * 3;
}

// See https://doc.qt.io/qt-6/qfont.html#Weight-enum
QFont::Weight Utils::intToFontWeight(int value)
{
    value *= 10;
    if (value < 150) {
        return QFont::Thin;
    } else if (value < 250) {
        return QFont::ExtraLight;
    } else if (value < 350) {
        return QFont::Light;
    } else if (value < 450) {
        return QFont::Normal;
    } else if (value < 550) {
        return QFont::Medium;
    } else if (value < 650) {
        return QFont::DemiBold;
    } else if (value < 750) {
        return QFont::Bold;
    } else if (value < 850) {
        return QFont::ExtraBold;
    }
    return QFont::Black;
}

QString Utils::exportFileName(QString mindMapFileName, QString exportExtension)
{
    const auto formattedDateTime = QDateTime::currentDateTime().toString("_yyyy-MM-dd_hh-mm-ss");
    return mindMapFileName.replace(Constants::Application::fileExtension(), "") + formattedDateTime + exportExtension;
}

bool Utils::fileExists(QString filePath)
{
    QDir dir;
    return dir.exists(filePath);
}

int Utils::fontWeightToInt(QFont::Weight value)
{
    return value / 10;
}

int Utils::fontWeightToInt(int value)
{
    return value / 10;
}

QColor Utils::mixedColor(QColor color1, QColor color2, double balance)
{
    return { static_cast<int>(color1.red() * balance + color2.red() * (1 - balance)),
             static_cast<int>(color1.green() * balance + color2.green() * (1 - balance)),
             static_cast<int>(color1.blue() * balance + color2.blue() * (1 - balance)) };
}

double Utils::mouseWheelZoomFactor(int angleDeltaY, int speedPercent)
{
    // A notch of a standard mouse wheel is 15 degrees, i.e. 120 eighths of a degree
    const double notches = angleDeltaY / 120.0;
    const auto speed = std::clamp(speedPercent, Constants::View::minMouseWheelZoomSpeed(), Constants::View::maxMouseWheelZoomSpeed()) / 100.0;
    return std::pow(Constants::View::zoomSensitivity(), notches * speed);
}
