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

#include "connection_pulse.hpp"

#include <QPainter>

#include <algorithm>
#include <cmath>

namespace SceneItems {

static const double coreRadius = 4.5;

static const double maxRingRadius = 22;

static const int durationMs = 900;

static double easeOutCubic(double t)
{
    return 1 - std::pow(1 - t, 3);
}

//! Overshoots a bit before settling to 1
static double easeOutBack(double t)
{
    const double c1 = 1.70158;
    const double c3 = c1 + 1;
    return 1 + c3 * std::pow(t - 1, 3) + c1 * std::pow(t - 1, 2);
}

ConnectionPulse::ConnectionPulse(QGraphicsItem * parentItem)
  : QGraphicsObject { parentItem }
  , m_animation { this, "progress" }
{
    m_animation.setDuration(durationMs);
    m_animation.setStartValue(0.0);
    m_animation.setEndValue(1.0);

    connect(&m_animation, &QPropertyAnimation::finished, this, [this] {
        setVisible(false);
    });

    setVisible(false);
}

QRectF ConnectionPulse::boundingRect() const
{
    const auto extent = maxRingRadius + 2;
    return { -extent, -extent, extent * 2, extent * 2 };
}

void ConnectionPulse::paint(QPainter * painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    // Two rings ripple outwards, the second one a bit later and fainter
    painter->setBrush(Qt::NoBrush);
    for (int ring = 0; ring < 2; ring++) {
        const auto delay = 0.22 * ring;
        const auto t = std::clamp((m_progress - delay) / (1 - delay), 0.0, 1.0);
        if (t <= 0 || t >= 1) {
            continue;
        }
        auto color = m_color;
        color.setAlphaF(static_cast<float>((1 - t) * (1 - t) * (ring ? 0.5 : 0.9)));
        painter->setPen(QPen { color, 2.5 * (1 - t) + 0.75 });
        const auto radius = coreRadius + (maxRingRadius - coreRadius) * easeOutCubic(t);
        painter->drawEllipse(QPointF {}, radius, radius);
    }

    // The dot pops in and fades out after the rings are gone
    const auto pop = m_progress < 0.15 ? easeOutBack(m_progress / 0.15) : 1.0;
    const auto fade = m_progress < 0.55 ? 1.0 : 1 - (m_progress - 0.55) / 0.45;
    painter->setPen(Qt::NoPen);
    auto haloColor = m_color;
    haloColor.setAlphaF(static_cast<float>(0.25 * fade));
    painter->setBrush(haloColor);
    painter->drawEllipse(QPointF {}, coreRadius * pop * 1.9, coreRadius * pop * 1.9);
    auto dotColor = m_color;
    dotColor.setAlphaF(static_cast<float>(fade));
    painter->setBrush(dotColor);
    painter->drawEllipse(QPointF {}, coreRadius * pop, coreRadius * pop);

    painter->restore();
}

void ConnectionPulse::start()
{
    m_animation.stop();
    setProgress(0);
    setVisible(true);
    m_animation.start();
}

void ConnectionPulse::stop()
{
    m_animation.stop();
    setVisible(false);
}

void ConnectionPulse::setColor(const QColor & color)
{
    m_color = color;
    m_color.setAlpha(255);
    update();
}

qreal ConnectionPulse::progress() const
{
    return m_progress;
}

void ConnectionPulse::setProgress(qreal progress)
{
    m_progress = progress;
    update();
}

} // namespace SceneItems
