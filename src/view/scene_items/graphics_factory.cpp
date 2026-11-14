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

#include "../../common/utils.hpp"
#include "../../domain/shadow_effect_params.hpp"

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

QPen GraphicsFactory::createSubtleOutlinePen(const QColor & fillColor)
{
    QPen pen { Utils::isColorBright(fillColor) ? QColor { 0, 0, 0, 40 } : QColor { 255, 255, 255, 36 }, 1 };
    pen.setCosmetic(true);
    return pen;
}

void GraphicsFactory::updateDropShadowEffect(QGraphicsEffect * effect, const ShadowEffectParams & params, bool selected, bool disabled, bool hovered)
{
    if (!effect) {
        return;
    }

    if (const auto shadow = qobject_cast<QGraphicsDropShadowEffect *>(effect)) {
        if (!disabled) {
            if (!selected) {
                // The shadow falls mostly downwards, as from a light above the canvas
                const auto offset = params.offset() + (hovered ? 1 : 0);
                shadow->setOffset(offset * 0.5, offset);
                shadow->setColor(params.shadowColor());
                shadow->setBlurRadius(hovered ? params.blurRadius() * 1.5 + 2 : params.blurRadius());
            } else {
                shadow->setOffset({});
                shadow->setColor(params.selectedItemShadowColor());
                shadow->setBlurRadius(params.selectedItemBlurRadius());
            }
        } else {
            shadow->setOffset({});
            shadow->setColor({});
            shadow->setBlurRadius({});
        }
    }
}
