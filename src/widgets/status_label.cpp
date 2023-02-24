// This file is part of Heimer.
// Copyright (C) 2023 Jussi Lind <jussi.lind@iki.fi>
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

#include "status_label.hpp"

#include "../editor_view.hpp"

namespace Widgets {

StatusLabel::StatusLabel(EditorView & editorView, QString message)
  : QLabel(message, &editorView)
  , m_editorView(editorView)
  , m_scaleAnimation(this, "scale")
{
    setOpenExternalLinks(true);
    setStyleSheet("background-color: grey; color: white; margin: 3px; padding: 5px; border-radius: 5px;");

    m_scaleAnimation.setStartValue(1.0);
    m_scaleAnimation.setEndValue(0.0);
    m_scaleAnimation.setDuration(250);

    connect(&m_scaleAnimation, &QPropertyAnimation::valueChanged, this, &StatusLabel::updateGeometryKakkaPissa);

    m_scaleAnimationDelayTimer.setSingleShot(true);
    m_scaleAnimationDelayTimer.setInterval(10000);

    connect(&m_scaleAnimationDelayTimer, &QTimer::timeout, this, [=] {
        m_initialSize = size();
        m_scaleAnimation.start();
    });

    m_scaleAnimationDelayTimer.start();
}

void StatusLabel::updateGeometryKakkaPissa()
{
    if (!m_initialSize.has_value()) {
        m_initialSize = size();
    }

    const auto gm = m_editorView.viewport()->geometry();
    const auto iw = m_initialSize->width();
    const auto ih = m_initialSize->height();
    setGeometry(gm.x() + gm.width() - iw + (iw / 2) * (1 - m_scale), //
                gm.y() + gm.height() - ih + (ih / 2) * (1 - m_scale), //
                iw * m_scale, ih * m_scale);
}

qreal StatusLabel::scale() const
{
    return m_scale;
}

void StatusLabel::setScale(qreal scale)
{
    m_scale = scale;
}

} // namespace Widgets
