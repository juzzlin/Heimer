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

#ifndef STATUS_LABEL_HPP
#define STATUS_LABEL_HPP

#include <QLabel>
#include <QPropertyAnimation>
#include <QTimer>

#include <optional>

class EditorView;

namespace Widgets {

class StatusLabel : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(qreal scale READ scale WRITE setScale NOTIFY scaleChanged)

public:
    StatusLabel(EditorView & editorView, QString message);

    void updateGeometry();

signals:
    void scaleChanged(qreal scale);

private:
    qreal scale() const;

    void setScale(qreal scale);

    EditorView & m_editorView;

    qreal m_scale = 1.0;

    QPropertyAnimation m_scaleAnimation;

    std::optional<QSize> m_initialSize;

    QTimer m_scaleAnimationDelayTimer;
};

} // namespace Widgets

#endif // STATUS_LABEL_HPP
