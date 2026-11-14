// This file is part of Heimer.
// Copyright (C) 2026 Jussi Lind <jussi.lind@iki.fi>
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

#include "utils_test.hpp"

#include "../../common/constants.hpp"
#include "../../common/utils.hpp"

#include <QTest>

#include <cmath>

void UtilsTest::test_mouseWheelZoomFactor_shouldZoomInAndOutSymmetrically()
{
    const auto zoomIn = Utils::mouseWheelZoomFactor(120, 100);
    const auto zoomOut = Utils::mouseWheelZoomFactor(-120, 100);

    QCOMPARE(zoomIn, Constants::View::zoomSensitivity());
    QCOMPARE(zoomIn * zoomOut, 1.0);
    QCOMPARE(Utils::mouseWheelZoomFactor(0, 100), 1.0);
}

void UtilsTest::test_mouseWheelZoomFactor_shouldBeProportionalToRotation()
{
    // A high-resolution wheel sends many small steps for one notch
    double highResolutionZoom = 1.0;
    for (int i = 0; i < 8; i++) {
        highResolutionZoom *= Utils::mouseWheelZoomFactor(15, 100);
    }

    QVERIFY(std::abs(highResolutionZoom - Utils::mouseWheelZoomFactor(120, 100)) < 1e-9);
}

void UtilsTest::test_mouseWheelZoomFactor_shouldScaleWithSpeed()
{
    const auto slow = Utils::mouseWheelZoomFactor(120, 50);
    const auto fast = Utils::mouseWheelZoomFactor(120, 200);

    QVERIFY(std::abs(slow * slow - Constants::View::zoomSensitivity()) < 1e-9);
    QVERIFY(std::abs(fast - std::pow(Constants::View::zoomSensitivity(), 2)) < 1e-9);
    // Out of range speeds are clamped
    QCOMPARE(Utils::mouseWheelZoomFactor(120, 0), Utils::mouseWheelZoomFactor(120, Constants::View::minMouseWheelZoomSpeed()));
}

QTEST_GUILESS_MAIN(UtilsTest)
