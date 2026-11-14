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
#include <QPainter>

#include <algorithm>
#include <cmath>
#include <vector>

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

//! One pass of a box blur on an 8-bit alpha image, horizontally or vertically
static void boxBlurPass(QImage & image, int radius, bool horizontal)
{
    const int length = horizontal ? image.width() : image.height();
    const int lineCount = horizontal ? image.height() : image.width();
    const int windowSize = radius * 2 + 1;
    std::vector<uchar> line(static_cast<size_t>(length));
    for (int lineIndex = 0; lineIndex < lineCount; lineIndex++) {
        const auto pixel = [&](int i) -> uchar & {
            return horizontal ? image.scanLine(lineIndex)[i] : image.scanLine(i)[lineIndex];
        };
        for (int i = 0; i < length; i++) {
            line[static_cast<size_t>(i)] = pixel(i);
        }
        const auto valueAt = [&](int i) {
            return i < 0 || i >= length ? 0 : line[static_cast<size_t>(i)];
        };
        int sum = 0;
        for (int i = -radius; i <= radius; i++) {
            sum += valueAt(i);
        }
        for (int i = 0; i < length; i++) {
            pixel(i) = static_cast<uchar>(sum / windowSize);
            sum += valueAt(i + radius + 1) - valueAt(i - radius);
        }
    }
}

QImage GraphicsFactory::createRoundedRectShadow(QSizeF size, double cornerRadius, int blurRadius, const QColor & color)
{
    const int margin = std::max(blurRadius, 0);
    const QSize imageSize { static_cast<int>(std::ceil(size.width())) + margin * 2, static_cast<int>(std::ceil(size.height())) + margin * 2 };
    QImage mask { imageSize, QImage::Format_Alpha8 };
    mask.fill(0);
    {
        QPainter painter { &mask };
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::black);
        painter.drawRoundedRect(QRectF { QPointF { static_cast<double>(margin), static_cast<double>(margin) }, size }, cornerRadius, cornerRadius);
    }

    // Three box blur passes approximate a Gaussian blur that extends about blurRadius pixels
    if (const int boxRadius = blurRadius / 3; boxRadius > 0) {
        for (int pass = 0; pass < 3; pass++) {
            boxBlurPass(mask, boxRadius, true);
            boxBlurPass(mask, boxRadius, false);
        }
    }

    QImage shadow { imageSize, QImage::Format_ARGB32_Premultiplied };
    shadow.fill(Qt::transparent);
    QPainter painter { &shadow };
    painter.drawImage(0, 0, mask);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(shadow.rect(), color);
    return shadow;
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
