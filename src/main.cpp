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
#include "constants.hpp"
#include "hash_seed.hpp"
#include "simple_logger.hpp"
#include "user_exception.hpp"

#include <cstdlib>
#include <iostream>

#include <QDir>

static void initLogger()
{
    using juzzlin::L;

    const QString logPath { QDir::tempPath() + QDir::separator() + "heimer.log" };
    L::init(logPath.toStdString().c_str());
    L::enableEchoMode(true);
    L::setTimestampMode(L::TimestampMode::DateTime, " ");
    const std::map<L::Level, std::string> symbols = {
        { L::Level::Debug, "D" },
        { L::Level::Error, "E" },
        { L::Level::Fatal, "F" },
        { L::Level::Info, "I" },
        { L::Level::Trace, "T" },
        { L::Level::Warning, "W" }
    };
    for (auto && symbol : symbols) {
        L::setLevelSymbol(symbol.first, "[" + symbol.second + "]");
    }

#if defined(NDEBUG) or defined(QT_NO_DEBUG)
    L::setLoggingLevel(L::Level::Info);
#else
    L::setLoggingLevel(L::Level::Debug);
#endif

    L().info() << Constants::Application::QSETTINGS_SOFTWARE_NAME << " version " << VERSION;
    L().info() << Constants::Application::COPYRIGHT;
    L().info() << "Compiled against Qt version " << QT_VERSION_STR;
}

int main(int argc, char ** argv)
{
    HashSeed::init();

#if QT_VERSION >= 0x50600 && QT_VERSION > 0x60000
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication::setOrganizationName(Constants::Application::QSETTINGS_COMPANY_NAME);
    QApplication::setApplicationName(Constants::Application::QSETTINGS_SOFTWARE_NAME);
#ifdef Q_OS_WIN32
    QSettings::setDefaultFormat(QSettings::IniFormat);
#endif

    try {
        initLogger();
        return Application(argc, argv).run();
    } catch (std::exception & e) {
        if (!dynamic_cast<UserException *>(&e)) {
            std::cerr << e.what() << std::endl;
        }
        return EXIT_FAILURE;
    }
}
