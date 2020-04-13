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

#ifndef UNDOSTACK_HPP
#define UNDOSTACK_HPP

#include "mind_map_data.hpp"

#include <list>

class UndoStack
{
public:
    UndoStack(int maxHistorySize = -1);

    void pushUndoPoint(const MindMapData & mindMapData);

    void pushRedoPoint(const MindMapData & mindMapData);

    void clear();

    bool isUndoable() const;

    std::unique_ptr<MindMapData> undo();

    bool isRedoable() const;

    std::unique_ptr<MindMapData> redo();

private:
    using MindMapDataVector = std::list<std::unique_ptr<MindMapData>>;

    MindMapDataVector m_undoStack;

    MindMapDataVector m_redoStack;

    int m_maxHistorySize;
};

#endif // UNDOSTACK_HPP
