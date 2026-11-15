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

#include "control_strategy_test.hpp"

#include "../../application/control_strategy.hpp"
#include "../../application/service_container.hpp"
#include "../../application/settings_proxy.hpp"

#include <QMouseEvent>
#include <QTest>

namespace {

QMouseEvent createPressEvent(Qt::MouseButton button, Qt::KeyboardModifiers modifiers = Qt::NoModifier)
{
    return { QEvent::MouseButtonPress, QPointF {}, QPointF {}, button, button, modifiers };
}

} // namespace

void ControlStrategyTest::cleanup()
{
    SC::instance().settingsProxy()->setInvertedControls(false);
}

void ControlStrategyTest::test_normalControls_shouldDragBackgroundWithoutModifier()
{
    const ControlStrategy dut;
    SC::instance().settingsProxy()->setInvertedControls(false);

    const auto leftEvent = createPressEvent(Qt::LeftButton);
    QVERIFY(dut.backgroundDragInitiated(leftEvent));
    QVERIFY(!dut.rubberBandInitiated(leftEvent));

    const auto rightEvent = createPressEvent(Qt::RightButton);
    QVERIFY(!dut.backgroundDragInitiated(rightEvent));
}

void ControlStrategyTest::test_normalControls_shouldStartRubberBandWithModifier()
{
    const ControlStrategy dut;
    SC::instance().settingsProxy()->setInvertedControls(false);

    for (const auto & modifier : { Qt::ControlModifier, Qt::ShiftModifier }) {
        const auto event = createPressEvent(Qt::LeftButton, modifier);
        QVERIFY(dut.rubberBandInitiated(event));
        QVERIFY(!dut.backgroundDragInitiated(event));
    }

    // Other modifiers don't count
    const auto event = createPressEvent(Qt::LeftButton, Qt::AltModifier);
    QVERIFY(!dut.rubberBandInitiated(event));
    QVERIFY(dut.backgroundDragInitiated(event));
}

void ControlStrategyTest::test_invertedControls_shouldSwapBackgroundDragAndRubberBand()
{
    const ControlStrategy dut;
    SC::instance().settingsProxy()->setInvertedControls(true);

    const auto event = createPressEvent(Qt::LeftButton);
    QVERIFY(dut.rubberBandInitiated(event));
    QVERIFY(!dut.backgroundDragInitiated(event));

    const auto modifierEvent = createPressEvent(Qt::LeftButton, Qt::ControlModifier);
    QVERIFY(dut.backgroundDragInitiated(modifierEvent));
    QVERIFY(!dut.rubberBandInitiated(modifierEvent));
}

void ControlStrategyTest::test_middleButton_shouldAlwaysStartRubberBand()
{
    const ControlStrategy dut;

    for (const auto & invertedControls : { false, true }) {
        SC::instance().settingsProxy()->setInvertedControls(invertedControls);
        for (const auto & modifiers : { Qt::KeyboardModifiers { Qt::NoModifier }, Qt::KeyboardModifiers { Qt::ControlModifier } }) {
            const auto event = createPressEvent(Qt::MiddleButton, modifiers);
            QVERIFY(dut.rubberBandInitiated(event));
            QVERIFY(!dut.backgroundDragInitiated(event));
        }
    }
}

void ControlStrategyTest::test_buttonClicks_shouldBeDetected()
{
    const ControlStrategy dut;

    const auto leftEvent = createPressEvent(Qt::LeftButton);
    QVERIFY(dut.primaryButtonClicked(leftEvent));
    QVERIFY(!dut.secondaryButtonClicked(leftEvent));

    const auto rightEvent = createPressEvent(Qt::RightButton);
    QVERIFY(!dut.primaryButtonClicked(rightEvent));
    QVERIFY(dut.secondaryButtonClicked(rightEvent));
}

QTEST_GUILESS_MAIN(ControlStrategyTest)
