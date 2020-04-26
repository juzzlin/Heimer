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

#ifndef NODEBASE_HPP
#define NODEBASE_HPP

#include <QColor>
#include <QPointF>
#include <QRectF>
#include <QSizeF>
#include <QString>

#include <memory>
#include <vector>

//! Base class for freely placeable target nodes in the editor.
class NodeBase
{
public:
    NodeBase();

    NodeBase(NodeBase & other) = delete;

    NodeBase & operator=(NodeBase & other) = delete;

    virtual ~NodeBase();

    virtual QColor color() const;

    virtual void setColor(const QColor & color);

    virtual int cornerRadius() const;

    virtual void setCornerRadius(int cornerRadius);

    virtual QColor textColor() const;

    virtual void setTextColor(const QColor & color);

    virtual int textSize() const;

    virtual void setTextSize(int textSize);

    virtual QPointF location() const;

    virtual void setLocation(QPointF newLocation);

    virtual QRectF placementBoundingRect() const;

    virtual bool selected() const;

    virtual void setSelected(bool selected);

    virtual QSizeF size() const;

    virtual void setSize(QSizeF size);

    virtual int index() const;

    virtual void setIndex(int index);

    virtual QString text() const;

    virtual void setText(const QString & text);

    virtual size_t imageRef() const;

    virtual void setImageRef(size_t imageRef);

private:
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
};

#endif // NODEBASE_HPP
