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

#include "main_window.hpp"

#include "constants.hpp"
#include "about_dlg.hpp"
#include "mediator.hpp"
#include "simple_logger.hpp"

#include <QAction>
#include <QApplication>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QScreen>
#include <QSettings>
#include <QSpinBox>
#include <QToolBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidgetAction>

#include <cassert>

MainWindow * MainWindow::m_instance = nullptr;

MainWindow::MainWindow()
: m_aboutDlg(new AboutDlg(this))
{
    if (!m_instance)
    {
        m_instance = this;
    }
    else
    {
        qFatal("MainWindow already instantiated!");
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

    editMenu->addSeparator();

    auto backgroundColorAction = new QAction(tr("Set background color..."), this);
    backgroundColorAction->setShortcut(QKeySequence("Ctrl+B"));

    connect(backgroundColorAction, &QAction::triggered, [this] () {
        emit actionTriggered(StateMachine::Action::BackgroundColorChangeRequested);
    });

    editMenu->addAction(backgroundColorAction);

    editMenu->addSeparator();

    auto edgeColorAction = new QAction(tr("Set edge color..."), this);
    edgeColorAction->setShortcut(QKeySequence("Ctrl+E"));

    connect(edgeColorAction, &QAction::triggered, [this] () {
        emit actionTriggered(StateMachine::Action::EdgeColorChangeRequested);
    });

    editMenu->addAction(edgeColorAction);

    editMenu->addSeparator();
}

QWidgetAction * MainWindow::createEdgeWidthAction()
{
    m_edgeWidthSpinBox = new QDoubleSpinBox(this);
    m_edgeWidthSpinBox->setSingleStep(Constants::Edge::STEP);
    m_edgeWidthSpinBox->setMinimum(Constants::Edge::MIN_SIZE);
    m_edgeWidthSpinBox->setMaximum(Constants::Edge::MAX_SIZE);
    m_edgeWidthSpinBox->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    const auto dummyWidget = new QWidget(this);
    const auto layout = new QHBoxLayout(dummyWidget);
    dummyWidget->setLayout(layout);
    auto label = new QLabel(tr("Edge width:"));
    layout->addWidget(label);
    layout->addWidget(m_edgeWidthSpinBox);
    layout->setContentsMargins(0, 0, 0, 0);

    auto action = new QWidgetAction(this);
    action->setDefaultWidget(dummyWidget);

    // The ugly cast is needed because there are QDoubleSpinBox::valueChanged(double) and QDoubleSpinBox::valueChanged(QString)
    // In Qt > 5.10 one can use QOverload<double>::of(...)
    connect(m_edgeWidthSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::edgeWidthChanged);

    return action;
}

QWidgetAction * MainWindow::createTextSizeAction()
{
    m_textSizeSpinBox = new QSpinBox(this);
    m_textSizeSpinBox->setMinimum(Constants::Text::MIN_SIZE);
    m_textSizeSpinBox->setMaximum(Constants::Text::MAX_SIZE);
    m_textSizeSpinBox->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    const auto dummyWidget = new QWidget(this);
    const auto layout = new QHBoxLayout(dummyWidget);
    dummyWidget->setLayout(layout);
    auto label = new QLabel(tr("Text size:"));
    layout->addWidget(label);
    layout->addWidget(m_textSizeSpinBox);
    layout->setContentsMargins(0, 0, 0, 0);

    auto action = new QWidgetAction(this);
    action->setDefaultWidget(dummyWidget);

    // The ugly cast is needed because there are QSpinBox::valueChanged(int) and QSpinBox::valueChanged(QString)
    // In Qt > 5.10 one can use QOverload<double>::of(...)
    connect(m_textSizeSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &MainWindow::textSizeChanged);

    return action;
}

QWidgetAction * MainWindow::createGridSizeAction()
{
    m_gridSizeSpinBox = new QSpinBox(this);
    m_gridSizeSpinBox->setMinimum(Constants::Grid::MIN_SIZE);
    m_gridSizeSpinBox->setMaximum(Constants::Grid::MAX_SIZE);
    m_gridSizeSpinBox->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    const auto dummyWidget = new QWidget(this);
    const auto layout = new QHBoxLayout(dummyWidget);
    dummyWidget->setLayout(layout);
    auto label = new QLabel(tr("Grid size:"));
    layout->addWidget(label);
    layout->addWidget(m_gridSizeSpinBox);
    layout->setContentsMargins(0, 0, 0, 0);

    auto action = new QWidgetAction(this);
    action->setDefaultWidget(dummyWidget);

    // The ugly cast is needed because there are QSpinBox::valueChanged(int) and QSpinBox::valueChanged(QString)
    // In Qt > 5.10 one can use QOverload<double>::of(...)
    connect(m_gridSizeSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &MainWindow::gridSizeChanged);

    return action;
}

void MainWindow::createFileMenu()
{
    const auto fileMenu = menuBar()->addMenu(tr("&File"));

    // Add "new"-action
    const auto newAct = new QAction(tr("&New..."), this);
    newAct->setShortcut(QKeySequence("Ctrl+N"));
    fileMenu->addAction(newAct);
    connect(newAct, &QAction::triggered, [=] () {
        emit actionTriggered(StateMachine::Action::NewSelected);
    });

    // Add "open"-action
    const auto openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcut(QKeySequence("Ctrl+O"));
    fileMenu->addAction(openAct);
    connect(openAct, &QAction::triggered, [=] () {
        emit actionTriggered(StateMachine::Action::OpenSelected);
    });

    fileMenu->addSeparator();

    // Add "save"-action
    m_saveAction = new QAction(tr("&Save"), this);
    m_saveAction->setShortcut(QKeySequence("Ctrl+S"));
    m_saveAction->setEnabled(false);
    fileMenu->addAction(m_saveAction);
    connect(m_saveAction, &QAction::triggered, [=] () {
        emit actionTriggered(StateMachine::Action::SaveSelected);
    });

    // Add "save as"-action
    m_saveAsAction = new QAction(tr("&Save as..."), this);
    m_saveAsAction->setShortcut(QKeySequence("Ctrl+Shift+S"));
    m_saveAsAction->setEnabled(false);
    fileMenu->addAction(m_saveAsAction);
    connect(m_saveAsAction, &QAction::triggered, [=] () {
        emit actionTriggered(StateMachine::Action::SaveAsSelected);
    });

    fileMenu->addSeparator();

    // Add "export to PNG image"-action
    const auto exportToPNGAction = new QAction(tr("&Export to PNG image..."), this);
    exportToPNGAction->setShortcut(QKeySequence("Ctrl+Shift+E"));
    fileMenu->addAction(exportToPNGAction);
    connect(exportToPNGAction, &QAction::triggered, [=] () {
        emit actionTriggered(StateMachine::Action::PngExportSelected);
    });

    fileMenu->addSeparator();

    // Add "quit"-action
    const auto quitAct = new QAction(tr("&Quit"), this);
    quitAct->setShortcut(QKeySequence("Ctrl+W"));
    fileMenu->addAction(quitAct);
    connect(quitAct, &QAction::triggered, [=] () {
        emit actionTriggered(StateMachine::Action::QuitSelected);
    });

    connect(fileMenu, &QMenu::aboutToShow, [=] () {
        exportToPNGAction->setEnabled(m_mediator->hasNodes());
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

void MainWindow::createToolBar()
{
    auto toolBar = new QToolBar(this);
    addToolBar(Qt::BottomToolBarArea, toolBar);
    toolBar->addAction(createEdgeWidthAction());
    toolBar->addSeparator();
    toolBar->addAction(createTextSizeAction());
    toolBar->addSeparator();
    toolBar->addAction(createGridSizeAction());
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

    connect(viewMenu, &QMenu::aboutToShow, [=] () {
        zoomToFit->setEnabled(m_mediator->hasNodes());
    });
}

void MainWindow::initialize()
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

    setWindowIcon(QIcon(":/heimer.png"));

    emit actionTriggered(StateMachine::Action::MainWindowInitialized);
}

void MainWindow::setMediator(std::shared_ptr<Mediator> mediator)
{
    m_mediator = mediator;
}

void MainWindow::setTitle()
{
    const auto appInfo = QString(Constants::Application::APPLICATION_NAME) + " " + Constants::Application::APPLICATION_VERSION;
    const auto displayFileName = m_mediator->fileName().isEmpty() ? tr("New File") : m_mediator->fileName();
    if (m_mediator->isModified())
    {
        setWindowTitle(appInfo + " - " + displayFileName + " - " + tr("Not Saved"));
    }
    else
    {
        setWindowTitle(appInfo + " - " + displayFileName);
    }
}

MainWindow * MainWindow::instance()
{
    return MainWindow::m_instance;
}

void MainWindow::closeEvent(QCloseEvent * event)
{
    event->ignore();
    emit actionTriggered(StateMachine::Action::QuitSelected);
}

void MainWindow::populateMenuBar()
{
    createFileMenu();

    createEditMenu();

    createToolBar();

    createViewMenu();

    createHelpMenu();
}

void MainWindow::disableUndoAndRedo()
{
    m_undoAction->setEnabled(false);
    m_redoAction->setEnabled(false);
}

void MainWindow::setEdgeWidth(double value)
{
    if (!qFuzzyCompare(m_edgeWidthSpinBox->value(), value))
    {
        m_edgeWidthSpinBox->setValue(value);
    }
}

void MainWindow::setTextSize(int textSize)
{
    if (m_textSizeSpinBox->value() != textSize)
    {
        m_textSizeSpinBox->setValue(textSize);
    }
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

void MainWindow::showAboutDlg()
{
    m_aboutDlg->exec();
}

void MainWindow::showAboutQtDlg()
{
    QMessageBox::aboutQt(this, tr("About Qt"));
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

void MainWindow::showErrorDialog(QString message)
{
    QMessageBox::critical(this,
         Constants::Application::APPLICATION_NAME,
         message,
         "");
}

void MainWindow::initializeNewMindMap()
{
    disableUndoAndRedo();
    setSaveActionStatesOnNewMindMap();

    emit actionTriggered(StateMachine::Action::NewMindMapInitialized);
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

MainWindow::~MainWindow() = default;
