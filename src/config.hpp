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

//! Config variables for editor and for the game.
namespace Config {

static constexpr auto APPLICATION_NAME = "Heimer";

static constexpr auto APPLICATION_VERSION = VERSION;

static constexpr auto COPYRIGHT = "Copyright (c) 2018 Jussi Lind";

static constexpr auto FILE_EXTENSION = ".alz";

//! "Company" name used in QSettings.
static constexpr auto QSETTINGS_COMPANY_NAME = "Heimer";

static constexpr auto WEB_SITE_URL = "http://juzzlin.github.io/Heimer";

static constexpr auto QSETTINGS_SOFTWARE_NAME = APPLICATION_NAME;

static constexpr auto TRANSLATIONS_RESOURCE_BASE = ":/heimer_";

} // Config
