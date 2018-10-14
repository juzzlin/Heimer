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

#include "nodehandle.hpp"

#include "layers.hpp"

#include <QPainter>
#include <QPen>

NodeHandle::NodeHandle(Node & parentNode, NodeHandle::Role role, int radius)
    : m_parentNode(parentNode)
    , m_role(role)
    , m_radius(radius)
    , m_sizeAnimation(this, "scale")
    , m_opacityAnimation(this, "opacity")
    , m_size(QSize(m_radius * 2, m_radius * 2))
{
    const size_t duration = 100;
    m_sizeAnimation.setDuration(duration);
    m_opacityAnimation.setDuration(duration);

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

    static std::map<Role, QPixmap> pixmapMap = {
        {Role::Add, QPixmap(":/add.png")},
        {Role::Color, QPixmap(":/colors.png")},
        {Role::TextColor, QPixmap(":/colorsText.png")}
    };

    painter->save();
    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    painter->drawPixmap(-m_size.width() / 2, -m_size.height() / 2, m_size.width(), m_size.height(), pixmapMap[role()]);
    painter->restore();
}

Node & NodeHandle::parentNode() const
{
    return m_parentNode;
}

NodeHandle::Role NodeHandle::role() const
{
    return m_role;
}

void NodeHandle::setVisible(bool visible)
{
    if (visible)
    {
        if (!m_visible)
        {
            QGraphicsItem::setVisible(true);

            m_visible = true;
            m_sizeAnimation.setStartValue(0.0f);
            m_sizeAnimation.setEndValue(1.0f);
            m_sizeAnimation.stop();
            m_sizeAnimation.start();

            m_opacityAnimation.setStartValue(0.0f);
            m_opacityAnimation.setEndValue(0.8f);
            m_opacityAnimation.stop();
            m_opacityAnimation.start();
        }
    }
    else
    {
        if (m_visible)
        {
            m_visible = false;
            m_sizeAnimation.setStartValue(scale());
            m_sizeAnimation.setEndValue(0.0f);
            m_sizeAnimation.stop();
            m_sizeAnimation.start();

            m_opacityAnimation.setStartValue(opacity());
            m_opacityAnimation.setEndValue(0.0f);
            m_opacityAnimation.stop();
            m_opacityAnimation.start();
        }
    }
}

NodeHandle::~NodeHandle()
{
}
