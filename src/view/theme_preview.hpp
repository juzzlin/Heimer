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

#ifndef THEME_PREVIEW_HPP
#define THEME_PREVIEW_HPP

#include <QPixmap>
#include <QSize>

struct Theme;

namespace ThemePreview {

//! Renders a miniature mind map (two nodes, an edge and an edge label) with the given theme.
QPixmap render(const Theme & theme, QSize size);

} // namespace ThemePreview

#endif // THEME_PREVIEW_HPP
