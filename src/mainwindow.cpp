// This file is part of Heimer.
// Copyright (C) 2018 Jussi Lind <jussi.lind@iki.fi>
//
// Heimer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either Config::Editor 3 of the License, or
// (at your option) any later Config::Editor.
// Heimer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Heimer. If not, see <http://www.gnu.org/licenses/>.

#include "mainwindow.hpp"

#include "config.hpp"
#include "aboutdlg.hpp"
#include "mediator.hpp"
#include "mindmapdata.hpp"
#include "mclogger.hh"
#include "statemachine.hpp"

#include <QAction>
#include <QApplication>
#include <QDateTime>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QGraphicsLineItem>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QScreen>
#include <QSettings>
#include <QStandardPaths>
#include <QTimer>
#include <QToolBar>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <cassert>

MainWindow * MainWindow::m_instance = nullptr;

static const QString FILE_EXTENSION(Config::FILE_EXTENSION);

MainWindow::MainWindow(QString mindMapFile)
: m_aboutDlg(new AboutDlg(this))
, m_argMindMapFile(mindMapFile)
, m_mediator(new Mediator(*this))
, m_stateMachine(new StateMachine)
{
    if (!m_instance)
    {
        m_instance = this;
    }
    else
    {
        qFatal("MainWindow already instantiated!");
    }

    setWindowIcon(QIcon(":/heimer-editor.png"));

    init();

    if (!m_argMindMapFile.isEmpty())
    {
        QTimer::singleShot(0, this, SLOT(openArgMindMap()));
    }
}

void MainWindow::addRedoAction(QMenu & menu)
{
    m_redoAction = new QAction(tr("Redo"), this);
    m_redoAction->setShortcut(QKeySequence("Ctrl+Shift+Z"));

    connect(m_redoAction, &QAction::triggered,  [this] () {
        m_mediator->redo();
        setupMindMapAfterUndoOrRedo();
    });

    m_redoAction->setEnabled(false);

    menu.addAction(m_redoAction);
}

void MainWindow::addUndoAction(QMenu & menu)
{
    m_undoAction = new QAction(tr("Undo"), this);
    m_undoAction->setShortcut(QKeySequence("Ctrl+Z"));

    connect(m_undoAction, &QAction::triggered, [this] () {
        m_mediator->undo();
        setupMindMapAfterUndoOrRedo();
    });

    m_undoAction->setEnabled(false);

    menu.addAction(m_undoAction);
}

void MainWindow::createEditMenu()
{
    const auto editMenu = menuBar()->addMenu(tr("&Edit"));

    addUndoAction(*editMenu);

    addRedoAction(*editMenu);
}

void MainWindow::createFileMenu()
{
    const auto fileMenu = menuBar()->addMenu(tr("&File"));

    // Add "new"-action
    const auto newAct = new QAction(tr("&New..."), this);
    newAct->setShortcut(QKeySequence("Ctrl+N"));
    fileMenu->addAction(newAct);
    connect(newAct, &QAction::triggered, [=] () {
        runState(m_stateMachine->calculateState(StateMachine::Action::NewSelected, *m_mediator));
    });

    // Add "open"-action
    const auto openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcut(QKeySequence("Ctrl+O"));
    fileMenu->addAction(openAct);
    connect(openAct, &QAction::triggered, [=] () {
        runState(m_stateMachine->calculateState(StateMachine::Action::OpenSelected, *m_mediator));
    });

    // Add "save"-action
    m_saveAction = new QAction(tr("&Save"), this);
    m_saveAction->setShortcut(QKeySequence("Ctrl+S"));
    m_saveAction->setEnabled(false);
    fileMenu->addAction(m_saveAction);
    connect(m_saveAction, &QAction::triggered, [=] () {
        runState(m_stateMachine->calculateState(StateMachine::Action::SaveSelected, *m_mediator));
    });

    // Add "save as"-action
    m_saveAsAction = new QAction(tr("&Save as..."), this);
    m_saveAsAction->setShortcut(QKeySequence("Ctrl+Shift+S"));
    m_saveAsAction->setEnabled(false);
    fileMenu->addAction(m_saveAsAction);
    connect(m_saveAsAction, &QAction::triggered, [=] () {
        runState(m_stateMachine->calculateState(StateMachine::Action::SaveAsSelected, *m_mediator));
    });

    // Add "quit"-action
    const auto quitAct = new QAction(tr("&Quit"), this);
    quitAct->setShortcut(QKeySequence("Ctrl+W"));
    fileMenu->addAction(quitAct);
    connect(quitAct, &QAction::triggered, [=] () {
        runState(m_stateMachine->calculateState(StateMachine::Action::QuitSelected, *m_mediator));
    });
}

void MainWindow::createHelpMenu()
{
    const auto helpMenu = menuBar()->addMenu(tr("&Help"));

    // Add "about"-action
    const auto aboutAct = new QAction(tr("&About"), this);
    helpMenu->addAction(aboutAct);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(showAboutDlg()));

    // Add "about Qt"-action
    const auto aboutQtAct = new QAction(tr("About &Qt"), this);
    helpMenu->addAction(aboutQtAct);
    connect(aboutQtAct, SIGNAL(triggered()), this, SLOT(showAboutQtDlg()));
}

void MainWindow::createViewMenu()
{
    const auto viewMenu = menuBar()->addMenu(tr("&View"));

    // Add "zoom in"-action
    const auto zoomIn = new QAction(tr("Zoom In"), this);
    viewMenu->addAction(zoomIn);
    zoomIn->setShortcut(QKeySequence("Ctrl++"));
    connect(zoomIn, &QAction::triggered, this, &MainWindow::zoomInTriggered);

    // Add "zoom out"-action
    const auto zoomOut = new QAction(tr("Zoom Out"), this);
    viewMenu->addAction(zoomOut);
    connect(zoomOut, &QAction::triggered, this, &MainWindow::zoomOutTriggered);
    zoomOut->setShortcut(QKeySequence("Ctrl+-"));

    viewMenu->addSeparator();

    // Add "zoom to fit"-action
    const auto zoomToFit = new QAction(tr("&Zoom To Fit"), this);
    viewMenu->addAction(zoomToFit);
    connect(zoomToFit, &QAction::triggered, this, &MainWindow::zoomToFitTriggered);
}

QString MainWindow::getFileDialogFileText() const
{
    return tr("Heimer Files") + " (*" + FILE_EXTENSION + ")";
}

void MainWindow::init()
{
    // Detect screen dimensions
    const auto screen = QGuiApplication::primaryScreen();
    const auto screenGeometry = screen->geometry();
    const int height = screenGeometry.height();
    const int width = screenGeometry.width();

    // Read dialog size data
    QSettings settings;
    settings.beginGroup(m_settingsGroup);
    const float defaultScale = 0.8;
    resize(settings.value("size", QSize(width, height) * defaultScale).toSize());
    settings.endGroup();

    // Try to center the window.
    move(width / 2 - this->width() / 2, height / 2 - this->height() / 2);

    populateMenuBar();

    runState(m_stateMachine->calculateState(StateMachine::Action::MainWindowInitialized, *m_mediator));
}

void MainWindow::setTitle()
{
    const auto appInfo = QString(Config::APPLICATION_NAME) + " " + Config::APPLICATION_VERSION;
    if (m_mediator->fileName().isEmpty())
    {
        if (m_mediator->hasNodes())
        {
            setWindowTitle(appInfo + " - " + tr("New File"));
        }
        else
        {
            setWindowTitle(appInfo);
        }
    }
    else
    {
        if (m_mediator->isModified())
        {
            setWindowTitle(appInfo + " - " + m_mediator->fileName() + " - " + tr("Not Saved"));
        }
        else
        {
            setWindowTitle(appInfo + " - " + m_mediator->fileName());
        }
    }
}

MainWindow * MainWindow::instance()
{
    return MainWindow::m_instance;
}

void MainWindow::closeEvent(QCloseEvent *)
{
    runState(m_stateMachine->calculateState(StateMachine::Action::QuitSelected, *m_mediator));
}

void MainWindow::populateMenuBar()
{
    createFileMenu();

    createEditMenu();

    createViewMenu();

    createHelpMenu();
}

void MainWindow::openArgMindMap()
{
    doOpenMindMap(m_argMindMapFile);
}

void MainWindow::openMindMap()
{
    MCLogger().info() << "Open file";

    const auto path = loadRecentPath();
    const auto fileName = QFileDialog::getOpenFileName(this, tr("Open File"), path, getFileDialogFileText());
    if (!fileName.isEmpty())
    {
        doOpenMindMap(fileName);
    }
}

void MainWindow::disableUndoAndRedo()
{
    m_undoAction->setEnabled(false);
    m_redoAction->setEnabled(false);
}

void MainWindow::enableUndo(bool enable)
{
    m_undoAction->setEnabled(enable);
}

void MainWindow::enableSave(bool enable)
{
    setTitle();

    m_saveAction->setEnabled(enable);
}

void MainWindow::enableSaveAs(bool enable)
{
    m_saveAsAction->setEnabled(enable);
}

QString MainWindow::loadRecentPath() const
{
    QSettings settings;
    settings.beginGroup(m_settingsGroup);
    const auto path = settings.value("recentPath", QStandardPaths::writableLocation(QStandardPaths::HomeLocation)).toString();
    settings.endGroup();
    return path;
}

void MainWindow::showAboutDlg()
{
    m_aboutDlg->exec();
}

void MainWindow::showAboutQtDlg()
{
    QMessageBox::aboutQt(this, tr("About Qt"));
}

void MainWindow::doOpenMindMap(QString fileName)
{
    MCLogger().info() << "Opening '" << fileName.toStdString();

    if (m_mediator->openMindMap(fileName))
    {
        disableUndoAndRedo();

        saveRecentPath(fileName);

        setSaveActionStatesOnOpenedMindMap();

        runState(m_stateMachine->calculateState(StateMachine::Action::MindMapOpened, *m_mediator));
    }
}

void MainWindow::saveRecentPath(QString fileName)
{
    QSettings settings;
    settings.beginGroup(m_settingsGroup);
    settings.setValue("recentPath", fileName);
    settings.endGroup();
}

void MainWindow::saveWindowSize()
{
    QSettings settings;
    settings.beginGroup(m_settingsGroup);
    settings.setValue("size", size());
    settings.endGroup();
}

void MainWindow::setupMindMapAfterUndoOrRedo()
{
    m_undoAction->setEnabled(m_mediator->isUndoable());
    m_redoAction->setEnabled(m_mediator->isRedoable());

    m_mediator->setupMindMapAfterUndoOrRedo();
}

void MainWindow::saveMindMap()
{
    MCLogger().info() << "Save..";

    if (!m_mediator->saveMindMap())
    {
        const auto msg = QString(tr("Failed to save file."));
        MCLogger().error() << msg.toStdString();
        showMessageBox(msg);
        runState(m_stateMachine->calculateState(StateMachine::Action::MindMapSaveFailed, *m_mediator));
        return;
    }
    m_saveAction->setEnabled(false);
    runState(m_stateMachine->calculateState(StateMachine::Action::MindMapSaved, *m_mediator));
}

void MainWindow::saveMindMapAs()
{
    MCLogger().info() << "Save as..";

    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save File As"),
        QStandardPaths::writableLocation(QStandardPaths::HomeLocation),
        getFileDialogFileText());

    if (fileName.isEmpty())
    {
        return;
    }

    if (!fileName.endsWith(FILE_EXTENSION))
    {
        fileName += FILE_EXTENSION;
    }

    if (m_mediator->saveMindMapAs(fileName))
    {
        const auto msg = QString(tr("File '")) + fileName + tr("' saved.");
        MCLogger().info() << msg.toStdString();
        runState(m_stateMachine->calculateState(StateMachine::Action::MindMapSavedAs, *m_mediator));
    }
    else
    {
        const auto msg = QString(tr("Failed to save file as '") + fileName + "'.");
        MCLogger().error() << msg.toStdString();
        showMessageBox(msg);
        runState(m_stateMachine->calculateState(StateMachine::Action::MindMapSaveAsFailed, *m_mediator));
    }
}

void MainWindow::showErrorDialog(QString message)
{
    QMessageBox::critical(this,
         Config::APPLICATION_NAME,
         message,
         "");
}

void MainWindow::showMessageBox(QString message)
{
    QMessageBox msgBox;
    msgBox.setText(message);
    msgBox.exec();
}

void MainWindow::initializeNewMindMap()
{
    MCLogger().info() << "New mind map";
    m_mediator->initializeNewMindMap();
    disableUndoAndRedo();
    setSaveActionStatesOnNewMindMap();

    runState(m_stateMachine->calculateState(StateMachine::Action::NewMindMapInitialized, *m_mediator));
}

void MainWindow::setSaveActionStatesOnNewMindMap()
{
    m_saveAction->setEnabled(false);
    m_saveAsAction->setEnabled(true);
}

void MainWindow::setSaveActionStatesOnOpenedMindMap()
{
    m_saveAction->setEnabled(false);
    m_saveAsAction->setEnabled(true);
}

void MainWindow::runState(StateMachine::State state)
{
    switch (state)
    {
    case StateMachine::State::CloseWindow:
        saveWindowSize();
        close();
        break;
    default:
    case StateMachine::State::Edit:
        setTitle();
        break;
    case StateMachine::State::InitializeNewMindMap:
        initializeNewMindMap();
        break;
    case StateMachine::State::SaveMindMap:
        saveMindMap();
        break;
    case StateMachine::State::ShowSaveAsDialog:
        saveMindMapAs();
        break;
    case StateMachine::State::ShowOpenDialog:
        openMindMap();
        break;
    }
}

MainWindow::~MainWindow()
{
    delete m_mediator;
    delete m_stateMachine;
}
