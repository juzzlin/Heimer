// This file is part of Dementia.
// Copyright (C) 2018 Jussi Lind <jussi.lind@iki.fi>
//
// Dementia is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either Config::Editor 3 of the License, or
// (at your option) any later Config::Editor.
// Dementia is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dementia. If not, see <http://www.gnu.org/licenses/>.

#include "mainwindow.hpp"

#include "config.hpp"
#include "aboutdlg.hpp"
#include "mediator.hpp"
#include "mindmapdata.hpp"

#include <QAction>
#include <QApplication>
#include <QDateTime>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QGraphicsLineItem>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QStandardPaths>
#include <QTimer>
#include <QToolBar>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <cassert>

MainWindow * MainWindow::m_instance = nullptr;

MainWindow::MainWindow(QString mindMapFile)
: m_aboutDlg(new AboutDlg(this))
, m_argMindMapFile(mindMapFile)
, m_mediator(new Mediator(*this))
{
    if (!m_instance)
    {
        m_instance = this;
    }
    else
    {
        qFatal("MainWindow already instantiated!");
    }

    setWindowIcon(QIcon(":/dementia-editor.png"));

    init();

    if (!m_argMindMapFile.isEmpty())
    {
        QTimer::singleShot(0, this, SLOT(openArgMindMap()));
    }
}

void MainWindow::openArgMindMap()
{
    doOpenMindMap(m_argMindMapFile);
}

void MainWindow::init()
{
    setTitle(tr("New file"));

    QSettings settings;

    // Read dialog size data
    settings.beginGroup(m_settingsGroup);
    resize(settings.value("size", QSize(640, 480)).toSize());
    settings.endGroup();

    // Try to center the window.
    QRect geometry(QApplication::desktop()->availableGeometry());
    move(geometry.width() / 2 - width() / 2,
        geometry.height() / 2 - height() / 2);

    m_mediator->initScene();

    populateMenuBar();
}

void MainWindow::setTitle(QString openFileName)
{
    setWindowTitle(
        QString(Config::Editor::EDITOR_NAME) + " " + Config::Editor::EDITOR_VERSION + " - " +
        openFileName);
}

MainWindow * MainWindow::instance()
{
    return MainWindow::m_instance;
}

void MainWindow::closeEvent(QCloseEvent * event)
{
    // Open settings file
    QSettings settings;

    // Save window size
    settings.beginGroup(m_settingsGroup);
    settings.setValue("size", size());
    settings.endGroup();

    event->accept();
}

void MainWindow::populateMenuBar()
{
    // Create "file"-menu
    QMenu * fileMenu = menuBar()->addMenu(tr("&File"));

    // Add "new"-action
    QAction * newAct = new QAction(tr("&New..."), this);
    newAct->setShortcut(QKeySequence("Ctrl+N"));
    fileMenu->addAction(newAct);
    connect(newAct, SIGNAL(triggered()), this, SLOT(initializeNewMindMap()));

    // Add "open"-action
    QAction * openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcut(QKeySequence("Ctrl+O"));
    fileMenu->addAction(openAct);
    connect(openAct, SIGNAL(triggered()), this, SLOT(openMindMap()));

    // Add "save"-action
    m_saveAction = new QAction(tr("&Save"), this);
    m_saveAction->setShortcut(QKeySequence("Ctrl+S"));
    fileMenu->addAction(m_saveAction);
    connect(m_saveAction, SIGNAL(triggered()), this, SLOT(saveMindMap()));
    m_saveAction->setEnabled(false);

    // Add "save as"-action
    m_saveAsAction = new QAction(tr("&Save as..."), this);
    m_saveAsAction->setShortcut(QKeySequence("Ctrl+Shift+S"));
    fileMenu->addAction(m_saveAsAction);
    connect(m_saveAsAction, SIGNAL(triggered()), this, SLOT(saveAsMindMap()));
    m_saveAsAction->setEnabled(false);

    // Add "quit"-action
    QAction * quitAct = new QAction(tr("&Quit"), this);
    quitAct->setShortcut(QKeySequence("Ctrl+W"));
    fileMenu->addAction(quitAct);
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

    // Create "edit"-menu
    QMenu * editMenu = menuBar()->addMenu(tr("&Edit"));

    // Add "undo"-action
    m_undoAction = new QAction(tr("Undo"), this);
    m_undoAction->setShortcut(QKeySequence("Ctrl+Z"));
    editMenu->addAction(m_undoAction);
    connect(m_undoAction, &QAction::triggered, [this](){
        m_mediator->undo();

        setupMindMapAfterUndoOrRedo();

        m_undoAction->setEnabled(m_mediator->isUndoable());
        m_redoAction->setEnabled(m_mediator->isRedoable());
    });
    m_undoAction->setEnabled(false);

    // Add "redo"-action
    m_redoAction = new QAction(tr("Redo"), this);
    m_redoAction->setShortcut(QKeySequence("Ctrl+Shift+Z"));
    editMenu->addAction(m_redoAction);
    connect(m_redoAction, &QAction::triggered, [this](){
        m_mediator->redo();

        setupMindMapAfterUndoOrRedo();

        m_undoAction->setEnabled(m_mediator->isUndoable());
        m_redoAction->setEnabled(m_mediator->isRedoable());
    });
    m_redoAction->setEnabled(false);

    // Create "help"-menu
    QMenu * helpMenu = menuBar()->addMenu(tr("&Help"));

    // Add "about"-action
    QAction * aboutAct = new QAction(tr("&About"), this);
    helpMenu->addAction(aboutAct);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(showAboutDlg()));

    // Add "about Qt"-action
    QAction * aboutQtAct = new QAction(tr("About &Qt"), this);
    helpMenu->addAction(aboutQtAct);
    connect(aboutQtAct, SIGNAL(triggered()), this, SLOT(showAboutQtDlg()));
}

void MainWindow::openMindMap()
{
    // Load recent path
    QSettings settings;

    settings.beginGroup(m_settingsGroup);
    QString path = settings.value("recentPath",
    QStandardPaths::writableLocation(QStandardPaths::HomeLocation)).toString();
    settings.endGroup();

    const QString fileName = QFileDialog::getOpenFileName(
        this, tr("Open a mindMap"), path, tr("MindMap Files (*.dm3)"));
    if (!fileName.isEmpty())
    {
        doOpenMindMap(fileName);
    }
}

void MainWindow::resizeEvent(QResizeEvent * event)
{
    if (!m_mediator->hasNodes())
    {
        m_mediator->center();
    }

    QMainWindow::resizeEvent(event);
}

void MainWindow::showAboutDlg()
{
    m_aboutDlg->exec();
}

void MainWindow::showAboutQtDlg()
{
    QMessageBox::aboutQt(this, tr("About Qt"));
}

bool MainWindow::doOpenMindMap(QString fileName)
{
    if (!QFile::exists(fileName))
    {
        return false;
    }

    // Undo stack will be cleared.
    m_undoAction->setEnabled(false);
    m_redoAction->setEnabled(false);

    if (m_mediator->openMindMap(fileName))
    {
        setTitle(fileName);

        // Save recent path
        QSettings settings;

        settings.beginGroup(m_settingsGroup);
        settings.setValue("recentPath", fileName);
        settings.endGroup();

        m_saveAction->setEnabled(true);

        setActionStatesOnNewMindMap();

        m_saved = true;

        return true;
    }

    return false;
}

void MainWindow::setupMindMapAfterUndoOrRedo()
{
    m_saveAction->setEnabled(true);

    setActionStatesOnNewMindMap();

    m_mediator->setupMindMapAfterUndoOrRedo();
}

void MainWindow::saveMindMap()
{
    // TODO
}

void MainWindow::saveAsMindMap()
{
    // TODO
}

void MainWindow::initializeNewMindMap()
{
    m_mediator->initializeNewMindMap();

    // Undo stack has been cleared.
    m_undoAction->setEnabled(false);
    m_redoAction->setEnabled(false);

    setActionStatesOnNewMindMap();

    setTitle(tr("New file"));

    m_saved = false;
}

void MainWindow::setActionStatesOnNewMindMap()
{
    m_saveAction->setEnabled(true);
    m_saveAsAction->setEnabled(true);
}

void MainWindow::enableUndo(bool enable)
{
    m_undoAction->setEnabled(enable);
}

MainWindow::~MainWindow()
{
    delete m_mediator;
}
