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

#ifndef LANGUAGE_MENU_TEST_HPP
#define LANGUAGE_MENU_TEST_HPP

#include "../unit_test_base.hpp"

class LanguageMenuTest : public UnitTestBase
{
    Q_OBJECT

public:
    LanguageMenuTest();

    ~LanguageMenuTest() override;

private slots:

    void cleanupTestCase();

    void test_nonNativeMenuBar_shouldAddLanguageMenuToNonNativeCornerMenuBar();

    void test_nonNativeMenuBar_shouldReuseCornerMenuBar();

    void test_nativeMenuBar_shouldAddLanguageMenuAsTopLevelMenu();

    void test_nativeMenuBar_shouldNotCreateAdditionalMenuBars();

    void test_menu_titleShouldBeActiveLanguage();

    void test_menu_shouldListOtherSupportedLanguages();

    void test_menu_triggeringLanguageActionShouldChangeActiveLanguage();
};

#endif // LANGUAGE_MENU_TEST_HPP
