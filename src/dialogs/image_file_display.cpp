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

#include "image_file_display.hpp"

#include <QDesktopServices>
#include <QEvent>
#include <QPainter>
#include <QUrl>

namespace Dialogs {

ImageFileDisplay::ImageFileDisplay(QWidget * parent)
  : QWidget(parent)
{
    setAttribute(Qt::WA_Hover, true);
}

bool ImageFileDisplay::event(QEvent * event)
{
    switch (event->type()) {
    case QEvent::HoverEnter:
        if (m_image.has_value()) {
            setCursor(Qt::PointingHandCursor);
        }
        return true;
    case QEvent::HoverLeave:
        unsetCursor();
        return true;
    default:
        break;
    }
    return QWidget::event(event);
}

QSize ImageFileDisplay::sizeHint() const
{
    return { 400, 300 };
}

QSize ImageFileDisplay::minimumSizeHint() const
{
    return { 40, 30 };
}

void ImageFileDisplay::mousePressEvent(QMouseEvent * event)
{
    if (m_image.has_value()) {
        QDesktopServices::openUrl(QUrl(QString("file://") + m_image->path().c_str()));
    }

    QWidget::mousePressEvent(event);
}

void ImageFileDisplay::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setPen(palette().dark().color());
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(QRect(0, 0, width() - 1, height() - 1));

    if (m_image.has_value()) {
        const auto imageWidth = m_image->image().width();
        const auto imageHeight = m_image->image().height();
        const auto margin = 1;
        if (imageWidth > imageHeight) {
            auto targetRect = QRect(margin, margin, this->width() - margin * 2, (this->width() - margin * 2) * imageHeight / imageWidth);
            const auto marginTop = ((this->height() - 2) - targetRect.height()) / 2;
            targetRect.moveTo(targetRect.x(), targetRect.y() + marginTop);
            painter.drawImage(targetRect, m_image->image());
        } else {
            auto targetRect = QRect(margin, margin, this->height() - 2 * imageWidth / imageHeight, this->height() - margin * 2);
            const auto marginLeft = ((this->width() - margin * 2) - targetRect.width()) / 2;
            targetRect.moveTo(targetRect.x() + marginLeft, targetRect.y());
            painter.drawImage(targetRect, m_image->image());
        }
    }
}

void ImageFileDisplay::setImage(const Image & image)
{
    m_image = image;

    setToolTip(tr("Click to open the image in browser"));

    update();
}

} // namespace Dialogs
