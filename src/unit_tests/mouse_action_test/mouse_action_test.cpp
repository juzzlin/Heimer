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

#include "mouse_action_test.hpp"

#include "../../common/test_mode.hpp"
#include "../../view/mouse_action.hpp"
#include "../../view/scene_items/node.hpp"

#include <QTest>

using Action = MouseAction::Action;

MouseActionTest::MouseActionTest()
{
    TestMode::setEnabled(true);
}

void MouseActionTest::test_initialState_shouldBeEmpty()
{
    const MouseAction dut;

    QCOMPARE(dut.action(), Action::None);
    QCOMPARE(dut.sourceNode(), nullptr);
    QCOMPARE(dut.sourcePos(), QPointF {});
    QCOMPARE(dut.sourcePosOnNode(), QPointF {});
}

void MouseActionTest::test_setSourceNode_shouldSetAction()
{
    MouseAction dut;
    SceneItems::Node node;

    dut.setSourceNode(&node, Action::MoveNode);
    QCOMPARE(dut.sourceNode(), &node);
    QCOMPARE(dut.action(), Action::MoveNode);

    dut.setSourceNode(&node, Action::CreateOrConnectNode);
    QCOMPARE(dut.action(), Action::CreateOrConnectNode);
}

void MouseActionTest::test_setRubberBandOrigin_shouldStartRubberBand()
{
    MouseAction dut;

    dut.setRubberBandOrigin({ 10, 20 });

    QCOMPARE(dut.rubberBandOrigin(), (QPointF { 10, 20 }));
    QCOMPARE(dut.action(), Action::RubberBand);
}

void MouseActionTest::test_positions_shouldBeStored()
{
    MouseAction dut;

    dut.setSourcePos({ 1, 2 });
    dut.setSourcePosOnNode({ 3, 4 });
    dut.setMappedPos({ 5, 6 });
    dut.setClickedScenePos({ 7, 8 });

    QCOMPARE(dut.sourcePos(), (QPointF { 1, 2 }));
    QCOMPARE(dut.sourcePosOnNode(), (QPointF { 3, 4 }));
    QCOMPARE(dut.mappedPos(), (QPointF { 5, 6 }));
    QCOMPARE(dut.clickedScenePos(), (QPointF { 7, 8 }));
    // Setting the positions doesn't start any action
    QCOMPARE(dut.action(), Action::None);
}

void MouseActionTest::test_clear_shouldResetSourceAndAction()
{
    MouseAction dut;
    SceneItems::Node node;
    dut.setSourceNode(&node, Action::MoveNode);
    dut.setSourcePos({ 1, 2 });
    dut.setSourcePosOnNode({ 3, 4 });

    dut.clear();

    QCOMPARE(dut.action(), Action::None);
    QCOMPARE(dut.sourceNode(), nullptr);
    QCOMPARE(dut.sourcePos(), QPointF {});
    QCOMPARE(dut.sourcePosOnNode(), QPointF {});
}

QTEST_GUILESS_MAIN(MouseActionTest)
