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
#include "constants.hpp"
#include "editordata.hpp"
#include "editorscene.hpp"
#include "editorview.hpp"
#include "mainwindow.hpp"
#include "mediator.hpp"
#include "pngexportdialog.hpp"
#include "statemachine.hpp"
#include "userexception.hpp"

#include "simple_logger.hpp"

#include <QColorDialog>
#include <QFileDialog>
#include <QLocale>
#include <QMessageBox>
#include <QObject>
#include <QSettings>
#include <QStandardPaths>

#include <iostream>

namespace {

using juzzlin::L;

static void printHelp()
{
    std::cout << std::endl << "Heimer version " << VERSION << std::endl;
    std::cout << Constants::Application::COPYRIGHT << std::endl << std::endl;
    std::cout << "Usage: heimer [options] [mindMapFile]" << std::endl << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "--help        Show this help." << std::endl;
    std::cout << "--lang [lang] Force language: fi." << std::endl;
    std::cout << std::endl;
}

static void initTranslations(QTranslator & appTranslator, QGuiApplication & app, QString lang = "")
{
    if (lang.isEmpty())
    {
        lang = QLocale::system().name();
    }

    if (appTranslator.load(Constants::Application::TRANSLATIONS_RESOURCE_BASE + lang))
    {
        app.installTranslator(&appTranslator);
        L().info() << "Loaded translations for " << lang.toStdString();
    }
    else
    {
        L().warning() << "Failed to load translations for " << lang.toStdString();
    }
}

}

void Application::parseArgs(int argc, char ** argv)
{
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
            m_lang = args[i + 1];
            i++;
        }
        else
        {
            m_mindMapFile = args[i];
        }
    }
}

Application::Application(int & argc, char ** argv)
    : m_app(argc, argv)
    , m_stateMachine(new StateMachine)
{
    parseArgs(argc, argv);

    initTranslations(m_appTranslator, m_app, m_lang);

    // Instantiate components here because the possible language given
    // in the command line must have been loaded before this
    m_mainWindow.reset(new MainWindow);
    m_mediator.reset(new Mediator(*m_mainWindow));
    m_editorData.reset(new EditorData);
    m_editorScene.reset(new EditorScene);
    m_editorView = new EditorView(*m_mediator);
    m_pngExportDialog.reset(new PngExportDialog(*m_mainWindow));

    m_mainWindow->setMediator(m_mediator);
    m_stateMachine->setMediator(m_mediator);

    m_mediator->setEditorData(m_editorData);
    m_mediator->setEditorScene(m_editorScene);
    m_mediator->setEditorView(*m_editorView);

    // Connect views and StateMachine together
    connect(this, &Application::actionTriggered, m_stateMachine.get(), &StateMachine::calculateState);
    connect(m_editorView, &EditorView::actionTriggered, m_stateMachine.get(), &StateMachine::calculateState);
    connect(m_mainWindow.get(), &MainWindow::actionTriggered, m_stateMachine.get(), &StateMachine::calculateState);
    connect(m_stateMachine.get(), &StateMachine::stateChanged, this, &Application::runState);

    connect(m_editorData.get(), &EditorData::isModifiedChanged, [=] (bool isModified) {
        m_mainWindow->enableSave(isModified && m_mediator->canBeSaved());
    });

    connect(m_pngExportDialog.get(), &PngExportDialog::pngExportRequested, m_mediator.get(), &Mediator::exportToPNG);

    connect(m_mediator.get(), &Mediator::exportFinished, m_pngExportDialog.get(), &PngExportDialog::finishExport);
    connect(m_mainWindow.get(), &MainWindow::edgeWidthChanged, m_mediator.get(), &Mediator::setEdgeWidth);
    connect(m_mainWindow.get(), &MainWindow::textSizeChanged, m_mediator.get(), &Mediator::setTextSize);
    connect(m_mainWindow.get(), &MainWindow::gridSizeChanged, m_editorView, &EditorView::setGridSize);

    m_mainWindow->initialize();
    m_mediator->initializeView();

    m_mainWindow->show();

    if (!m_mindMapFile.isEmpty())
    {
        QTimer::singleShot(0, this, &Application::openArgMindMap);
    }
}

QString Application::getFileDialogFileText() const
{
    return tr("Heimer Files") + " (*" + Constants::Application::FILE_EXTENSION + ")";
}

QString Application::loadRecentPath() const
{
    QSettings settings;
    settings.beginGroup(m_settingsGroup);
    const auto path = settings.value("recentPath", QStandardPaths::writableLocation(QStandardPaths::HomeLocation)).toString();
    settings.endGroup();
    return path;
}

int Application::run()
{
    return m_app.exec();
}

void Application::runState(StateMachine::State state)
{
    switch (state)
    {
    case StateMachine::State::TryCloseWindow:
        m_mainWindow->saveWindowSize();
        m_mainWindow->close();
        break;
    case StateMachine::State::Exit:
        m_mainWindow->saveWindowSize();
        QApplication::exit(EXIT_SUCCESS);
        break;
    default:
    case StateMachine::State::Edit:
        m_mainWindow->setTitle();
        break;
    case StateMachine::State::InitializeNewMindMap:
        m_mediator->initializeNewMindMap();
        break;
    case StateMachine::State::SaveMindMap:
        saveMindMap();
        break;
    case StateMachine::State::ShowBackgroundColorDialog:
        showBackgroundColorDialog();
        break;
    case StateMachine::State::ShowPngExportDialog:
        showPngExportDialog();
        break;
    case StateMachine::State::ShowNotSavedDialog:
        switch (showNotSavedDialog())
        {
        case QMessageBox::Save:
            emit actionTriggered(StateMachine::Action::NotSavedDialogAccepted);
            break;
        case QMessageBox::Discard:
            emit actionTriggered(StateMachine::Action::NotSavedDialogDiscarded);
            break;
        case QMessageBox::Cancel:
            emit actionTriggered(StateMachine::Action::NotSavedDialogCanceled);
            break;
        }
        break;
    case StateMachine::State::ShowSaveAsDialog:
        saveMindMapAs();
        break;
    case StateMachine::State::ShowOpenDialog:
        openMindMap();
        break;
    }
}

void Application::openArgMindMap()
{
    doOpenMindMap(m_mindMapFile);
}

void Application::openMindMap()
{
    L().debug() << "Open file";

    const auto path = loadRecentPath();
    const auto fileName = QFileDialog::getOpenFileName(m_mainWindow.get(), tr("Open File"), path, getFileDialogFileText());
    if (!fileName.isEmpty())
    {
        doOpenMindMap(fileName);
    }
}

void Application::doOpenMindMap(QString fileName)
{
    L().debug() << "Opening '" << fileName.toStdString();

    if (m_mediator->openMindMap(fileName))
    {
        m_mainWindow->disableUndoAndRedo();

        saveRecentPath(fileName);

        m_mainWindow->setSaveActionStatesOnOpenedMindMap();

        emit actionTriggered(StateMachine::Action::MindMapOpened);
    }
}

void Application::saveMindMap()
{
    L().debug() << "Save..";

    if (!m_mediator->saveMindMap())
    {
        const auto msg = QString(tr("Failed to save file."));
        L().error() << msg.toStdString();
        showMessageBox(msg);
        emit actionTriggered(StateMachine::Action::MindMapSaveFailed);
        return;
    }

    m_mainWindow->enableSave(false);
    emit actionTriggered(StateMachine::Action::MindMapSaved);
}


void Application::saveMindMapAs()
{
    L().debug() << "Save as..";

    QString fileName = QFileDialog::getSaveFileName(
        m_mainWindow.get(),
        tr("Save File As"),
        QStandardPaths::writableLocation(QStandardPaths::HomeLocation),
        getFileDialogFileText());

    if (fileName.isEmpty())
    {
        return;
    }

    if (!fileName.endsWith(Constants::Application::FILE_EXTENSION))
    {
        fileName += Constants::Application::FILE_EXTENSION;
    }

    if (m_mediator->saveMindMapAs(fileName))
    {
        const auto msg = QString(tr("File '")) + fileName + tr("' saved.");
        L().debug() << msg.toStdString();
        emit actionTriggered(StateMachine::Action::MindMapSavedAs);
    }
    else
    {
        const auto msg = QString(tr("Failed to save file as '") + fileName + "'.");
        L().error() << msg.toStdString();
        showMessageBox(msg);
        emit actionTriggered(StateMachine::Action::MindMapSaveAsFailed);
    }
}

void Application::saveRecentPath(QString fileName)
{
    QSettings settings;
    settings.beginGroup(m_settingsGroup);
    settings.setValue("recentPath", fileName);
    settings.endGroup();
}

void Application::showBackgroundColorDialog()
{
    const auto color = QColorDialog::getColor(Qt::white, m_mainWindow.get());
    if (color.isValid()) {
        m_mediator->setBackgroundColor(color);
    }
    emit actionTriggered(StateMachine::Action::BackgroundColorChanged);
}

void Application::showPngExportDialog()
{
    m_pngExportDialog->setImageSize(m_mediator->zoomForExport());
    m_pngExportDialog->exec();

    // Doesn't matter if canceled or not
    emit actionTriggered(StateMachine::Action::PngExported);
}

void Application::showMessageBox(QString message)
{
    QMessageBox msgBox(m_mainWindow.get());
    msgBox.setText(message);
    msgBox.exec();
}

int Application::showNotSavedDialog()
{
    QMessageBox msgBox(m_mainWindow.get());
    msgBox.setText(tr("The mind map has been modified."));
    msgBox.setInformativeText(tr("Do you want to save your changes?"));
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    return msgBox.exec();
}

Application::~Application() = default;
