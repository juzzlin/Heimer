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
#include <QPropertyAnimation>

#include <map>
#include <memory>

#include "edgebase.hpp"

class Node;
class EdgeDot;
class QGraphicsEllipseItem;
class TextEdit;

//! A graphic representation of a graph edge between nodes.
class Edge : public QObject, public QGraphicsLineItem, public EdgeBase
{
    Q_OBJECT

public:

    Edge(Node & sourceNode, Node & targetNode);

    Node & sourceNode() const;

    Node & targetNode() const;

public slots:

    void updateLine();

private:

    void initDots();

    void updateDots(const std::pair<QPointF, QPointF> & nearestPoints);

    void updateLabel();

    EdgeDot * m_sourceDot;

    EdgeDot * m_targetDot;

    TextEdit * m_label;

    QPropertyAnimation m_sourceDotSizeAnimation;

    QPropertyAnimation m_targetDotSizeAnimation;

    int m_dotRadius = 10;
};

using EdgePtr = std::shared_ptr<Edge>;

#endif // EDGE_HPP
