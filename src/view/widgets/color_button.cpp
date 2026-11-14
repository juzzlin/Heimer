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

#include "color_button.hpp"

#include <QColorDialog>
#include <QPainter>
#include <QPixmap>

namespace Widgets {

ColorButton::ColorButton(QString text, bool alphaChannel, QWidget * parent)
  : QPushButton { text, parent }
  , m_alphaChannel(alphaChannel)
{
    connect(this, &QPushButton::clicked, this, [=] {
        QColorDialog::ColorDialogOptions options = QColorDialog::DontUseNativeDialog;
        if (m_alphaChannel) {
            options |= QColorDialog::ShowAlphaChannel;
        }
        // QColorDialog keeps the alpha of the initial color when another color is picked, so a fully
        // transparent color could never be changed to a visible one without also changing the alpha.
        auto initialColor = m_color;
        if (initialColor.alpha() == 0) {
            initialColor.setAlpha(255);
        }
        if (const auto color = QColorDialog::getColor(initialColor, this, this->text(), options); color.isValid() && color != m_color) {
            setColor(color);
            emit colorChanged(m_color);
        }
    });

    updateIcon();
}

QColor ColorButton::color() const
{
    return m_color;
}

void ColorButton::setColor(const QColor & color)
{
    m_color = color;
    updateIcon();
}

void ColorButton::updateIcon()
{
    const int size = 16;
    QPixmap pixmap { size, size };
    pixmap.fill(Qt::transparent);
    QPainter painter { &pixmap };
    // Checkerboard to visualize transparency
    painter.fillRect(0, 0, size, size, Qt::white);
    painter.fillRect(0, 0, size / 2, size / 2, Qt::lightGray);
    painter.fillRect(size / 2, size / 2, size / 2, size / 2, Qt::lightGray);
    painter.fillRect(0, 0, size, size, m_color);
    painter.setPen(Qt::darkGray);
    painter.drawRect(0, 0, size - 1, size - 1);
    painter.end();
    setIcon(pixmap);
}

} // namespace Widgets
