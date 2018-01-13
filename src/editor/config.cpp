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

#include "config.hpp"

namespace Config {

const QString Common::dataPath                = DATA_PATH;

const QString Common::COPYRIGHT               = "Copyright (c) 2018 Jussi Lind";
const QString Common::QSETTINGS_COMPANY_NAME  = "dementia";
const QString Common::WEB_SITE_URL            = "http://juzzlin.github.io/Dementia";

const QString Editor::SELECT_ICON_PATH        = ":/cursor.png";
const QString Editor::ERASE_ICON_PATH         = ":/cross.png";
const QString Editor::CLEAR_ICON_PATH         = ":/clear.png";
const QString Editor::EDITOR_NAME             = "Dementia";
const QString Editor::EDITOR_VERSION          = VERSION;
const QString Editor::QSETTINGS_SOFTWARE_NAME = "Dementia";

} // Config
