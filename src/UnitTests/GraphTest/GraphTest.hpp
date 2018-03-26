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

#include <QTest>

class GraphTest : public QObject
{
    Q_OBJECT

public:

    GraphTest();

private slots:

    void testAddEdge();

    void testAddNode();

    void testAddTwoNodes();

    void testGetEdges();

    void testGetNodes();

    void testGetNodeByIndex();

    void testGetNodeByIndex_NotFound();
};
