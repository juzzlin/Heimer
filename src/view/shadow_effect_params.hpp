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

#include "../common/constants.hpp"

#include <QColor>

class ShadowEffectParams
{
public:
    ShadowEffectParams() = default;

    ShadowEffectParams(int offset, int blurRadius, int selectedItemBlurRadius, QColor shadowColor, QColor selectedItemShadowColor)
      : m_offset(offset)
      , m_blurRadius(blurRadius)
      , m_selectedItemBlurRadius(selectedItemBlurRadius)
      , m_shadowColor(shadowColor)
      , m_selectedItemShadowColor(selectedItemShadowColor)
    {
    }

    bool operator==(const ShadowEffectParams & other) const
    {
        return m_offset == other.m_offset && m_blurRadius == other.m_blurRadius && m_selectedItemBlurRadius == other.m_selectedItemBlurRadius && m_shadowColor == other.m_shadowColor && m_selectedItemShadowColor == other.m_selectedItemShadowColor;
    }

    bool operator!=(const ShadowEffectParams & other) const
    {
        return !(*this == other);
    }

    int offset() const;

    int blurRadius() const;

    int selectedItemBlurRadius() const;

    QColor shadowColor() const;

    QColor selectedItemShadowColor() const;

private:
    int m_offset = Constants::Settings::defaultShadowEffectOffset();

    int m_blurRadius = Constants::Settings::defaultShadowEffectBlurRadius();

    int m_selectedItemBlurRadius = Constants::Settings::defaultSelectedItemShadowEffectBlurRadius();

    QColor m_shadowColor = Constants::Settings::defaultShadowEffectShadowColor();

    QColor m_selectedItemShadowColor = Constants::Settings::defaultShadowEffectSelectedItemShadowColor();
};

inline int ShadowEffectParams::offset() const
{
    return m_offset;
}

inline int ShadowEffectParams::blurRadius() const
{
    return m_blurRadius;
}

inline int ShadowEffectParams::selectedItemBlurRadius() const
{
    return m_selectedItemBlurRadius;
}

inline QColor ShadowEffectParams::shadowColor() const
{
    return m_shadowColor;
}

inline QColor ShadowEffectParams::selectedItemShadowColor() const
{
    return m_selectedItemShadowColor;
}

#endif // SHADOW_EFFECT_PARAMS_HPP
