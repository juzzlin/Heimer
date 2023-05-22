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

#include "graphics_factory.hpp"

#include "../core/shadow_effect_params.hpp"

#include <QGraphicsDropShadowEffect>

QGraphicsEffect * GraphicsFactory::createDropShadowEffect(const ShadowEffectParams & params, bool selected)
{
    const auto effect = new QGraphicsDropShadowEffect;
    updateDropShadowEffect(effect, params, selected, false);
    return effect;
}

QPen GraphicsFactory::createOutlinePen(const QColor & backgroundColor, double brightness)
{
    const int width = 1;
    return QPen { QColor { static_cast<int>(backgroundColor.red() * brightness),
                           static_cast<int>(backgroundColor.green() * brightness),
                           static_cast<int>(backgroundColor.blue() * brightness) },
                  width };
}

void GraphicsFactory::updateDropShadowEffect(QGraphicsEffect * effect, const ShadowEffectParams & params, bool selected, bool disabled)
{
    if (!effect) {
        return;
    }

    if (const auto shadow = qobject_cast<QGraphicsDropShadowEffect *>(effect)) {
        if (!disabled) {
            if (!selected) {
                shadow->setOffset(params.offset, params.offset);
                shadow->setColor(params.shadowColor);
                shadow->setBlurRadius(params.blurRadius);
            } else {
                shadow->setOffset({});
                shadow->setColor(params.selectedItemShadowColor);
                shadow->setBlurRadius(params.selectedItemBlurRadius);
            }
        } else {
            shadow->setOffset({});
            shadow->setColor({});
            shadow->setBlurRadius({});
        }
    }
}
