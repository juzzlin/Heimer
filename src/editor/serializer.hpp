// This file is part of Dementia.
// Copyright (C) 2018 Jussi Lind <jussi.lind@iki.fi>
//
// Dementia is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dementia is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dementia. If not, see <http://www.gnu.org/licenses/>.

#ifndef SERIALIZER_HPP
#define SERIALIZER_HPP

#include "mindmapdata.hpp"

#include <QDomDocument>

namespace Serializer {

    struct DataKeywords
    {
        struct Header
        {
            static constexpr auto DESIGN = "design";

            static constexpr auto APPLICATION_VERSION = "version";
        };

        struct Graph
        {
            static constexpr auto NODE = "node";

            struct Node
            {
                static constexpr auto COLOR = "color";
                static constexpr auto INDEX = "index";
                static constexpr auto TEXT = "text";
                static constexpr auto X = "x";
                static constexpr auto Y = "y";

                struct Color
                {
                    static constexpr auto R = "r";
                    static constexpr auto G = "g";
                    static constexpr auto B = "b";
                };
            };

            static constexpr auto EDGE = "edge";

            struct Edge
            {
                static constexpr auto INDEX0 = "index0";
                static constexpr auto INDEX1 = "index1";
            };
        };
    };

    MindMapDataPtr fromXml(QDomDocument document);

    QDomDocument toXml(MindMapData & mindMapData);
}

#endif // SERIALIZER_HPP
