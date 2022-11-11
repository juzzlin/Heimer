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

#ifndef NODE_HPP
#define NODE_HPP

#include <QFont>
#include <QGraphicsItem>
#include <QImage>
#include <QObject>

#include <map>
#include <vector>

#include "../types.hpp"

#include "edge.hpp"
#include "edge_point.hpp"
#include "node_handle.hpp"
#include "scene_item_base.hpp"

class Image;
class QGraphicsTextItem;
class TextEdit;

struct ShadowEffectParams;

namespace Core {
class SettingsProxy;
}

namespace SpecialContent {
class SpecialContentModel;
class SpecialContentTray;
}

namespace SceneItems {

struct NodeModel;

//! Freely placeable target node.
class Node : public SceneItemBase
{
    Q_OBJECT

public:
    //! Constructor.
    Node();

    //! Copy constructor.
    Node(NodeCR other);

    ~Node() override;

    void addGraphicsEdge(EdgeR edge);

    void addSpecialContent(const SpecialContent::SpecialContentModel & specialContentModel);

    void adjustSize();

    void applyImage(const Image & image);

    QRectF boundingRect() const override;

    QColor color() const;

    bool containsText(const QString & text) const;

    int cornerRadius() const;

    void enableShadowEffect(bool enable) override;

    static std::pair<EdgePoint, EdgePoint> getNearestEdgePoints(NodeCR node1, NodeCR node2);

    void highlightText(const QString & text);

    size_t imageRef() const;

    int index() const;

    static NodeP lastHoveredNode();

    QPointF location() const;

    QRectF placementBoundingRect() const;

    void removeGraphicsEdge(EdgeR edge);

    void removeFromScene() override;

    bool selected() const;

    void setColor(const QColor & color);

    void setCornerRadius(int value);

    void changeFont(const QFont & font);

    void setHandlesVisible(bool visible);

    void setImageRef(size_t imageRef);

    void setIndex(int index);

    void setLocation(QPointF newLocation);

    void setSelected(bool selected);

    void setShadowEffect(const ShadowEffectParams & params);

    void setShowSpecialContentTray(bool showSpecialContentTray);

    void setSize(const QSizeF & size);

    void setText(const QString & text);

    void setTextColor(const QColor & color);

    void setTextInputActive(bool active);

    void setTextSize(int textSize);

    QSizeF size() const;

    QString text() const;

    QColor textColor() const;

    void unselectText();

signals:

    void imageRequested(size_t imageRef, Node & node);

    void undoPointRequested();

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent * event) override;

    void hoverMoveEvent(QGraphicsSceneHoverEvent * event) override;

    void mousePressEvent(QGraphicsSceneMouseEvent * event) override;

    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = nullptr) override;

private:
    void createEdgePoints();

    void createHandles();

    QRectF expandedTextEditRect() const;

    void initTextField();

    void removeHandles();

    void updateEdgeLines();

    void updateHandlePositions();

    Core::SettingsProxy & m_settingsProxy;

    std::unique_ptr<NodeModel> m_nodeModel;

    int m_cornerRadius = 0;

    int m_textSize = 11; // Not sure if we should set yet another default value here..

    QFont m_font;

    bool m_selected = false;

    std::map<NodeHandle::Role, NodeHandle *> m_handles;

    std::vector<EdgeP> m_graphicsEdges;

    std::vector<EdgePoint> m_edgePoints;

    TextEdit * m_textEdit;

    SpecialContent::SpecialContentTray * m_specialContentTray;

    QPointF m_currentMousePos;

    QPixmap m_pixmap;

    bool m_showSpecialContentTray = false;

    static NodeP m_lastHoveredNode;
};

} // namespace SceneItems

#endif // NODE_HPP
