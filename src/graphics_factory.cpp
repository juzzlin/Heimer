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

#include <QGraphicsDropShadowEffect>

QGraphicsEffect * GraphicsFactory::createDropShadowEffect(bool selected)
{
    const auto effect = new QGraphicsDropShadowEffect;
    setSelected(effect, selected);
    return effect;
}

void GraphicsFactory::setSelected(QGraphicsEffect * effect, bool selected)
{
    if (!effect) {
        return;
    }

    if (const auto shadow = qobject_cast<QGraphicsDropShadowEffect *>(effect)) {
        if (!selected) {
            shadow->setOffset({ 3, 3 });
            shadow->setColor({ 96, 96, 96 });
            shadow->setBlurRadius(5);
        } else {
            shadow->setOffset({});
            shadow->setColor({ 255, 0, 0 });
            shadow->setBlurRadius(50);
        }
    }
}
