// This file is part of Heimer.
// Copyright (C) 2026 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef THEME_HPP
#define THEME_HPP

#include "../common/constants.hpp"
#include "shadow_effect_params.hpp"

#include <QColor>
#include <QFont>
#include <QString>

//! The visual style of a mind map.
//!
//! The default values correspond to the "Classic" look. Individual nodes may
//! override the node colors, so nodeColor and nodeTextColor are the colors of new nodes.
struct Theme
{
    QString name;

    // Canvas
    QColor backgroundColor = Constants::MindMap::defaultBackgroundColor();

    QColor gridColor = Constants::MindMap::defaultGridColor();

    // Nodes
    QColor nodeColor = Constants::MindMap::defaultNodeColor();

    QColor nodeTextColor = Constants::MindMap::defaultNodeTextColor();

    int cornerRadius = Constants::Node::defaultCornerRadius();

    // Text
    QFont font;

    int textSize = Constants::MindMap::defaultTextSize();

    // Edges
    QColor edgeColor = Constants::MindMap::defaultEdgeColor();

    double edgeWidth = Constants::Settings::defaultEdgeWidth();

    double arrowSize = Constants::Settings::defaultArrowSize();

    // Edge labels
    QColor edgeLabelColor = Constants::Edge::defaultLabelColor();

    QColor edgeLabelTextColor = Constants::Edge::defaultLabelTextColor();

    // Effects
    ShadowEffectParams shadowEffect;

    //! \return true if the visual properties are equal. The name is not compared.
    bool isVisuallyEqual(const Theme & other) const
    {
        return backgroundColor == other.backgroundColor && gridColor == other.gridColor && //
          nodeColor == other.nodeColor && nodeTextColor == other.nodeTextColor && cornerRadius == other.cornerRadius && //
          font == other.font && textSize == other.textSize && //
          edgeColor == other.edgeColor && qFuzzyCompare(edgeWidth, other.edgeWidth) && qFuzzyCompare(arrowSize, other.arrowSize) && //
          edgeLabelColor == other.edgeLabelColor && edgeLabelTextColor == other.edgeLabelTextColor && //
          shadowEffect == other.shadowEffect;
    }

    bool operator==(const Theme & other) const
    {
        return name == other.name && isVisuallyEqual(other);
    }

    bool operator!=(const Theme & other) const
    {
        return !(*this == other);
    }
};

#endif // THEME_HPP
