// This file is part of Dementia.
// Copyright (C) 2018 Jussi Lind <jussi.lind@iki.fi>
//
// Dementia is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dementia is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dementia. If not, see <http://www.gnu.org/licenses/>.

#include "nodehandle.hpp"

#include <QPainter>
#include <QPen>

NodeHandle::NodeHandle(int radius)
    : m_radius(radius)
    , m_sizeAnimation(this, "size")
{
    m_sizeAnimation.setDuration(125);

    QGraphicsItem::setVisible(false);
}

QRectF NodeHandle::boundingRect() const
{
    const int margin = 1;
    return QRectF(-size().width() / 2 - margin, - size().height() / 2 - margin, size().width() + margin * 2, size().height() + margin * 2);
}

void NodeHandle::paint(QPainter * painter,
    const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);

    painter->save();
    painter->drawPixmap(-size().width() / 2, -size().height() / 2, QPixmap(":/add.png").scaled(size().width(), size().height()));
    painter->restore();
}

QSize NodeHandle::size() const
{
    return m_size;
}

void NodeHandle::setSize(const QSize & size)
{
    prepareGeometryChange();

    m_size = size;
}

void NodeHandle::setVisible(bool visible)
{
    static const QSize away(0, 0);
    static const QSize full(m_radius * 2, m_radius * 2);

    if (visible)
    {
        QGraphicsItem::setVisible(true);
        m_sizeAnimation.setStartValue(away);
        m_sizeAnimation.setEndValue(full);
    }
    else
    {
        m_sizeAnimation.setStartValue(full);
        m_sizeAnimation.setEndValue(away);
    }

    m_sizeAnimation.start();
}

NodeHandle::~NodeHandle()
{
}
