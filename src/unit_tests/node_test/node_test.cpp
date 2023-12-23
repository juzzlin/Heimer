// This file is part of Heimer.
// Copyright (C) 2021 Jussi Lind <jussi.lind@iki.fi>
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

#include "node_test.hpp"

#include "../../common/test_mode.hpp"
#include "../../view/scene_items/node.hpp"

using SceneItems::Node;

NodeTest::NodeTest()
{
    TestMode::setEnabled(true);
}

void NodeTest::testContainsText()
{
    Node node;
    node.setText("Foobar");
    QVERIFY(node.containsText("Foobar"));
    QVERIFY(node.containsText("foobar"));
    QVERIFY(node.containsText("foo"));
    QVERIFY(node.containsText("bar"));
}

QTEST_GUILESS_MAIN(NodeTest)
