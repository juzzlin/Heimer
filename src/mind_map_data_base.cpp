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

#include "mind_map_data_base.hpp"

MindMapDataBase::MindMapDataBase(QString name)
    : m_name(name)
{
}

MindMapDataBase::MindMapDataBase(const MindMapDataBase & other)
    : m_name(other.m_name)
{
}

QString MindMapDataBase::name() const
{
    return m_name;
}

unsigned int MindMapDataBase::index() const
{
    return m_index;
}

void MindMapDataBase::setName(QString name)
{
    m_name = name;
}

void MindMapDataBase::setIndex(unsigned int index)
{
    m_index = index;
}

MindMapDataBase::~MindMapDataBase()
{
}
