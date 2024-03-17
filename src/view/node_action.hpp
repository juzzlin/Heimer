// This file is part of Heimer.
// Copyright (C) 2020 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef NODE_ACTION_HPP
#define NODE_ACTION_HPP

#include <QColor>
#include <QImage>

class NodeAction
{
public:
    enum class Type
    {
        None,
        AttachImage,
        ConnectSelected,
        Copy,
        Delete,
        DisconnectSelected,
        MirrorLayoutHorizontally,
        MirrorLayoutVertically,
        Paste,
        RemoveAttachedImage,
        SetNodeColor,
        SetTextColor,
    };

    NodeAction(Type type)
      : m_type(type)
    {
    }

    NodeAction(Type type, QColor color)
      : m_type(type)
      , m_color(color)
    {
    }

    NodeAction(Type type, QImage image, QString fileName)
      : m_type(type)
      , m_image(image)
      , m_fileName(fileName)
    {
    }

    Type type() const;

    QColor color() const;

    QImage image() const;

    QString fileName() const;

private:
    Type m_type = Type::None;

    QColor m_color = Qt::white;

    QImage m_image;

    QString m_fileName;
};

inline NodeAction::Type NodeAction::type() const
{
    return m_type;
}

inline QColor NodeAction::color() const
{
    return m_color;
}

inline QImage NodeAction::image() const
{
    return m_image;
}

inline QString NodeAction::fileName() const
{
    return m_fileName;
}

#endif // NODE_ACTION_HPP
