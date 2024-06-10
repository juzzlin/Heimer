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

#ifndef EDGE_TEXT_EDIT_HPP
#define EDGE_TEXT_EDIT_HPP

#include "../../common/types.hpp"
#include "scene_item_base_graphics_item.hpp"
#include "text_edit.hpp"

#include <QPropertyAnimation>
#include <QTimer>

namespace SceneItems {

class Edge;

class EdgeTextEdit : public TextEdit
{
    Q_OBJECT

public:
    EdgeTextEdit(EdgeP parentItem);

    EdgeP edge() const;

    void setVisible(bool visible);

    void updateDueToLostFocus();

    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event) override;

    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event) override;

    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget) override;

signals:
    void hoverEntered();

    void visibilityTimeout();

private:
    void hideAnimated();

    void showAnimated();

    void initializeVisibilityTimer();

    void paintOutline(QPainter * painter, const QStyleOptionGraphicsItem * option);

    EdgeP m_edge;

    QPropertyAnimation m_opacityAnimation;

    QTimer m_visibilityTimer;
};

} // namespace SceneItems

#endif // EDGE_TEXT_EDIT_HPP
