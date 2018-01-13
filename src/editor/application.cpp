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

#include "application.hpp"
#include "mainwindow.hpp"

#include "config.hpp"
#include "userexception.hpp"

#include <QLocale>
#include <QSettings>

#include <iostream>

static void printHelp()
{
    std::cout << std::endl << "Dementia version " << VERSION << std::endl;
    std::cout << Config::Common::COPYRIGHT.toStdString() << std::endl << std::endl;
    std::cout << "Usage: dementia [options] [mindMapFile]" << std::endl << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "--help        Show this help." << std::endl;
    std::cout << "--lang [lang] Force language: fi." << std::endl;
    std::cout << std::endl;
}

static void initTranslations(QTranslator & appTranslator, QGuiApplication & app, QString lang = "")
{
    if (lang == "")
    {
        lang = QLocale::system().name();
    }

    if (appTranslator.load(QString(DATA_PATH) + "/translations/dementia_" + lang))
    {
        app.installTranslator(&appTranslator);
        std::cout << "Loaded translations for " << lang.toStdString() << std::endl;
    }
    else
    {
        std::cerr << "Failed to load translations for " << lang.toStdString() << std::endl;
    }
}

void Application::parseArgs(int argc, char ** argv)
{
    QString lang = "";

    const std::vector<QString> args(argv, argv + argc);
    for (unsigned int i = 1; i < args.size(); i++)
    {
        if (args[i] == "-h" || args[i] == "--help")
        {
            printHelp();
            throw UserException("Exit due to help.");
        }
        else if (args[i] == "--lang" && (i + i) < args.size())
        {
            lang = args[i + 1];
        }
        else
        {
            m_mindMapFile = args[i];
        }
    }

    initTranslations(m_appTranslator, m_app, lang);
}

Application::Application(int & argc, char ** argv)
    : m_app(argc, argv)
{
    parseArgs(argc, argv);

    m_mainWindow = new MainWindow(m_mindMapFile);
    m_mainWindow->show();
}

int Application::run()
{
    return m_app.exec();
}

Application::~Application()
{
    delete m_mainWindow;
}
