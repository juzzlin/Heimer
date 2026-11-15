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

#ifndef MAGIC_ZOOM_TEST_HPP
#define MAGIC_ZOOM_TEST_HPP

#include "../unit_test_base.hpp"

class MagicZoomTest : public UnitTestBase
{
    Q_OBJECT

public:
    MagicZoomTest();

private slots:

    void test_calculateRectangle_shouldBeEmptyWithoutItems();

    void test_calculateRectangleByNodes_shouldContainNodesWithMargin();
    void test_calculateRectangleByNodes_exportShouldHaveFixedMargin();

    void test_calculateRectangleByItems_shouldIgnoreOtherItems();

    void test_calculateRectangleByEdges_shouldContainLabels();
};

#endif // MAGIC_ZOOM_TEST_HPP
