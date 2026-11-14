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

#ifndef GRAPH_TEST_HPP
#define GRAPH_TEST_HPP

#include <QTest>

#include "../unit_test_base.hpp"

class GraphTest : public UnitTestBase
{
    Q_OBJECT

public:
    GraphTest();

private slots:

    void test_addEdge();

    void test_addNode();

    void test_addTwoNodes();

    void test_areNodesDirectlyConnected();

    void test_deleteEdge();

    void test_deleteNode();

    void test_deleteNodeInvolvingEdge();

    void test_getEdges();

    void test_getNodes();

    void test_getNodesConnectedToNode();

    void test_getNodeByIndex();

    void test_getNodeByIndex_NotFound();
};

#endif // GRAPH_TEST_HPP
