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

#ifndef EDGE_BASE_HPP
#define EDGE_BASE_HPP

#include <memory>

#include <QColor>
#include <QString>

class NodeBase;

class EdgeBase
{
public:
    enum class ArrowMode
    {
        Single = 0,
        Double = 1,
        Hidden = 2
    };

    EdgeBase(NodeBase & sourceNode, NodeBase & targetNode);

    virtual void setSourceNode(NodeBase & sourceNode);

    virtual void setTargetNode(NodeBase & targetNode);

    virtual NodeBase & sourceNodeBase() const;

    virtual NodeBase & targetNodeBase() const;

    ArrowMode arrowMode() const;

    virtual void setArrowMode(ArrowMode arrowMode);

    QColor color() const;

    virtual void setColor(const QColor & color);

    QString text() const;

    virtual void setText(const QString & text);

    virtual ~EdgeBase()
    {
    }

    virtual double width() const;

    virtual void setWidth(double width);

    virtual int textSize() const;

    virtual void setTextSize(int textSize);

    virtual bool reversed() const;

    virtual void setReversed(bool reversed);

    virtual bool selected() const;

    virtual void setSelected(bool selected);

private:
    NodeBase * m_sourceNode;

    NodeBase * m_targetNode;

    QString m_text;

    double m_width = 2;

    int m_textSize = 11; // Not sure if we should set yet another default value here..

    QColor m_color;

    bool m_reversed = false;

    bool m_selected = false;

    ArrowMode m_arrowMode = ArrowMode::Single;
};

#endif // EDGE_BASE_HPP
