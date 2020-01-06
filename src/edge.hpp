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

#ifndef EDGE_HPP
#define EDGE_HPP

#include <QGraphicsLineItem>
#include <QTimer>

#include <map>
#include <memory>

#include "edge_base.hpp"
#include "edge_point.hpp"

class EdgeDot;
class EdgeTextEdit;
class Node;
class QGraphicsEllipseItem;
class QPropertyAnimation;

//! A graphic representation of a graph edge between nodes.
class Edge : public QObject, public QGraphicsLineItem, public EdgeBase
{
    Q_OBJECT

public:
    Edge(Node & sourceNode, Node & targetNode, bool enableAnimations = true, bool enableLabel = true);

    virtual ~Edge() override;

    Node & sourceNode() const;

    Node & targetNode() const;

    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event) override;

    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event) override;

public slots:

    void updateLine();

    virtual void setArrowMode(ArrowMode arrowMode) override;

    virtual void setColor(const QColor & color) override;

    virtual void setWidth(double width) override;

    virtual void setText(const QString & text) override;

    virtual void setTextSize(int textSize) override;

    virtual void setReversed(bool reversed) override;

    virtual void setSelected(bool selected) override;

signals:

    void undoPointRequested();

private:
    QPen getPen() const;

    void initDots();

    void setArrowHeadPen(const QPen & pen);

    void setLabelVisible(bool visible);

    void updateArrowhead();

    void updateDots();

    void updateLabel();

    bool m_enableAnimations;

    bool m_enableLabel;

    EdgeDot * m_sourceDot;

    EdgeDot * m_targetDot;

    EdgeTextEdit * m_label;

    QGraphicsLineItem * m_arrowheadL0;

    QGraphicsLineItem * m_arrowheadR0;

    QGraphicsLineItem * m_arrowheadL1;

    QGraphicsLineItem * m_arrowheadR1;

    QPropertyAnimation * m_sourceDotSizeAnimation;

    QPropertyAnimation * m_targetDotSizeAnimation;

    QTimer m_labelVisibilityTimer;
};

using EdgePtr = std::shared_ptr<Edge>;

#endif // EDGE_HPP
