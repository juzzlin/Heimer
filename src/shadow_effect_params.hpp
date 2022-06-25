// This file is part of Heimer.
// Copyright (C) 2022 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef SHADOW_EFFECT_PARAMS_HPP
#define SHADOW_EFFECT_PARAMS_HPP

#include "constants.hpp"

#include <QColor>

struct ShadowEffectParams
{
    ShadowEffectParams()
    {
    }

    ShadowEffectParams(int offset, int blurRadius, int selectedItemBlurRadius, QColor shadowColor, QColor selectedItemShadowColor)
      : offset(offset)
      , blurRadius(blurRadius)
      , selectedItemBlurRadius(selectedItemBlurRadius)
      , shadowColor(shadowColor)
      , selectedItemShadowColor(selectedItemShadowColor)
    {
    }

    bool operator==(const ShadowEffectParams & other) const
    {
        return offset == other.offset && blurRadius == other.blurRadius && selectedItemBlurRadius == other.selectedItemBlurRadius && shadowColor == other.shadowColor && selectedItemShadowColor == other.selectedItemShadowColor;
    }

    bool operator!=(const ShadowEffectParams & other) const
    {
        return !(*this == other);
    }

    int offset = Constants::Effects::Defaults::SHADOW_EFFECT_OFFSET;

    int blurRadius = Constants::Effects::Defaults::SHADOW_EFFECT_BLUR_RADIUS;

    int selectedItemBlurRadius = Constants::Effects::Defaults::SELECTED_ITEM_SHADOW_EFFECT_BLUR_RADIUS;

    QColor shadowColor = Constants::Effects::Defaults::SHADOW_EFFECT_SHADOW_COLOR;

    QColor selectedItemShadowColor = Constants::Effects::Defaults::SHADOW_EFFECT_SELECTED_ITEM_SHADOW_COLOR;
};

#endif // SHADOW_EFFECT_PARAMS_HPP
