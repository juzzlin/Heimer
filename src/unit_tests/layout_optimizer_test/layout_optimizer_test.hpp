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

#ifndef LAYOUT_OPTIMIZER_TEST_HPP
#define LAYOUT_OPTIMIZER_TEST_HPP

#include <QTest>

#include "../unit_test_base.hpp"

class LayoutOptimizerTest : public UnitTestBase
{
    Q_OBJECT

public:
    LayoutOptimizerTest();

private slots:

    void testNoNodes_ShouldNotInitialize();

    void testSingleNode_ShouldNotDoAnything();

    void testMultipleNodes_ShouldReduceCost();

    void testMultipleNodes_NoEdges_ShouldSpread();
};

#endif // LAYOUT_OPTIMIZER_TEST_HPP
