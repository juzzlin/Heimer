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
#ifndef UNDO_STACK_TEST_HPP
#define UNDO_STACK_TEST_HPP

#include <QObject>

class UndoStackTest : public QObject
{
    Q_OBJECT

private slots:

    void test_initialState_shouldNotBeUndoableOrRedoable();

    void test_undo_shouldReturnUndoPointsInReverseOrder();

    void test_redo_shouldReturnRedoPointsInReverseOrder();

    void test_undo_emptyStackShouldReturnNullopt();

    void test_clear_shouldClearBothStacks();

    void test_clearRedoStack_shouldKeepUndoStack();

    void test_maxHistorySize_shouldDropOldestUndoPoints();

    void test_maxHistorySize_shouldDropOldestRedoPoints();

    void test_maxHistorySizeZero_shouldBeUnlimited();

    void test_moveOnlyUndoPoints_shouldBeSupported();
};

#endif // UNDO_STACK_TEST_HPP
