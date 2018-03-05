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

#ifndef NODEHANDLE_HPP
#define NODEHANDLE_HPP

#include <QGraphicsItem>
#include <QPropertyAnimation>

class Node;

class NodeHandle : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_PROPERTY(qreal scale READ scale WRITE setScale)
    Q_INTERFACES(QGraphicsItem)

public:

    explicit NodeHandle(int radius);

    virtual ~NodeHandle();

    virtual QRectF boundingRect () const override;

    virtual void paint(QPainter * painter,
        const QStyleOptionGraphicsItem * option, QWidget * widget = nullptr) override;

    void setVisible(bool visible);

private:

    int m_radius;

    QPropertyAnimation m_sizeAnimation;

    QSize m_size;
};

#endif // NODEHANDLE_HPP
