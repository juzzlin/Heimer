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

#ifndef EDGETEXTEDIT_HPP
#define EDGETEXTEDIT_HPP

#include "text_edit.hpp"

#include <QPropertyAnimation>
#include <QTimer>

class Edge;

class EdgeTextEdit : public TextEdit
{
public:
    EdgeTextEdit(Edge * parentItem);

    void setVisible(bool visible);

    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent * event) override;

    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event) override;

    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event) override;

private:
    QPropertyAnimation m_sizeAnimation;

    QTimer m_visibilityTimer;
};

#endif // EDGETEXTEDIT_HPP
