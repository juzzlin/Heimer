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

#define _CRT_SECURE_NO_WARNINGS

#include "mclogger.hh"
#include <ctime>

#ifdef Q_OS_ANDROID
#include <QDebug>
#else
#include <cstdio>
#endif

bool   MCLogger::m_echoMode = false;
bool   MCLogger::m_dateTime = true;
FILE * MCLogger::m_file     = nullptr;

MCLogger::MCLogger()
{
}

bool MCLogger::init(const char * fileName, bool append)
{
    MCLogger::m_file = nullptr;
    if (fileName)
    {
        if (append)
        {
            MCLogger::m_file = fopen(fileName, "a+");
        }
        else
        {
            MCLogger::m_file = fopen(fileName, "w+");
        }

        if (!m_file)
        {
#ifdef Q_OS_ANDROID
            qDebug() << "ERROR!!: Couldn't open '" << fileName << "' for write.";
#else
            fprintf(stderr, "ERROR!!: Couldn't open '%s' for write.\n", fileName);
#endif
            return false;
        }
    }

    return true;
}

void MCLogger::enableEchoMode(bool enable)
{
    MCLogger::m_echoMode = enable;
}

void MCLogger::enableDateTimePrefix(bool enable)
{
    MCLogger::m_dateTime = enable;
}

void MCLogger::prefixDateTime()
{
    if (MCLogger::m_dateTime)
    {
        time_t rawTime;
        time(&rawTime);
        std::string timeStr(ctime(&rawTime));
        timeStr.erase(timeStr.length() - 1);
        m_oss << "[" << timeStr << "] ";
    }
}

std::ostringstream & MCLogger::info()
{
    MCLogger::prefixDateTime();
    m_oss << "I: ";
    return m_oss;
}

std::ostringstream & MCLogger::warning()
{
    MCLogger::prefixDateTime();
    m_oss << "W: ";
    return m_oss;
}

std::ostringstream & MCLogger::error()
{
    MCLogger::prefixDateTime();
    m_oss << "E: ";
    return m_oss;
}

std::ostringstream & MCLogger::fatal()
{
    MCLogger::prefixDateTime();
    m_oss << "F: ";
    return m_oss;
}

MCLogger::~MCLogger()
{
    if (MCLogger::m_file)
    {
        fprintf(MCLogger::m_file, "%s", m_oss.str().c_str());
        fprintf(MCLogger::m_file, "\n");
        fflush(MCLogger::m_file);
    }

    if (MCLogger::m_echoMode)
    {
#ifdef Q_OS_ANDROID
        qDebug() << m_oss.str().c_str();
#else
        fprintf(stdout, "%s", m_oss.str().c_str());
        fprintf(stdout, "\n");
        fflush(stdout);
#endif
    }
}
