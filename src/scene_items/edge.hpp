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

#include <QTimer>

#include <map>
#include <memory>

#include "../types.hpp"
#include "edge_model.hpp"
#include "edge_point.hpp"
#include "edge_text_edit.hpp"
#include "scene_item_base.hpp"

class QFont;
class QGraphicsEllipseItem;
class QGraphicsLineItem;
class QPropertyAnimation;

struct ShadowEffectParams;

namespace Core {
class SettingsProxy;
}

namespace SceneItems {

class EdgeDot;
class Node;

//! A graphic representation of a graph edge between nodes.
class Edge : public SceneItemBase
{
    Q_OBJECT

public:

    //! Constructor.
    //! Note!!: We are using raw pointers here because the edge only must only refer to the nodes.
    Edge(NodeP sourceNode, NodeP targetNode, bool enableAnimations = true, bool enableLabel = true);

    //! Constructor.
    //! Note!!: We are using raw pointers from the shared pointers here because the edge only must only refer to the nodes.
    Edge(NodeS sourceNode, NodeS targetNode, bool enableAnimations = true, bool enableLabel = true);

    //! Copy edge data and find connected node instances from the given graph.
    Edge(EdgeCR other, GraphCR graph);

    //! Copy edge data and leave connected nodes as nullptr's.
    Edge(EdgeCR other);

    virtual ~Edge() override;

    EdgeModel::ArrowMode arrowMode() const;

    QRectF boundingRect() const override;

    bool dashedLine() const;

    void enableShadowEffect(bool enable) override;

    double length() const;

    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = nullptr) override;

    void removeFromScene() override;

    bool reversed() const;

    void restoreLabelParent();

    NodeR sourceNode() const;

    NodeR targetNode() const;

    void setSourceNode(NodeR sourceNode);

    void setTargetNode(NodeR targetNode);

    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event) override;

    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event) override;

    QString text() const;

public slots:

    void changeFont(const QFont & font);

    void updateLine();

    void setArrowMode(EdgeModel::ArrowMode arrowMode);

    void setArrowSize(double arrowSize);

    void setColor(const QColor & color);

    void setDashedLine(bool enable);

    void setEdgeWidth(double edgeWidth);

    void setText(const QString & text);

    void setTextSize(int textSize);

    void setReversed(bool reversed);

    void setSelected(bool selected);

    void setShadowEffect(const ShadowEffectParams & params);

signals:

    void undoPointRequested();

private:
    QPen buildPen(bool ignoreDashSetting = false) const;

    void copyData(EdgeCR other);

    void initDots();

    void setArrowHeadPen(const QPen & pen);

    void setLabelVisible(bool visible, EdgeTextEdit::VisibilityChangeReason vcr = EdgeTextEdit::VisibilityChangeReason::Timeout);

    void updateArrowhead();

    void updateDots();

    enum class LabelUpdateReason
    {
        Default,
        EdgeGeometryChanged
    };

    void updateLabel(LabelUpdateReason lur = LabelUpdateReason::Default);

    Core::SettingsProxy & m_settingsProxy;

    std::unique_ptr<EdgeModel> m_edgeModel;

    NodeP m_sourceNode = nullptr;

    NodeP m_targetNode = nullptr;

    int m_textSize = 11; // Not sure if we should set yet another default value here..

    QColor m_color;

    bool m_selected = false;

    bool m_enableAnimations;

    bool m_enableLabel;

    EdgeDot * m_sourceDot;

    EdgeDot * m_targetDot;

    QPointF m_previousRelativeSourcePos;

    QPointF m_previousRelativeTargetPos;

    EdgeTextEdit * m_label;

    EdgeTextEdit * m_dummyLabel;

    QGraphicsLineItem * m_line;

    QGraphicsLineItem * m_arrowheadL0;

    QGraphicsLineItem * m_arrowheadR0;

    QGraphicsLineItem * m_arrowheadL1;

    QGraphicsLineItem * m_arrowheadR1;

    QPropertyAnimation * m_sourceDotSizeAnimation;

    QPropertyAnimation * m_targetDotSizeAnimation;

    QTimer m_labelVisibilityTimer;
};

} // namespace SceneItems

#endif // EDGE_HPP
