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

#ifndef UNDO_STACK_HPP
#define UNDO_STACK_HPP

#include <cstddef>
#include <list>
#include <optional>

//! Generic undo/redo stack of undo points of type T.
template<typename T>
class UndoStack
{
public:
    //! \param maxHistorySize The maximum size of the undo and redo stacks or 0 for "unlimited".
    //! The oldest undo points are dropped when the stack is full.
    explicit UndoStack(size_t maxHistorySize = 0)
      : m_maxHistorySize(maxHistorySize)
    {
    }

    void pushUndoPoint(T undoPoint)
    {
        push(m_undoStack, std::move(undoPoint));
    }

    void pushRedoPoint(T redoPoint)
    {
        push(m_redoStack, std::move(redoPoint));
    }

    void clear()
    {
        m_undoStack.clear();
        m_redoStack.clear();
    }

    void clearRedoStack()
    {
        m_redoStack.clear();
    }

    bool isUndoable() const
    {
        return !m_undoStack.empty();
    }

    //! \return The latest undo point or nullopt if not undoable.
    std::optional<T> undo()
    {
        return pop(m_undoStack);
    }

    bool isRedoable() const
    {
        return !m_redoStack.empty();
    }

    //! \return The latest redo point or nullopt if not redoable.
    std::optional<T> redo()
    {
        return pop(m_redoStack);
    }

    size_t undoStackSize() const
    {
        return m_undoStack.size();
    }

    size_t redoStackSize() const
    {
        return m_redoStack.size();
    }

private:
    void push(std::list<T> & stack, T point)
    {
        stack.push_back(std::move(point));
        if (m_maxHistorySize && stack.size() > m_maxHistorySize) {
            stack.pop_front();
        }
    }

    static std::optional<T> pop(std::list<T> & stack)
    {
        if (stack.empty()) {
            return {};
        }
        auto head = std::move(stack.back());
        stack.pop_back();
        return head;
    }

    std::list<T> m_undoStack;

    std::list<T> m_redoStack;

    size_t m_maxHistorySize;
};

#endif // UNDO_STACK_HPP
