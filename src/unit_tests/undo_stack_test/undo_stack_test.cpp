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
#include "undo_stack_test.hpp"

#include "../../domain/undo_stack.hpp"

#include <QTest>

#include <memory>

void UndoStackTest::test_initialState_shouldNotBeUndoableOrRedoable()
{
    UndoStack<int> undoStack;

    QVERIFY(!undoStack.isUndoable());
    QVERIFY(!undoStack.isRedoable());
    QCOMPARE(undoStack.undoStackSize(), size_t { 0 });
    QCOMPARE(undoStack.redoStackSize(), size_t { 0 });
}

void UndoStackTest::test_undo_shouldReturnUndoPointsInReverseOrder()
{
    UndoStack<int> undoStack;
    undoStack.pushUndoPoint(1);
    undoStack.pushUndoPoint(2);
    undoStack.pushUndoPoint(3);

    QCOMPARE(undoStack.undo(), std::optional<int> { 3 });
    QCOMPARE(undoStack.undo(), std::optional<int> { 2 });
    QCOMPARE(undoStack.undo(), std::optional<int> { 1 });
    QVERIFY(!undoStack.isUndoable());
}

void UndoStackTest::test_redo_shouldReturnRedoPointsInReverseOrder()
{
    UndoStack<int> undoStack;
    undoStack.pushRedoPoint(1);
    undoStack.pushRedoPoint(2);

    QVERIFY(undoStack.isRedoable());
    QCOMPARE(undoStack.redo(), std::optional<int> { 2 });
    QCOMPARE(undoStack.redo(), std::optional<int> { 1 });
    QVERIFY(!undoStack.isRedoable());
}

void UndoStackTest::test_undo_emptyStackShouldReturnNullopt()
{
    UndoStack<int> undoStack;

    QVERIFY(!undoStack.undo().has_value());
    QVERIFY(!undoStack.redo().has_value());
}

void UndoStackTest::test_clear_shouldClearBothStacks()
{
    UndoStack<int> undoStack;
    undoStack.pushUndoPoint(1);
    undoStack.pushRedoPoint(2);

    undoStack.clear();

    QVERIFY(!undoStack.isUndoable());
    QVERIFY(!undoStack.isRedoable());
}

void UndoStackTest::test_clearRedoStack_shouldKeepUndoStack()
{
    UndoStack<int> undoStack;
    undoStack.pushUndoPoint(1);
    undoStack.pushRedoPoint(2);

    undoStack.clearRedoStack();

    QVERIFY(undoStack.isUndoable());
    QVERIFY(!undoStack.isRedoable());
}

void UndoStackTest::test_maxHistorySize_shouldDropOldestUndoPoints()
{
    UndoStack<int> undoStack { 3 };
    for (int i = 1; i <= 5; i++) {
        undoStack.pushUndoPoint(i);
    }

    QCOMPARE(undoStack.undoStackSize(), size_t { 3 });
    QCOMPARE(undoStack.undo(), std::optional<int> { 5 });
    QCOMPARE(undoStack.undo(), std::optional<int> { 4 });
    QCOMPARE(undoStack.undo(), std::optional<int> { 3 });
    QVERIFY(!undoStack.isUndoable());
}

void UndoStackTest::test_maxHistorySize_shouldDropOldestRedoPoints()
{
    UndoStack<int> undoStack { 2 };
    for (int i = 1; i <= 4; i++) {
        undoStack.pushRedoPoint(i);
    }

    QCOMPARE(undoStack.redoStackSize(), size_t { 2 });
    QCOMPARE(undoStack.redo(), std::optional<int> { 4 });
    QCOMPARE(undoStack.redo(), std::optional<int> { 3 });
    QVERIFY(!undoStack.isRedoable());
}

void UndoStackTest::test_maxHistorySizeZero_shouldBeUnlimited()
{
    UndoStack<int> undoStack { 0 };
    for (int i = 0; i < 1000; i++) {
        undoStack.pushUndoPoint(i);
    }

    QCOMPARE(undoStack.undoStackSize(), size_t { 1000 });
}

void UndoStackTest::test_moveOnlyUndoPoints_shouldBeSupported()
{
    UndoStack<std::unique_ptr<int>> undoStack;
    undoStack.pushUndoPoint(std::make_unique<int>(42));

    const auto undoPoint = undoStack.undo();

    QVERIFY(undoPoint.has_value());
    QCOMPARE(**undoPoint, 42);
}

QTEST_GUILESS_MAIN(UndoStackTest)
