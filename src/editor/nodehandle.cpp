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

#include "layers.hpp"

#include <QPainter>
#include <QPen>

NodeHandle::NodeHandle(int radius)
    : m_radius(radius)
    , m_sizeAnimation(this, "scale")
    , m_size(QSize(m_radius * 2, m_radius * 2))
{
    m_sizeAnimation.setDuration(125);

    QGraphicsItem::setVisible(false);

    setZValue(static_cast<int>(Layers::NodeHandle));
}

QRectF NodeHandle::boundingRect() const
{
    const int margin = 1;
    return QRectF(-m_size.width() / 2 - margin, - m_size.height() / 2 - margin, m_size.width() + margin * 2, m_size.height() + margin * 2);
}

void NodeHandle::paint(QPainter * painter,
    const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);

    painter->save();
    painter->drawPixmap(-m_size.width() / 2, -m_size.height() / 2, QPixmap(":/add.png").scaled(m_size.width(), m_size.height()));
    painter->restore();
}

void NodeHandle::setVisible(bool visible)
{
    if (visible)
    {
        QGraphicsItem::setVisible(true);
        m_sizeAnimation.setStartValue(0.0f);
        m_sizeAnimation.setEndValue(1.0f);
    }
    else if (!visible)
    {
        m_sizeAnimation.setStartValue(scale());
        m_sizeAnimation.setEndValue(0.0f);
    }

    m_sizeAnimation.start();
}

NodeHandle::~NodeHandle()
{
}
