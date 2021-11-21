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

#ifndef EDITOR_DATA_TEST_HPP
#define EDITOR_DATA_TEST_HPP

#include <QTest>

class EditorDataTest : public QObject
{
    Q_OBJECT

public:
    EditorDataTest();

private slots:

    void testAddAndDeleteEdge();

    void testGroupConnection();

    void testGroupDisconnection();

    void testGroupDelete();

    void testGroupMove();

    void testGroupSelection();

    void testLoadState();

    void testUndoAddNodes();

    void testRedoAddNodes();

    void testRedoState();

    void testTextSearch();

    void testUndoAddEdge();

    void testUndoArrowMode();

    void testUndoDeleteEdge();

    void testUndoBackgroundColor();

    void testUndoCornerRadius();

    void testUndoEdgeColor();

    void testUndoEdgeWidth();

    void testUndoEdgeText();

    void testUndoEdgeReversed();

    void testUndoFontChange();

    void testUndoGroupConnection();

    void testUndoGroupDisconnection();

    void testUndoNodeColor();

    void testUndoNodeImage();

    void testUndoNodeLocation();

    void testUndoNodeTextColor();

    void testUndoTextSize();

    void testUndoState();

    void testUndoStackResetOnNewDesign();

    void testUndoStackResetOnLoadDesign();

    void testUndoModificationFlagOnNewDesign();

    void testUndoModificationFlagOnLoadDesign();
};

#endif // EDITOR_DATA_TEST_HPP
