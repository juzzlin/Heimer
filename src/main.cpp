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

#include <QApplication>
#include <QSettings>

#include "application.hpp"
#include "config.hpp"
#include "hashseed.hpp"
#include "userexception.hpp"
#include "mclogger.hh"

#include <cstdlib>
#include <iostream>
#include <memory>

#include <QDir>

static void initLogger()
{
    QString logPath = QDir::tempPath() + QDir::separator() + "heimer.log";
    MCLogger::init(logPath.toStdString().c_str());
    MCLogger::enableEchoMode(true);
    MCLogger::enableDateTimePrefix(true);
    MCLogger().info() << Config::QSETTINGS_SOFTWARE_NAME << " version " << VERSION;
    MCLogger().info() << Config::COPYRIGHT;
    MCLogger().info() << "Compiled against Qt version " << QT_VERSION_STR;
}

int main(int argc, char ** argv)
{
    HashSeed::init();

    QApplication::setOrganizationName(Config::QSETTINGS_COMPANY_NAME);
    QApplication::setApplicationName(Config::QSETTINGS_SOFTWARE_NAME);
#ifdef Q_OS_WIN32
    QSettings::setDefaultFormat(QSettings::IniFormat);
#endif

    std::unique_ptr<Application> app;

    try
    {
        initLogger();

        app.reset(new Application(argc, argv));

        return app->run();
    }
    catch (std::exception & e)
    {
        if (!dynamic_cast<UserException *>(&e))
        {
            std::cerr << e.what() << std::endl;
        }

        app.reset();

        return EXIT_FAILURE;
    }
}
