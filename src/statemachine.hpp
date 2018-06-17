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

#ifndef STATEMACHINE_HPP
#define STATEMACHINE_HPP

class Mediator;

class StateMachine
{
public:

    enum class State
    {
        CloseWindow,
        Edit,
        Init,
        InitializeNewMindMap,
        ShowOpenDialog,
        SaveMindMap,
        ShowSaveAsDialog
    };

    enum class Action
    {
        MainWindowInitialized,
        MindMapOpened,
        MindMapSaved,
        MindMapSavedAs,
        MindMapSaveFailed,
        MindMapSaveAsFailed,
        NewMindMapInitialized,
        NewSelected,
        OpenSelected,
        QuitSelected,
        SaveSelected,
        SaveAsSelected,
        UndoSelected,
        RedoSelected
    };

    StateMachine();

    State calculateState(StateMachine::Action action, Mediator & mediator);

private:

    State m_state = State::Init;
};

#endif // STATEMACHINE_HPP
