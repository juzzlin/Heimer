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

#ifndef GRAPHICS_FACTORY_HPP
#define GRAPHICS_FACTORY_HPP

#include "constants.hpp"

#include <QColor>

class QGraphicsEffect;

namespace GraphicsFactory {

struct ShadowEffectParams
{
    ShadowEffectParams()
    {
    }

    ShadowEffectParams(int offset)
      : offset(offset)
    {
    }

    ShadowEffectParams(int offset, int blurRadiusNormal, int blurRadiusSelected)
      : offset(offset)
      , blurRadiusNormal(blurRadiusNormal)
      , blurRadiusSelected(blurRadiusSelected)
    {
    }

    ShadowEffectParams(int offset, int blurRadiusNormal, int blurRadiusSelected, QColor colorNormal, QColor colorSelected)
      : offset(offset)
      , blurRadiusNormal(blurRadiusNormal)
      , blurRadiusSelected(blurRadiusSelected)
      , colorNormal(colorNormal)
      , colorSelected(colorSelected)
    {
    }

    bool operator==(const ShadowEffectParams & other) const
    {
        return offset == other.offset && blurRadiusNormal == other.blurRadiusNormal && blurRadiusSelected == other.blurRadiusSelected && colorNormal == other.colorNormal && colorSelected == other.colorSelected;
    }

    bool operator!=(const ShadowEffectParams & other) const
    {
        return !(*this == other);
    }

    int offset = Constants::Effects::Defaults::SHADOW_EFFECT_OFFSET;

    int blurRadiusNormal = Constants::Effects::Defaults::SHADOW_EFFECT_NORMAL_BLUR_RADIUS;

    int blurRadiusSelected = Constants::Effects::Defaults::SHADOW_EFFECT_SELECTED_BLUR_RADIUS;

    QColor colorNormal = { 96, 96, 96 };

    QColor colorSelected = { 255, 0, 0 };
};

QGraphicsEffect * createDropShadowEffect(bool selected, const ShadowEffectParams & params);

void updateDropShadowEffect(QGraphicsEffect * effect, bool selected, const ShadowEffectParams & params);
} // namespace GraphicsFactory

#endif // GRAPHICS_FACTORY_HPP
