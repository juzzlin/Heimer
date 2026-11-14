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

#include "theme_preview.hpp"

#include "../domain/theme.hpp"

#include <QPainter>
#include <QPainterPath>

#include <algorithm>

namespace ThemePreview {

static void drawShadowedRoundedRect(QPainter & painter, const QRectF & rect, double radius, const QColor & color, const Theme & theme)
{
    const auto shadowOffset = std::min(theme.shadowEffect.offset(), 3);
    if (shadowOffset > 0 && theme.shadowEffect.shadowColor().alpha() > 0) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(theme.shadowEffect.shadowColor());
        painter.drawRoundedRect(rect.translated(shadowOffset, shadowOffset), radius, radius);
    }

    painter.setPen(QPen { color.darker(130), 1 });
    painter.setBrush(color);
    painter.drawRoundedRect(rect, radius, radius);
}

QPixmap render(const Theme & theme, QSize size)
{
    QPixmap pixmap { size };
    pixmap.fill(theme.backgroundColor);

    QPainter painter { &pixmap };
    painter.setRenderHint(QPainter::Antialiasing);

    // Grid
    const int gridStep = std::max(size.height() / 4, 4);
    painter.setPen(QPen { theme.gridColor, 1 });
    for (int x = gridStep; x < size.width(); x += gridStep) {
        painter.drawLine(x, 0, x, size.height());
    }
    for (int y = gridStep; y < size.height(); y += gridStep) {
        painter.drawLine(0, y, size.width(), y);
    }

    // Nodes
    const double nodeWidth = size.width() * 0.3;
    const double nodeHeight = size.height() * 0.3;
    const QRectF node0 { size.width() * 0.06, size.height() * 0.12, nodeWidth, nodeHeight };
    const QRectF node1 { size.width() * 0.64, size.height() * 0.58, nodeWidth, nodeHeight };
    const double radius = std::min(static_cast<double>(theme.cornerRadius), nodeHeight / 2) * 0.6;

    // Edge
    painter.setPen(QPen { theme.edgeColor, std::max(1.0, theme.edgeWidth * 0.75) });
    painter.drawLine(node0.center(), node1.center());

    drawShadowedRoundedRect(painter, node0, radius, theme.nodeColor, theme);
    drawShadowedRoundedRect(painter, node1, radius, theme.nodeColor, theme);

    auto font = theme.font;
    font.setPixelSize(std::max(static_cast<int>(nodeHeight * 0.55), 6));
    painter.setFont(font);
    painter.setPen(theme.nodeTextColor);
    painter.drawText(node0, Qt::AlignCenter, "Aa");
    painter.drawText(node1, Qt::AlignCenter, "Aa");

    // Edge label
    const auto center = (node0.center() + node1.center()) / 2;
    const QRectF label { center.x() - nodeWidth * 0.4, center.y() - nodeHeight * 0.35, nodeWidth * 0.8, nodeHeight * 0.7 };
    painter.setPen(QPen { theme.edgeLabelColor.darker(130), 1 });
    painter.setBrush(theme.edgeLabelColor);
    painter.drawRoundedRect(label, 2, 2);
    font.setPixelSize(std::max(static_cast<int>(label.height() * 0.7), 5));
    painter.setFont(font);
    painter.setPen(theme.edgeLabelTextColor);
    painter.drawText(label, Qt::AlignCenter, "ab");

    return pixmap;
}

} // namespace ThemePreview
