// This file is part of Heimer.
// Copyright (C) 2022 Jussi Lind <jussi.lind@iki.fi>
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

#include "version_test.hpp"

#include "../../application/version.hpp"

void VersionTest::testInitialization_defaultShouldNotBeValid()
{
    QVERIFY(!Version().isValid());
}

void VersionTest::testInitialization_copyConstructor()
{
    const Version version({ 1, 42, 666 });
    QVERIFY(version.isValid());
    QVERIFY(version.major == 1);
    QVERIFY(version.minor == 42);
    QVERIFY(version.patch == 666);
}

void VersionTest::testInitialization_initializeWithString()
{
    const Version version("1.42.666");
    QVERIFY(version.isValid());
    QVERIFY(version.major == 1);
    QVERIFY(version.minor == 42);
    QVERIFY(version.patch == 666);
}

void VersionTest::testInitialization_initializeWithVersion()
{
    const Version version(1, 42, 666);
    QVERIFY(version.isValid());
    QVERIFY(version.major == 1);
    QVERIFY(version.minor == 42);
    QVERIFY(version.patch == 666);
}

void VersionTest::testEquality()
{
    QVERIFY(Version(1, 42, 666) == Version(1, 42, 666));
    QVERIFY(Version(1, 42, 666) >= Version(1, 42, 666));
    QVERIFY(Version(1, 42, 666) <= Version(1, 42, 666));
    QVERIFY(Version(2, 42, 666) != Version(1, 42, 666));
    QVERIFY(Version(1, 43, 666) != Version(1, 42, 666));
    QVERIFY(Version(1, 42, 667) != Version(1, 42, 666));
    QVERIFY(Version(2, 42, 666) > Version(1, 42, 666));
    QVERIFY(Version(1, 43, 666) > Version(1, 42, 666));
    QVERIFY(Version(1, 42, 667) > Version(1, 42, 666));
    QVERIFY(Version(1, 42, 666) < Version(2, 42, 666));
    QVERIFY(Version(1, 42, 666) < Version(1, 43, 666));
    QVERIFY(Version(1, 42, 666) < Version(1, 42, 667));
    QVERIFY(Version(2, 42, 666) >= Version(1, 42, 666));
    QVERIFY(Version(1, 43, 666) >= Version(1, 42, 666));
    QVERIFY(Version(1, 42, 667) >= Version(1, 42, 666));
    QVERIFY(Version(1, 42, 666) <= Version(2, 42, 666));
    QVERIFY(Version(1, 42, 666) <= Version(1, 43, 666));
    QVERIFY(Version(1, 42, 666) <= Version(1, 42, 667));
}

QTEST_GUILESS_MAIN(VersionTest)
