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

#include "edge_point.hpp"
#include "edge_text_edit.hpp"

class EdgeDot;
class Node;
class QFont;
class QGraphicsEllipseItem;
class QPropertyAnimation;

//! A graphic representation of a graph edge between nodes.
class Edge : public QObject, public QGraphicsLineItem
{
    Q_OBJECT

public:
    enum class ArrowMode
    {
        Single = 0,
        Double = 1,
        Hidden = 2
    };

    Edge(Node & sourceNode, Node & targetNode, bool enableAnimations = true, bool enableLabel = true);

    virtual ~Edge() override;

    Node & sourceNode() const;

    Node & targetNode() const;

    void setSourceNode(Node & sourceNode);

    void setTargetNode(Node & targetNode);

    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event) override;

    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event) override;

    QString text() const;

    ArrowMode arrowMode() const;

    bool reversed() const;

public slots:

    void updateLine();

    void setArrowMode(ArrowMode arrowMode);

    void setColor(const QColor & color);

    void setFont(const QFont & font);

    void setWidth(double width);

    void setText(const QString & text);

    void setTextSize(int textSize);

    void setReversed(bool reversed);

    void setSelected(bool selected);

signals:

    void undoPointRequested();

private:
    QPen getPen() const;

    void initDots();

    void setArrowHeadPen(const QPen & pen);

    void setLabelVisible(bool visible, EdgeTextEdit::VisibilityChangeReason vcr = EdgeTextEdit::VisibilityChangeReason::Default);

    void updateArrowhead();

    void updateDots();

    enum class LabelUpdateReason
    {
        Default,
        EdgeGeometryChanged
    };

    void updateLabel(LabelUpdateReason lur = LabelUpdateReason::Default);

    Node * m_sourceNode = nullptr;

    Node * m_targetNode = nullptr;

    QString m_text;

    double m_width = 2;

    int m_textSize = 11; // Not sure if we should set yet another default value here..

    QColor m_color;

    bool m_reversed;

    bool m_selected = false;

    ArrowMode m_arrowMode;

    bool m_enableAnimations;

    bool m_enableLabel;

    EdgeDot * m_sourceDot;

    EdgeDot * m_targetDot;

    QPointF m_previousRelativeSourcePos;

    QPointF m_previousRelativeTargetPos;

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
