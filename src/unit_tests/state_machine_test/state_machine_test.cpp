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

#include "state_machine_test.hpp"

#include "../../application/state_machine.hpp"

#include <QSignalSpy>
#include <QTest>

using Action = StateMachine::Action;
using State = StateMachine::State;

namespace {

//! Fake mind map status for the state machine
struct MindMapStatus
{
    bool isModified = false;
    bool canBeSaved = false;
};

class StateRecorder
{
public:
    explicit StateRecorder(MindMapStatus & status)
      : m_stateMachine {
          [&status] { return status.isModified; },
          [&status] { return status.canBeSaved; }
      }
    {
        QObject::connect(&m_stateMachine, &StateMachine::stateChanged, [this](State state) {
            m_state = state;
        });
    }

    State calculateState(Action action)
    {
        m_stateMachine.calculateState(action);
        return m_state;
    }

    StateMachine & stateMachine()
    {
        return m_stateMachine;
    }

private:
    StateMachine m_stateMachine;
    State m_state = State::Init;
};

void addRow(const char * name, Action action, State state)
{
    QTest::newRow(name) << static_cast<int>(action) << static_cast<int>(state);
}

void addColumns()
{
    QTest::addColumn<int>("action");
    QTest::addColumn<int>("expectedState");
}

//! Actions that might lose unsaved changes and the states they finally lead to
void addActionsThatCheckUnsavedChanges()
{
    addColumns();
    addRow("New", Action::NewSelected, State::InitializeNewMindMap);
    addRow("Open", Action::OpenSelected, State::ShowOpenDialog);
    addRow("Quit", Action::QuitSelected, State::Exit);
    addRow("Recent file", Action::RecentFileSelected, State::OpenRecent);
    addRow("Drop file", Action::DropFileSelected, State::OpenDrop);
}

} // namespace

void StateMachineTest::test_calculateState_shouldShowDialogOnRequest_data()
{
    addColumns();
    addRow("Background color", Action::BackgroundColorChangeRequested, State::ShowBackgroundColorDialog);
    addRow("Edge color", Action::EdgeColorChangeRequested, State::ShowEdgeColorDialog);
    addRow("Edge label color", Action::EdgeLabelColorChangeRequested, State::ShowEdgeLabelColorDialog);
    addRow("Edge label text color", Action::EdgeLabelTextColorChangeRequested, State::ShowEdgeLabelTextColorDialog);
    addRow("Grid color", Action::GridColorChangeRequested, State::ShowGridColorDialog);
    addRow("Image", Action::ImageAttachmentRequested, State::ShowImageFileDialog);
    addRow("Layout optimization", Action::LayoutOptimizationRequested, State::ShowLayoutOptimizationDialog);
    addRow("Node color", Action::NodeColorChangeRequested, State::ShowNodeColorDialog);
    addRow("PNG export", Action::PngExportSelected, State::ShowPngExportDialog);
    addRow("Save as", Action::SaveAsSelected, State::ShowSaveAsDialog);
    addRow("SVG export", Action::SvgExportSelected, State::ShowSvgExportDialog);
    addRow("Text color", Action::TextColorChangeRequested, State::ShowTextColorDialog);
    addRow("Main window initialized", Action::MainWindowInitialized, State::InitializeNewMindMap);
}

void StateMachineTest::test_calculateState_shouldShowDialogOnRequest()
{
    QFETCH(int, action);
    QFETCH(int, expectedState);

    MindMapStatus status;
    StateRecorder dut { status };

    QCOMPARE(static_cast<int>(dut.calculateState(static_cast<Action>(action))), expectedState);
}

void StateMachineTest::test_calculateState_shouldReturnToEdit_data()
{
    addColumns();
    addRow("Background color changed", Action::BackgroundColorChanged, State::Edit);
    addRow("Edge color changed", Action::EdgeColorChanged, State::Edit);
    addRow("Edge label color changed", Action::EdgeLabelColorChanged, State::Edit);
    addRow("Edge label text color changed", Action::EdgeLabelTextColorChanged, State::Edit);
    addRow("Grid color changed", Action::GridColorChanged, State::Edit);
    addRow("Image load failed", Action::ImageLoadFailed, State::Edit);
    addRow("Layout optimized", Action::LayoutOptimized, State::Edit);
    addRow("Mind map opened", Action::MindMapOpened, State::Edit);
    addRow("Save failed", Action::MindMapSaveFailed, State::Edit);
    addRow("Save as canceled", Action::MindMapSaveAsCanceled, State::Edit);
    addRow("Save as failed", Action::MindMapSaveAsFailed, State::Edit);
    addRow("New mind map initialized", Action::NewMindMapInitialized, State::Edit);
    addRow("Node color changed", Action::NodeColorChanged, State::Edit);
    addRow("Not saved dialog canceled", Action::NotSavedDialogCanceled, State::Edit);
    addRow("Opening canceled", Action::OpeningMindMapCanceled, State::Edit);
    addRow("Opening failed", Action::OpeningMindMapFailed, State::Edit);
    addRow("PNG exported", Action::PngExported, State::Edit);
    addRow("SVG exported", Action::SvgExported, State::Edit);
    addRow("Text color changed", Action::TextColorChanged, State::Edit);
}

void StateMachineTest::test_calculateState_shouldReturnToEdit()
{
    QFETCH(int, action);
    QFETCH(int, expectedState);

    MindMapStatus status;
    StateRecorder dut { status };

    QCOMPARE(static_cast<int>(dut.calculateState(static_cast<Action>(action))), expectedState);
}

void StateMachineTest::test_calculateState_unmodifiedMindMapShouldNotAskToSave_data()
{
    addActionsThatCheckUnsavedChanges();
}

void StateMachineTest::test_calculateState_unmodifiedMindMapShouldNotAskToSave()
{
    QFETCH(int, action);
    QFETCH(int, expectedState);

    MindMapStatus status;
    StateRecorder dut { status };

    QCOMPARE(static_cast<int>(dut.calculateState(static_cast<Action>(action))), expectedState);
}

void StateMachineTest::test_calculateState_modifiedMindMapShouldAskToSave_data()
{
    addActionsThatCheckUnsavedChanges();
}

void StateMachineTest::test_calculateState_modifiedMindMapShouldAskToSave()
{
    QFETCH(int, action);

    MindMapStatus status;
    status.isModified = true;
    StateRecorder dut { status };

    QCOMPARE(dut.calculateState(static_cast<Action>(action)), State::ShowNotSavedDialog);
}

void StateMachineTest::test_calculateState_discardedChangesShouldContinueOriginalAction_data()
{
    addActionsThatCheckUnsavedChanges();
}

void StateMachineTest::test_calculateState_discardedChangesShouldContinueOriginalAction()
{
    QFETCH(int, action);
    QFETCH(int, expectedState);

    MindMapStatus status;
    status.isModified = true;
    StateRecorder dut { status };

    QCOMPARE(dut.calculateState(static_cast<Action>(action)), State::ShowNotSavedDialog);
    QCOMPARE(static_cast<int>(dut.calculateState(Action::NotSavedDialogDiscarded)), expectedState);
}

void StateMachineTest::test_calculateState_savedChangesShouldContinueOriginalAction_data()
{
    addActionsThatCheckUnsavedChanges();
}

void StateMachineTest::test_calculateState_savedChangesShouldContinueOriginalAction()
{
    QFETCH(int, action);
    QFETCH(int, expectedState);

    MindMapStatus status;
    status.isModified = true;
    status.canBeSaved = true;
    StateRecorder dut { status };

    QCOMPARE(dut.calculateState(static_cast<Action>(action)), State::ShowNotSavedDialog);
    QCOMPARE(dut.calculateState(Action::NotSavedDialogAccepted), State::Save);
    QCOMPARE(static_cast<int>(dut.calculateState(Action::MindMapSaved)), expectedState);
}

void StateMachineTest::test_calculateState_saveShouldAskFileNameIfNeeded()
{
    MindMapStatus status;
    StateRecorder dut { status };

    // A new mind map has no file name yet
    QCOMPARE(dut.calculateState(Action::SaveSelected), State::ShowSaveAsDialog);
    QCOMPARE(dut.calculateState(Action::MindMapSavedAs), State::Edit);

    status.canBeSaved = true;
    QCOMPARE(dut.calculateState(Action::SaveSelected), State::Save);
    QCOMPARE(dut.calculateState(Action::MindMapSaved), State::Edit);

    // Unsaved new mind map when quitting
    status.isModified = true;
    status.canBeSaved = false;
    QCOMPARE(dut.calculateState(Action::QuitSelected), State::ShowNotSavedDialog);
    QCOMPARE(dut.calculateState(Action::NotSavedDialogAccepted), State::ShowSaveAsDialog);
    QCOMPARE(dut.calculateState(Action::MindMapSavedAs), State::Exit);
}

void StateMachineTest::test_calculateState_canceledDialogShouldForgetOriginalAction()
{
    MindMapStatus status;
    status.isModified = true;
    StateRecorder dut { status };

    QCOMPARE(dut.calculateState(Action::QuitSelected), State::ShowNotSavedDialog);
    QCOMPARE(dut.calculateState(Action::NotSavedDialogCanceled), State::Edit);

    // A later save must not quit the application
    status.canBeSaved = true;
    QCOMPARE(dut.calculateState(Action::SaveSelected), State::Save);
    QCOMPARE(dut.calculateState(Action::MindMapSaved), State::Edit);

    // Canceling the file name dialog must not quit either
    status.canBeSaved = false;
    QCOMPARE(dut.calculateState(Action::QuitSelected), State::ShowNotSavedDialog);
    QCOMPARE(dut.calculateState(Action::NotSavedDialogAccepted), State::ShowSaveAsDialog);
    QCOMPARE(dut.calculateState(Action::MindMapSaveAsCanceled), State::Edit);
    QCOMPARE(dut.calculateState(Action::MindMapSavedAs), State::Edit);
}

void StateMachineTest::test_calculateState_shouldEmitStateChanged()
{
    MindMapStatus status;
    StateRecorder recorder { status };
    QSignalSpy spy { &recorder.stateMachine(), &StateMachine::stateChanged };

    recorder.stateMachine().calculateState(Action::NodeColorChangeRequested);
    recorder.stateMachine().calculateState(Action::NodeColorChanged);

    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.at(0).at(0).value<State>(), State::ShowNodeColorDialog);
    QCOMPARE(spy.at(1).at(0).value<State>(), State::Edit);
}

QTEST_GUILESS_MAIN(StateMachineTest)
