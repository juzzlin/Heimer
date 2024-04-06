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

#ifndef NODE_HANDLE_HPP
#define NODE_HANDLE_HPP

#include <QGraphicsItem>
#include <QPropertyAnimation>
#include <QTimer>

#include "../../common/types.hpp"
#include "scene_item_base.hpp"

namespace SceneItems {

class Node;

class NodeHandle : public SceneItemBase
{
    Q_OBJECT

public:
    enum class Role
    {
        ConnectOrCreate,
        NodeColor,
        Move,
        TextColor
    };

    NodeHandle(NodeR parentNode, Role role, int radius);

    virtual ~NodeHandle() override;

    QRectF boundingRect() const override;

    void paint(QPainter * painter,
               const QStyleOptionGraphicsItem * option, QWidget * widget = nullptr) override;

    void setVisible(bool visible);

    Role role() const;

    NodeR parentNode() const;

    int radius() const;

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent * event) override;

    void hoverLeaveEvent(QGraphicsSceneHoverEvent * event) override;

    void hoverMoveEvent(QGraphicsSceneHoverEvent * event) override;

private:
    QColor calculateBackgroundColor() const;

    void drawArrow(QPainter & painter, QPointF arrowStart, QPointF arrowEnd) const;

    void drawBackground(QPainter & painter) const;

    void drawColorHandle(QPainter & painter) const;

    void drawConnectOrCreateHandle(QPainter & painter) const;

    void drawMoveHandle(QPainter & painter) const;

    void drawPixmapHandle(QPainter & painter) const;

    void drawTextColorHandle(QPainter & painter) const;

    void drawCenteredRect(QPainter & painter, qreal relW, qreal relH, qreal relY) const;

    QString getToolTipTextByRole(NodeHandle::Role role) const;

    qreal relXToX(qreal relX) const;

    qreal relYToY(qreal relY) const;

    qreal relWToW(qreal relW) const;

    qreal relHToH(qreal relH) const;

    QPointF relPToP(const QPointF & relP) const;

    QPen getForegroundPen() const;

    const double m_handleOpacity = 0.75;

    const int m_handleAnimationDuration = 150;

    const int m_handleVisibilityDuration = 2500;

    NodeR m_parentNode;

    Role m_role;

    int m_radius;

    QSize m_size;

    QTimer m_visibilityTimer;

    // Logical state
    bool m_visible = false;
};

} // namespace SceneItems

#endif // NODE_HANDLE_HPP
