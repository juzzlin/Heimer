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

#ifndef EDITOR_SERVICE_TEST_HPP
#define EDITOR_SERVICE_TEST_HPP

#include <QTest>

#include "../unit_test_base.hpp"

class EditorServiceTest : public UnitTestBase
{
    Q_OBJECT

public:
    EditorServiceTest();

private slots:

    void test_addAndDeleteEdge();

    void test_groupConnection();

    void test_groupDisconnection();

    void test_groupDeletionOfEdges_shouldDeleteSelectedEdges();

    void test_groupDeletionOfNodes_shouldDeleteSelectedNodes();

    void test_groupMove();

    void test_groupSelectionOfEdges();

    void test_groupSelectionOfNodes();

    void test_initializationResetsFileName();

    void test_loadState();

    void test_undoAddNodes();

    void test_redoAddNodes();

    void test_redoState();

    void test_textSearch();

    void test_undoAddEdge();

    void test_undoArrowMode();

    void test_undoArrowSize();

    void test_undoDeleteEdge();

    void test_undoBackgroundColor();

    void test_undoCornerRadius();

    void test_undoEdgeColor();

    void test_undoEdgeDashedLine();

    void test_undoEdgeWidth();

    void test_undoEdgeText();

    void test_undoEdgeReversed();

    void test_undoFontChange();

    void test_undoGroupConnection();

    void test_undoGroupDisconnection();

    void test_undoNodeColor();

    void test_undoNodeImage();

    void test_undoNodeLocation();

    void test_undoNodeTextColor();

    void test_undoTextSize();

    void test_undoState();

    void test_undoStackResetOnNewDesign();

    void test_undoStackResetOnLoadDesign();

    void test_undoModificationFlagOnNewDesign();

    void test_undoModificationFlagOnLoadDesign();
};

#endif // EDITOR_SERVICE_TEST_HPP
