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

#include <optional>

#include "special_content/special_content_model.hpp"

struct NodeAction
{
    enum class Type
    {
        None,
        AddSpecialContent,
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
      : type(type)
    {
    }

    NodeAction(Type type, QColor color)
      : type(type)
      , color(color)
    {
    }

    NodeAction(Type type, QImage image, QString fileName)
      : type(type)
      , image(image)
      , fileName(fileName)
    {
    }

    NodeAction(Type type, SpecialContent::SpecialContentModel specialContentModel)
      : type(type)
      , specialContentModel(specialContentModel)
    {
    }

    Type type = Type::None;

    QColor color = Qt::white;

    QImage image;

    QString fileName;

    SpecialContent::SpecialContentModel specialContentModel;
};

#endif // NODE_ACTION_HPP
