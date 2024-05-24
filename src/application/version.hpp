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

#ifndef VERSION_HPP
#define VERSION_HPP

#include <QString>
#include <QStringList>

#include <ostream>
#include <sstream>

#undef major
#undef minor
#undef patch

struct Version
{
    Version() = default;

    Version(int major, int minor, int patch)
      : major(major)
      , minor(minor)
      , patch(patch)
    {
    }

    explicit Version(QString version)
    {
        if (const auto splitVersion = version.split("."); splitVersion.size() == 3) {
            major = splitVersion.at(0).toInt();
            minor = splitVersion.at(1).toInt();
            patch = splitVersion.at(2).toInt();
        }
    }

    Version(const Version & other)
      : major(other.major)
      , minor(other.minor)
      , patch(other.patch)
    {
    }

    Version & operator=(const Version & other)
    {
        if (this != &other) {
            major = other.major;
            minor = other.minor;
            patch = other.patch;
        }
        return *this;
    }

    bool operator<(const Version & other) const
    {
        return (major < other.major) || (major == other.major && minor < other.minor) || (major == other.major && minor == other.minor && patch < other.patch);
    }

    bool operator>(const Version & other) const
    {
        return (major > other.major) || (major == other.major && minor > other.minor) || (major == other.major && minor == other.minor && patch > other.patch);
    }

    bool operator<=(const Version & other) const
    {
        return *this == other || *this < other;
    }

    bool operator>=(const Version & other) const
    {
        return *this == other || *this > other;
    }

    bool operator==(const Version & other) const
    {
        return major == other.major && minor == other.minor && patch == other.patch;
    }

    bool operator!=(const Version & other) const
    {
        return !(*this == other);
    }

    bool isValid() const
    {
        return major >= 0 && minor >= 0 && patch >= 0;
    }

    QString toString() const
    {
        std::stringstream ss;
        ss << *this;
        return { ss.str().c_str() };
    }

    int major = -1;
    int minor = -1;
    int patch = -1;

    friend std::ostream & operator<<(std::ostream & os, const Version & version);
};

inline std::ostream & operator<<(std::ostream & os, const Version & version)
{
    os << version.major << "." << version.minor << "." << version.patch;
    return os;
}

#endif // VERSION_HPP
