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

#include <QGraphicsItem>
#include <QImage>
#include <QObject>

#include <map>
#include <vector>

#include "edge.hpp"
#include "edge_point.hpp"
#include "node_handle.hpp"

class Image;
class QGraphicsTextItem;
class TextEdit;

//! Freely placeable target node.
class Node : public QObject, public QGraphicsItem
{
    Q_OBJECT

    Q_INTERFACES(QGraphicsItem)

public:
    //! Constructor.
    Node();

    //! Copy constructor.
    Node(const Node & other);

    ~Node() override;

    void addGraphicsEdge(Edge & edge);

    void adjustSize();

    void applyImage(const Image & image);

    QRectF boundingRect() const override;

    QColor color() const;

    int cornerRadius() const;

    using NodePtr = std::shared_ptr<Node>;
    EdgePtr createAndAddGraphicsEdge(NodePtr targetNode);

    static std::pair<EdgePoint, EdgePoint> getNearestEdgePoints(const Node & node1, const Node & node2);

    void hoverEnterEvent(QGraphicsSceneHoverEvent * event) override;

    void hoverMoveEvent(QGraphicsSceneHoverEvent * event) override;

    size_t imageRef() const;

    int index() const;

    static Node * lastHoveredNode();

    QPointF location() const;

    void mousePressEvent(QGraphicsSceneMouseEvent * event) override;

    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = nullptr) override;

    QRectF placementBoundingRect() const;

    void removeGraphicsEdge(Edge & edge);

    void removeHandles();

    bool selected() const;

    void setColor(const QColor & color);

    void setCornerRadius(int value);

    void setHandlesVisible(bool visible);

    void setImageRef(size_t imageRef);

    void setIndex(int index);

    void setLocation(QPointF newLocation);

    void setSelected(bool selected);

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

private:
    void createEdgePoints();

    void createHandles();

    QRectF expandedTextEditRect() const;

    void initTextField();

    void updateEdgeLines();

    void updateHandlePositions();

    QColor m_color = Qt::white;

    int m_cornerRadius = 0;

    QColor m_textColor = Qt::black;

    int m_textSize = 11; // Not sure if we should set yet another default value here..

    QPointF m_location;

    QSizeF m_size;

    QString m_text;

    bool m_selected = false;

    int m_index = -1;

    size_t m_imageRef = 0;

    std::map<NodeHandle::Role, NodeHandle *> m_handles;

    std::vector<Edge *> m_graphicsEdges;

    std::vector<EdgePoint> m_edgePoints;

    TextEdit * m_textEdit;

    QPointF m_currentMousePos;

    QPixmap m_pixmap;

    static Node * m_lastHoveredNode;
};

using NodePtr = std::shared_ptr<Node>;

#endif // NODE_HPP
