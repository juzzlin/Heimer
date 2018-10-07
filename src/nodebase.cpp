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

#include "nodebase.hpp"

NodeBase::NodeBase()
{
}

QSizeF NodeBase::size() const
{
    return m_size;
}

void NodeBase::setSize(QSizeF size)
{
    m_size = size;
}

QPointF NodeBase::location() const
{
    return m_location;
}

void NodeBase::setLocation(QPointF newLocation)
{
    m_location = newLocation;
}

QRectF NodeBase::placementBoundingRect() const
{
    return {-size().width() / 2, -size().height() / 2, size().width(), size().height()};
}

int NodeBase::index() const
{
    return m_index;
}

void NodeBase::setIndex(int index)
{
    m_index = index;
}

QString NodeBase::text() const
{
    return m_text;
}

void NodeBase::setText(const QString & text)
{
    m_text = text;
}

QColor NodeBase::color() const
{
    return m_color;
}

void NodeBase::setColor(const QColor & color)
{
    m_color = color;
}

QColor NodeBase::textColor() const
{
    return m_textColor;
}

void NodeBase::setTextColor(const QColor & color)
{
    m_textColor = color;
}

NodeBase::~NodeBase()
{
}
