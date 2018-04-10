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

//! Config variables for editor and for the game.
namespace Config {

static constexpr auto APPLICATION_NAME = "Dementia";

static constexpr auto APPLICATION_VERSION = VERSION;

//! The base data path given by -DDATA_PATH.
static constexpr auto dataPath = DATA_PATH;

static constexpr auto COPYRIGHT = "Copyright (c) 2018 Jussi Lind";

//! "Company" name used in QSettings.
static constexpr auto QSETTINGS_COMPANY_NAME = "Dementia";

static constexpr auto WEB_SITE_URL = "http://juzzlin.github.io/Dementia";

static constexpr auto QSETTINGS_SOFTWARE_NAME = APPLICATION_NAME;

} // Config
