// This file is part of Heimer.
// Copyright (C) 2020 Jussi Lind <jussi.lind@iki.fi>
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

#include "test_mode.hpp"

#include "simple_logger.hpp"

static const auto TAG = "TestMode";

bool TestMode::m_enabled = false;

bool TestMode::enabled()
{
    return m_enabled;
}

void TestMode::setEnabled(bool testModeEnabled)
{
    m_enabled = testModeEnabled;
}

void TestMode::logDisabledCode(const std::string & message)
{
    juzzlin::L(TAG).debug() << "TestMode: '" << message << "' disabled";
}
