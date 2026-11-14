// This file is part of Heimer.
// Copyright (C) 2026 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef CONNECTION_PULSE_HPP
#define CONNECTION_PULSE_HPP

#include <QGraphicsObject>
#include <QPropertyAnimation>

namespace SceneItems {

//! Animation shown at the end of an edge when it connects to a new location on a node:
//! a dot pops in, two rings ripple outwards and everything fades out.
class ConnectionPulse : public QGraphicsObject
{
    Q_OBJECT
    Q_PROPERTY(qreal progress READ progress WRITE setProgress)

public:
    explicit ConnectionPulse(QGraphicsItem * parentItem = nullptr);

    QRectF boundingRect() const override;

    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = nullptr) override;

    //! Starts the animation from the beginning.
    void start();

    //! Stops and hides the animation.
    void stop();

    void setColor(const QColor & color);

    //! \return The progress of the animation from 0 to 1.
    qreal progress() const;

    void setProgress(qreal progress);

private:
    QColor m_color;

    qreal m_progress = 1.0;

    QPropertyAnimation m_animation;
};

} // namespace SceneItems

#endif // CONNECTION_PULSE_HPP
