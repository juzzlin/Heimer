// This file is part of Hei-**   mer.
// Copyright (C) 2019 Jussi Lind <jussi.lind@iki.fi>
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

#include "copy_paste.hpp"

#include "grid.hpp"
#include "mediator.hpp"
#include "mouse_action.hpp"
#include "node.hpp"

CopyPaste::CopyPaste(Mediator & mediator, Grid & grid)
  : m_mediator(mediator)
  , m_grid(grid)
{
}

void CopyPaste::copy(const Node & source)
{
    m_copiedNode = std::make_shared<Node>(source);
}

void CopyPaste::paste()
{
    auto pastedNode(std::make_shared<Node>(*m_copiedNode));
    m_mediator.pasteNodeAt(*pastedNode, m_grid.snapToGrid(m_mediator.mouseAction().mappedPos() - m_mediator.mouseAction().sourcePosOnNode()));
}

bool CopyPaste::isEmpty() const
{
    return m_copiedNode == nullptr;
}
