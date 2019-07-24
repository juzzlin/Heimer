// This file is part of Heimer.
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

#ifndef COPY_PASTE_HPP
#define COPY_PASTE_HPP

#include <memory>

class Grid;
class Mediator;
class Node;

class CopyPaste
{
public:

    CopyPaste(Mediator & mediator, Grid & grid);

    void copy(const Node & source);

    void paste();

    bool isEmpty() const;

private:

    Mediator & m_mediator;

    Grid & m_grid;

    std::shared_ptr<Node> m_copiedNode;
};

#endif // COPY_PASTE_HPP
