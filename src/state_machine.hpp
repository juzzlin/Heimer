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

class Mediator;

class StateMachine : public QObject
{
    Q_OBJECT

public:
    enum class State
    {
        Edit,
        Exit,
        Init,
        InitializeNewMindMap,
        OpenRecent,
        Save,
        ShowBackgroundColorDialog,
        ShowEdgeColorDialog,
        ShowImageFileDialog,
        ShowLayoutOptimizationDialog,
        ShowNotSavedDialog,
        ShowOpenDialog,
        ShowPngExportDialog,
        ShowSaveAsDialog,
        ShowSvgExportDialog,
        TryCloseWindow
    };

    enum class Action
    {
        BackgroundColorChanged,
        BackgroundColorChangeRequested,
        EdgeColorChanged,
        EdgeColorChangeRequested,
        ImageAttachmentRequested,
        ImageLoadFailed,
        LayoutOptimizationRequested,
        LayoutOptimized,
        MainWindowInitialized,
        MindMapOpened,
        MindMapSaved,
        MindMapSavedAs,
        MindMapSaveFailed,
        MindMapSaveAsCanceled,
        MindMapSaveAsFailed,
        NewMindMapInitialized,
        NewSelected,
        NotSavedDialogAccepted,
        NotSavedDialogCanceled,
        NotSavedDialogDiscarded,
        OpeningMindMapCanceled,
        OpeningMindMapFailed,
        OpenSelected,
        PngExported,
        PngExportSelected,
        QuitSelected,
        RecentFileSelected,
        RedoSelected,
        SaveAsSelected,
        SaveSelected,
        SvgExported,
        SvgExportSelected,
        UndoSelected
    };

    enum class QuitType
    {
        None,
        New,
        Open,
        OpenRecent,
        Close
    };

    StateMachine();

    void calculateState(StateMachine::Action action);

    void setMediator(std::shared_ptr<Mediator> mediator);

signals:

    void stateChanged(StateMachine::State state);

private:
    State m_state = State::Init;

    QuitType m_quitType = QuitType::None;

    std::shared_ptr<Mediator> m_mediator;
};

#endif // STATE_MACHINE_HPP
