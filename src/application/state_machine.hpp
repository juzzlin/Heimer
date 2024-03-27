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

#ifndef STATE_MACHINE_HPP
#define STATE_MACHINE_HPP

#include <QObject>

#include <memory>

class ApplicationService;

//! State machine mainly for application logic involving user actions (show dialog, load mind map, etc ... )
class StateMachine : public QObject
{
    Q_OBJECT

public:
    StateMachine(QObject * parent = nullptr);

    enum class State
    {
        Edit,
        Exit,
        Init,
        InitializeNewMindMap,
        OpenDrop,
        OpenRecent,
        Save,
        ShowBackgroundColorDialog,
        ShowEdgeColorDialog,
        ShowGridColorDialog,
        ShowImageFileDialog,
        ShowLayoutOptimizationDialog,
        ShowNodeColorDialog,
        ShowNotSavedDialog,
        ShowOpenDialog,
        ShowPngExportDialog,
        ShowSaveAsDialog,
        ShowSvgExportDialog,
        ShowTextColorDialog,
        TryCloseWindow
    };

    enum class Action
    {
        BackgroundColorChangeRequested,
        BackgroundColorChanged,
        DropFileSelected,
        EdgeColorChangeRequested,
        EdgeColorChanged,
        GridColorChangeRequested,
        GridColorChanged,
        ImageAttachmentRequested,
        ImageLoadFailed,
        LayoutOptimizationRequested,
        LayoutOptimized,
        MainWindowInitialized,
        MindMapOpened,
        MindMapSaveAsCanceled,
        MindMapSaveAsFailed,
        MindMapSaveFailed,
        MindMapSaved,
        MindMapSavedAs,
        NewMindMapInitialized,
        NewSelected,
        NodeColorChangeRequested,
        NodeColorChanged,
        NotSavedDialogAccepted,
        NotSavedDialogCanceled,
        NotSavedDialogDiscarded,
        OpenSelected,
        OpeningMindMapCanceled,
        OpeningMindMapFailed,
        PngExportSelected,
        PngExported,
        QuitSelected,
        RecentFileSelected,
        RedoSelected,
        SaveAsSelected,
        SaveSelected,
        SvgExportSelected,
        SvgExported,
        TextColorChangeRequested,
        TextColorChanged,
        UndoSelected
    };

    enum class QuitType
    {
        None,
        New,
        Open,
        OpenRecent,
        OpenDrop,
        Close
    };

    StateMachine();

    void calculateState(StateMachine::Action action);

signals:

    void stateChanged(StateMachine::State state);

private:
    State m_state = State::Init;

    QuitType m_quitType = QuitType::None;
};

#endif // STATE_MACHINE_HPP
