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

#include <QColor>
#include <QImage>
#include <QPen>

class QGraphicsEffect;
class ShadowEffectParams;

namespace GraphicsFactory {

QGraphicsEffect * createDropShadowEffect(const ShadowEffectParams & params, bool selected);

QPen createOutlinePen(const QColor & backgroundColor, double brightness = 0.5);

//! \return A subtle, translucent outline pen that works on both bright and dark fill colors.
QPen createSubtleOutlinePen(const QColor & fillColor);

//! \return A blurred shadow of a rounded rectangle. The image has a margin of blurRadius pixels on each side.
QImage createRoundedRectShadow(QSizeF size, double cornerRadius, int blurRadius, const QColor & color);

//! \param hovered Lifts the item by making the shadow larger. Has no effect on selected items.
void updateDropShadowEffect(QGraphicsEffect * effect, const ShadowEffectParams & params, bool selected, bool disabled = false, bool hovered = false);
} // namespace GraphicsFactory

#endif // GRAPHICS_FACTORY_HPP
