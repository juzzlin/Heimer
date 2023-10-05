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

class AlzFileIOTest : public QObject
{
    Q_OBJECT

public:
    AlzFileIOTest();

private slots:

    void testEmptyDesign();

    void testStyle_ArrowSize();

    void testStyle_BackgroundColor();

    void testStyle_CornerRadius();

    void testStyle_EdgeColor();

    void testStyle_EdgeWidth();

    void testStyle_FontItalic();

    void testStyle_FontNonItalic();

    void testStyle_FontBold();

    void testStyle_FontNonBold();

    void testStyle_FontOverline();

    void testStyle_FontNonOverline();

    void testStyle_FontStrikeOut();

    void testStyle_FontNonStrikeOut();

    void testStyle_FontUnderline();

    void testStyle_FontNonUnderline();

    void testStyle_FontWeight_Light();

    void testStyle_FontWeight_Medium();

    void testStyle_FontWeight_Bold();

    void testStyle_GridColor();

    void testStyle_TextSize();

    void testMetadata_LayoutOptimizer();

    void testGraph_NodeDeletion();

    void testGraph_SingleEdge();

    void testGraph_SingleNode();

    void testLoadJpg();

    void testLoadPng();

    void testNotUsedImages();

    void testUsedImages();

    void testV1_ArrowSize();

    void testV1_BackgroundColor();

    void testV1_CornerRadius();

    void testV1_EdgeColor();

    void testV1_EdgeWidth();

    void testV1_FontItalic();

    void testV1_FontNonItalic();

    void testV1_FontBold();

    void testV1_FontNonBold();

    void testV1_FontOverline();

    void testV1_FontNonOverline();

    void testV1_FontStrikeOut();

    void testV1_FontNonStrikeOut();

    void testV1_FontUnderline();

    void testV1_FontNonUnderline();

    void testV1_FontWeight_Light();

    void testV1_FontWeight_Medium();

    void testV1_FontWeight_Bold();

    void testV1_GridColor();

    void testV1_TextSize();

    void testV1_LayoutOptimizer();

    void testV1_Graph_SingleEdge();

    void testV1_Graph_SingleNode();

    void testV1_Version();

    void testV2_Version();
};

#endif // ALZ_FILE_IO_TEST_HPP
