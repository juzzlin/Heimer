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

#ifndef LANGUAGE_MENU_HPP
#define LANGUAGE_MENU_HPP

class LanguageService;
class QMenu;
class QMenuBar;

namespace Menus {

//! Adds the language selection menu to the given menu bar and returns it.
//!
//! With a non-native menu bar the language menu is placed in a separate, non-native
//! corner menu bar at the top-right corner. The corner menu bar is re-used if it already exists.
//!
//! With a native menu bar (e.g. KDE global menu, macOS) corner widgets are not shown and a
//! second native menu bar would replace the main menu, so the language menu is added as
//! a regular top-level menu instead.
QMenu * addLanguageMenu(QMenuBar & menuBar, bool nativeMenuBar, LanguageService & languageService);

} // namespace Menus

#endif // LANGUAGE_MENU_HPP
