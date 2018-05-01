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

#include "nodetextedit.hpp"
#include "node.hpp"

#include <QKeyEvent>

#include <cassert>

NodeTextEdit::NodeTextEdit(Node * parentItem)
    : TextEdit(parentItem)
{
}

void NodeTextEdit::keyPressEvent(QKeyEvent * event)
{
    QGraphicsTextItem::keyPressEvent(event);

    const float tolerance = 0.001f;
    if (boundingRect().height() > maxHeight() + tolerance || boundingRect().width() > maxWidth() + tolerance ||
        boundingRect().height() < maxHeight() - tolerance || boundingRect().width() < maxWidth() - tolerance)
    {
        auto node = dynamic_cast<Node *>(parentItem());
        assert(node);
        node->adjustSize();
    }
}
