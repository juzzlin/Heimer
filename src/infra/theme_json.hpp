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

#ifndef THEME_JSON_HPP
#define THEME_JSON_HPP

#include "../domain/theme.hpp"

#include <QByteArray>
#include <QJsonObject>

#include <optional>
#include <vector>

//! JSON serialization of themes, e.g. for user theme presets.
namespace ThemeJson {

QJsonObject toJson(const Theme & theme);

//! Missing or invalid properties keep the default ("Classic") values.
Theme fromJson(const QJsonObject & json);

//! \return A document containing the given themes.
QByteArray toDocument(const std::vector<Theme> & themes);

//! \return The themes of the given document or nullopt if the document is invalid.
std::optional<std::vector<Theme>> fromDocument(const QByteArray & document);

} // namespace ThemeJson

#endif // THEME_JSON_HPP
