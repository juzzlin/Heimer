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

#include "application.hpp"
#include "config.hpp"
#include "editordata.hpp"
#include "editorscene.hpp"
#include "editorview.hpp"
#include "mainwindow.hpp"
#include "mediator.hpp"
#include "statemachine.hpp"
#include "userexception.hpp"

#include "contrib/mclogger.hh"

#include <QLocale>
#include <QObject>
#include <QSettings>

#include <iostream>

static void printHelp()
{
    std::cout << std::endl << "Heimer version " << VERSION << std::endl;
    std::cout << Config::COPYRIGHT << std::endl << std::endl;
    std::cout << "Usage: heimer [options] [mindMapFile]" << std::endl << std::endl;
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

    if (appTranslator.load(Config::TRANSLATIONS_RESOURCE_BASE + lang))
    {
        app.installTranslator(&appTranslator);
        MCLogger().info() << "Loaded translations for " << lang.toStdString();
    }
    else
    {
        MCLogger().warning() << "Failed to load translations for " << lang.toStdString();
    }
}

QString Application::parseArgs(int argc, char ** argv)
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
            i++;
        }
        else
        {
            m_mindMapFile = args[i];
        }
    }

    initTranslations(m_appTranslator, m_app, lang);

    return m_mindMapFile;
}

Application::Application(int & argc, char ** argv)
    : m_app(argc, argv)
    , m_stateMachine(new StateMachine)
    , m_mainWindow(new MainWindow(parseArgs(argc, argv)))
    , m_mediator(new Mediator(*m_mainWindow))
    , m_editorData(new EditorData)
    , m_editorScene(new EditorScene)
    , m_editorView(new EditorView(*m_mediator))
{
    m_mainWindow->setMediator(m_mediator);
    m_stateMachine->setMediator(m_mediator);

    m_mediator->setEditorData(m_editorData);
    m_mediator->setEditorScene(m_editorScene);
    m_mediator->setEditorView(*m_editorView);

    // Connect MainWindow and StateMachine together
    connect(m_mainWindow.get(), &MainWindow::actionTriggered, m_stateMachine.get(), &StateMachine::calculateState);
    connect(m_stateMachine.get(), &StateMachine::stateChanged, m_mainWindow.get(), &MainWindow::runState);

    connect(m_editorData.get(), &EditorData::isModifiedChanged, [=] (bool isModified) {
        m_mainWindow->enableSave(isModified && m_mediator->canBeSaved());
    });

    m_mainWindow->initialize();
    m_mediator->initializeView();

    m_mainWindow->show();
}

int Application::run()
{
    return m_app.exec();
}

Application::~Application() = default;
