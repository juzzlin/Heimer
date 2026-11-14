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

#ifndef ALZ_FILE_IO_TEST_HPP
#define ALZ_FILE_IO_TEST_HPP

#include <QTest>

#include "../unit_test_base.hpp"

class AlzFileIOTest : public UnitTestBase
{
    Q_OBJECT

public:
    AlzFileIOTest();

    ~AlzFileIOTest() override;

private slots:

    void test_emptyDesign();

    void test_style_ArrowSize();

    void test_style_BackgroundColor();

    void test_style_CornerRadius();

    void test_style_EdgeColor();

    void test_style_EdgeWidth();

    void test_style_FontItalic();

    void test_style_FontNonItalic();

    void test_style_FontBold();

    void test_style_FontNonBold();

    void test_style_FontOverline();

    void test_style_FontNonOverline();

    void test_style_FontStrikeOut();

    void test_style_FontNonStrikeOut();

    void test_style_FontUnderline();

    void test_style_FontNonUnderline();

    void test_style_FontWeight_Light();

    void test_style_FontWeight_Medium();

    void test_style_FontWeight_Bold();

    void test_style_GridColor();

    void test_style_TextSize();

    void test_metadata_LayoutOptimizer();

    void test_graph_NodeDeletion();

    void test_graph_SingleEdge();

    void test_graph_SingleNode();

    void test_graph_NodeText();

    void test_graph_EdgeText();

    void test_loadJpg();

    void test_loadPng();

    void test_notUsedImages();

    void test_usedImages();

    void test_v1_ArrowSize();

    void test_v1_BackgroundColor();

    void test_v1_CornerRadius();

    void test_v1_EdgeColor();

    void test_v1_EdgeWidth();

    void test_v1_FontItalic();

    void test_v1_FontNonItalic();

    void test_v1_FontBold();

    void test_v1_FontNonBold();

    void test_v1_FontOverline();

    void test_v1_FontNonOverline();

    void test_v1_FontStrikeOut();

    void test_v1_FontNonStrikeOut();

    void test_v1_FontUnderline();

    void test_v1_FontNonUnderline();

    void test_v1_FontWeight_Light();

    void test_v1_FontWeight_Medium();

    void test_v1_FontWeight_Bold();

    void test_v1_GridColor();

    void test_v1_TextSize();

    void test_v1_LayoutOptimizer();

    void test_v1_Graph_SingleEdge();

    void test_v1_Graph_SingleNode();

    void test_v1_Version();

    void test_v2_Version();
};

#endif // ALZ_FILE_IO_TEST_HPP
