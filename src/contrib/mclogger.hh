// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA  02110-1301, USA.
//

#ifndef MCLOGGER_HH
#define MCLOGGER_HH

#include <cstdio>
#include <sstream>

/*! A logging class. A MCLogger instance flushes on destruction.
 *
 * Example initialization:
 *
 * MCLogger::init("myLog.txt");
 * MCLogger::enableEchoMode(true);
 *
 * Example logging:
 *
 * MCLogger().info() << "Initialization finished.";
 * MCLogger().error() << "Foo happened!";
 */
class MCLogger
{
public:

    //! Constructor.
    MCLogger();

    //! Destructor.
    ~MCLogger();

    /*! Initialize the logger.
     *  \param fileName Log to fileName. Can be nullptr.
     *  \param append The existing log will be appended if true.
     *  \return false if file couldn't be opened. */
    static bool init(const char * fileName, bool append = false);

    //! Enable/disable echo mode.
    //! \param enable Echo everything if true. Default is false.
    static void enableEchoMode(bool enable);

    //! Enable/disable date and time prefix.
    //! \param enable Prefix with date and time if true. Default is true.
    static void enableDateTimePrefix(bool enable);

    //! Get stream to the debug log message. Is disabled with -DNDEBUG.
    std::ostringstream & debug();

    //! Get stream to the info log message.
    std::ostringstream & info();

    //! Get stream to the warning log message.
    std::ostringstream & warning();

    //! Get stream to the error log message.
    std::ostringstream & error();

    //! Get stream to the fatal log message.
    std::ostringstream & fatal();

private:

    MCLogger(const MCLogger & r) = delete;
    MCLogger & operator=(const MCLogger & r) = delete;

    void prefixDateTime();

    static bool m_echoMode;

    static bool m_dateTime;

    static FILE * m_file;

    std::ostringstream m_oss;

    std::ostringstream m_null;
};

#endif // MCLOGGER_HH
