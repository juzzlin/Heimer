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

#include "undo_stack.hpp"

#include "mind_map_data.hpp"

UndoStack::UndoStack(size_t maxHistorySize)
  : m_maxHistorySize(maxHistorySize)
{
}

void UndoStack::pushUndoPoint(MindMapDataCR mindMapData)
{
    m_undoStack.push_back(std::make_unique<MindMapData>(mindMapData));

    if (m_undoStack.size() > m_maxHistorySize && m_maxHistorySize) {
        m_undoStack.pop_front();
    }
}

void UndoStack::pushRedoPoint(MindMapDataCR mindMapData)
{
    m_redoStack.push_back(std::make_unique<MindMapData>(mindMapData));

    if (m_redoStack.size() > m_maxHistorySize && m_maxHistorySize) {
        m_redoStack.pop_front();
    }
}

void UndoStack::clear()
{
    m_undoStack.clear();
    m_redoStack.clear();
}

void UndoStack::clearRedoStack()
{
    m_redoStack.clear();
}

bool UndoStack::isUndoable() const
{
    return !m_undoStack.empty();
}

MindMapDataU UndoStack::undo()
{
    if (isUndoable()) {
        auto head = std::move(m_undoStack.back());
        m_undoStack.pop_back();
        return head;
    }

    return {};
}

bool UndoStack::isRedoable() const
{
    return !m_redoStack.empty();
}

MindMapDataU UndoStack::redo()
{
    if (isRedoable()) {
        auto head = std::move(m_redoStack.back());
        m_redoStack.pop_back();
        return head;
    }

    return {};
}
