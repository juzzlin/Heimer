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

#ifndef STATE_MACHINE_TEST_HPP
#define STATE_MACHINE_TEST_HPP

#include <QObject>

class StateMachineTest : public QObject
{
    Q_OBJECT

private slots:

    void test_calculateState_shouldShowDialogOnRequest_data();
    void test_calculateState_shouldShowDialogOnRequest();

    void test_calculateState_shouldReturnToEdit_data();
    void test_calculateState_shouldReturnToEdit();

    void test_calculateState_unmodifiedMindMapShouldNotAskToSave_data();
    void test_calculateState_unmodifiedMindMapShouldNotAskToSave();

    void test_calculateState_modifiedMindMapShouldAskToSave_data();
    void test_calculateState_modifiedMindMapShouldAskToSave();

    void test_calculateState_discardedChangesShouldContinueOriginalAction_data();
    void test_calculateState_discardedChangesShouldContinueOriginalAction();

    void test_calculateState_savedChangesShouldContinueOriginalAction_data();
    void test_calculateState_savedChangesShouldContinueOriginalAction();

    void test_calculateState_saveShouldAskFileNameIfNeeded();

    void test_calculateState_canceledDialogShouldForgetOriginalAction();

    void test_calculateState_shouldEmitStateChanged();
};

#endif // STATE_MACHINE_TEST_HPP
