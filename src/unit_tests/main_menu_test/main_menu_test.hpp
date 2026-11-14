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

#ifndef MAIN_MENU_TEST_HPP
#define MAIN_MENU_TEST_HPP

#include "../unit_test_base.hpp"

class MainMenuTest : public UnitTestBase
{
    Q_OBJECT

public:
    MainMenuTest();

private slots:

    void cleanupTestCase();

    void test_retranslate_shouldPreserveEnabledActions();

    void test_retranslate_shouldPreserveDisabledActions();

    void test_retranslate_shouldPreserveFullScreenState();

    void test_retranslate_shouldPreserveMenuStructure();

    void test_retranslate_shouldNotLeakMenusOrActions();
};

#endif // MAIN_MENU_TEST_HPP
