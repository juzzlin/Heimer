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

#include "about_dlg.hpp"
#include "constants.hpp"
#include "mediator.hpp"
#include "node_action.hpp"
#include "recent_files_manager.hpp"
#include "recent_files_menu.hpp"
#include "settings.hpp"
#include "settings_dialog.hpp"
#include "simple_logger.hpp"
#include "tool_bar.hpp"
#include "whats_new_dlg.hpp"
#include "widget_factory.hpp"

#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QScreen>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWidgetAction>

#include <cassert>

MainWindow * MainWindow::m_instance = nullptr;

MainWindow::MainWindow()
  : m_aboutDlg(new AboutDlg(this))
  , m_settingsDlg(new SettingsDialog(this))
  , m_toolBar(new ToolBar(this))
  , m_whatsNewDlg(new WhatsNewDlg(this))
  , m_connectSelectedNodesAction(new QAction(tr("Connect selected nodes"), this))
  , m_disconnectSelectedNodesAction(new QAction(tr("Disconnect selected nodes"), this))
  , m_saveAction(new QAction(tr("&Save"), this))
  , m_saveAsAction(new QAction(tr("&Save as") + Constants::Misc::THREE_DOTS, this))
  , m_undoAction(new QAction(tr("Undo"), this))
  , m_redoAction(new QAction(tr("Redo"), this))
  , m_cornerRadiusSpinBox(new QSpinBox(this))
  , m_gridSizeSpinBox(new QSpinBox(this))
  , m_copyOnDragCheckBox(new QCheckBox(tr("Copy on drag"), this))
  , m_showGridCheckBox(new QCheckBox(tr("Show grid"), this))
  , m_searchLineEdit(new QLineEdit(this))
{
    if (!m_instance) {
        m_instance = this;
    } else {
        qFatal("MainWindow already instantiated!");
    }

    addToolBar(Qt::BottomToolBarArea, m_toolBar);
}

void MainWindow::addConnectSelectedNodesAction(QMenu & menu)
{
    m_connectSelectedNodesAction->setShortcut(QKeySequence("Ctrl+Shift+C"));
    connect(m_connectSelectedNodesAction, &QAction::triggered, [this] {
        juzzlin::L().debug() << "Connect selected triggered";
        m_mediator->performNodeAction({ NodeAction::Type::ConnectSelected });
    });
    menu.addAction(m_connectSelectedNodesAction);
    connect(&menu, &QMenu::aboutToShow, [=] {
        m_connectSelectedNodesAction->setEnabled(m_mediator->areSelectedNodesConnectable());
    });
}

void MainWindow::addDisconnectSelectedNodesAction(QMenu & menu)
{
    m_disconnectSelectedNodesAction->setShortcut(QKeySequence("Ctrl+Shift+D"));
    connect(m_disconnectSelectedNodesAction, &QAction::triggered, [this] {
        juzzlin::L().debug() << "Disconnect selected triggered";
        m_mediator->performNodeAction({ NodeAction::Type::DisconnectSelected });
    });
    menu.addAction(m_disconnectSelectedNodesAction);
    connect(&menu, &QMenu::aboutToShow, [=] {
        m_disconnectSelectedNodesAction->setEnabled(m_mediator->areSelectedNodesDisconnectable());
    });
}

void MainWindow::addRedoAction(QMenu & menu)
{
    m_redoAction->setShortcut(QKeySequence(QKeySequence::Redo));

    connect(m_redoAction, &QAction::triggered, [=] {
        m_mediator->redo();
    });

    m_redoAction->setEnabled(false);

    menu.addAction(m_redoAction);
}

void MainWindow::addUndoAction(QMenu & menu)
{
    m_undoAction->setShortcut(QKeySequence(QKeySequence::Undo));

    connect(m_undoAction, &QAction::triggered, [=] {
        m_mediator->undo();
    });

    m_undoAction->setEnabled(false);

    menu.addAction(m_undoAction);
}

void MainWindow::changeFont(const QFont & font)
{
    m_toolBar->changeFont(font);
}

void MainWindow::createEditMenu()
{
    const auto editMenu = menuBar()->addMenu(tr("&Edit"));

    addUndoAction(*editMenu);

    addRedoAction(*editMenu);

    editMenu->addSeparator();

    addConnectSelectedNodesAction(*editMenu);

    addDisconnectSelectedNodesAction(*editMenu);

    editMenu->addSeparator();

    const auto colorMenu = new QMenu;
    const auto colorMenuAction = editMenu->addMenu(colorMenu);
    colorMenuAction->setText(tr("General &colors"));

    const auto backgroundColorAction = new QAction(tr("Set background color") + Constants::Misc::THREE_DOTS, this);
    backgroundColorAction->setShortcut(QKeySequence("Ctrl+B"));

    connect(backgroundColorAction, &QAction::triggered, [=] {
        emit actionTriggered(StateMachine::Action::BackgroundColorChangeRequested);
    });

    colorMenu->addAction(backgroundColorAction);

    colorMenu->addSeparator();

    const auto edgeColorAction = new QAction(tr("Set edge color") + Constants::Misc::THREE_DOTS, this);
    edgeColorAction->setShortcut(QKeySequence("Ctrl+E"));

    connect(edgeColorAction, &QAction::triggered, [=] {
        emit actionTriggered(StateMachine::Action::EdgeColorChangeRequested);
    });

    colorMenu->addAction(edgeColorAction);

    colorMenu->addSeparator();

    const auto gridColorAction = new QAction(tr("Set grid color") + Constants::Misc::THREE_DOTS, this);
    gridColorAction->setShortcut(QKeySequence("Ctrl+G"));

    connect(gridColorAction, &QAction::triggered, [=] {
        emit actionTriggered(StateMachine::Action::GridColorChangeRequested);
    });

    colorMenu->addAction(gridColorAction);

    editMenu->addSeparator();

    auto optimizeLayoutAction = new QAction(tr("Optimize layout") + Constants::Misc::THREE_DOTS, this);
    optimizeLayoutAction->setShortcut(QKeySequence("Ctrl+Shift+O"));

    connect(optimizeLayoutAction, &QAction::triggered, [=] {
        emit actionTriggered(StateMachine::Action::LayoutOptimizationRequested);
    });

    editMenu->addAction(optimizeLayoutAction);
}

QWidgetAction * MainWindow::createCornerRadiusAction()
{
    m_cornerRadiusSpinBox->setMinimum(0);
    m_cornerRadiusSpinBox->setMaximum(Constants::Node::MAX_CORNER_RADIUS);
    m_cornerRadiusSpinBox->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    connect(m_cornerRadiusSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::cornerRadiusChanged);
#else
    connect(m_cornerRadiusSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &MainWindow::cornerRadiusChanged);
#endif
    return WidgetFactory::buildToolBarWidgetActionWithLabel(tr("Corner radius:"), *m_cornerRadiusSpinBox, *this).second;
}

QWidgetAction * MainWindow::createGridSizeAction()
{
    m_gridSizeSpinBox->setMinimum(Constants::Grid::MIN_SIZE);
    m_gridSizeSpinBox->setMaximum(Constants::Grid::MAX_SIZE);
    m_gridSizeSpinBox->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    const auto signal = QOverload<int>::of(&QSpinBox::valueChanged);
#else
    const auto signal = static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged);
#endif
    connect(m_gridSizeSpinBox, signal, this, &MainWindow::gridSizeChanged);
    connect(m_gridSizeSpinBox, signal, Settings::saveGridSize);

    m_gridSizeSpinBox->setValue(Settings::loadGridSize());

    return WidgetFactory::buildToolBarWidgetActionWithLabel(tr("Grid size:"), *m_gridSizeSpinBox, *this).second;
}

QWidgetAction * MainWindow::createSearchAction()
{
    m_searchTimer.setSingleShot(true);
    connect(&m_searchTimer, &QTimer::timeout, [this, searchLineEdit = m_searchLineEdit]() {
        const auto text = searchLineEdit->text();
        juzzlin::L().debug() << "Search text changed: " << text.toStdString();
        emit searchTextChanged(text);
    });
    connect(m_searchLineEdit, &QLineEdit::textChanged, [searchTimer = &m_searchTimer](const QString & text) {
        if (text.isEmpty()) {
            searchTimer->start(0);
        } else {
            searchTimer->start(Constants::View::TEXT_SEARCH_DELAY_MS);
        }
    });
    connect(m_searchLineEdit, &QLineEdit::returnPressed, [searchTimer = &m_searchTimer] {
        searchTimer->start(0);
    });

    return WidgetFactory::buildToolBarWidgetActionWithLabel(tr("Search:"), *m_searchLineEdit, *this).second;
}

void MainWindow::createExportSubMenu(QMenu & fileMenu)
{
    const auto exportMenu = new QMenu;
    const auto exportMenuAction = fileMenu.addMenu(exportMenu);
    exportMenuAction->setText(tr("&Export"));

    // Add "export to PNG image"-action
    const auto exportToPngAction = new QAction(tr("&PNG"), this);
    exportMenu->addAction(exportToPngAction);
    connect(exportToPngAction, &QAction::triggered, [=] {
        emit actionTriggered(StateMachine::Action::PngExportSelected);
    });

    exportMenu->addSeparator();

    // Add "export to SVG file"-action
    const auto exportToSvgAction = new QAction(tr("&SVG"), this);
    exportMenu->addAction(exportToSvgAction);
    connect(exportToSvgAction, &QAction::triggered, [=] {
        emit actionTriggered(StateMachine::Action::SvgExportSelected);
    });

    connect(&fileMenu, &QMenu::aboutToShow, [=] {
        exportToPngAction->setEnabled(m_mediator->hasNodes());
        exportToSvgAction->setEnabled(m_mediator->hasNodes());
    });
}

void MainWindow::createFileMenu()
{
    const auto fileMenu = menuBar()->addMenu(tr("&File"));

    // Add "new"-action
    const auto newAct = new QAction(tr("&New") + Constants::Misc::THREE_DOTS, this);
    newAct->setShortcut(QKeySequence(QKeySequence::New));
    fileMenu->addAction(newAct);
    connect(newAct, &QAction::triggered, [=] {
        emit actionTriggered(StateMachine::Action::NewSelected);
    });

    // Add "open"-action
    const auto openAct = new QAction(tr("&Open") + Constants::Misc::THREE_DOTS, this);
    openAct->setShortcut(QKeySequence(QKeySequence::Open));
    fileMenu->addAction(openAct);
    connect(openAct, &QAction::triggered, [=] {
        emit actionTriggered(StateMachine::Action::OpenSelected);
    });

    // Add "Recent Files"-menu
    const auto recentFilesMenu = new RecentFilesMenu;
    const auto recentFilesMenuAction = fileMenu->addMenu(recentFilesMenu);
    recentFilesMenuAction->setText(tr("Recent &Files"));
    connect(recentFilesMenu, &RecentFilesMenu::fileSelected, [=] {
        emit actionTriggered(StateMachine::Action::RecentFileSelected);
    });

    fileMenu->addSeparator();

    // Add "save"-action
    m_saveAction->setShortcut(QKeySequence(QKeySequence::Save));
    m_saveAction->setEnabled(false);
    fileMenu->addAction(m_saveAction);
    connect(m_saveAction, &QAction::triggered, [=] {
        emit actionTriggered(StateMachine::Action::SaveSelected);
    });

    // Add "save as"-action
    m_saveAsAction->setShortcut(QKeySequence(QKeySequence::SaveAs));
    m_saveAsAction->setEnabled(false);
    fileMenu->addAction(m_saveAsAction);
    connect(m_saveAsAction, &QAction::triggered, [=] {
        emit actionTriggered(StateMachine::Action::SaveAsSelected);
    });

    fileMenu->addSeparator();

    createExportSubMenu(*fileMenu);

    fileMenu->addSeparator();

    // Add "settings"-action
    const auto settingsAct = new QAction(tr("Settings") + Constants::Misc::THREE_DOTS, this);
    connect(settingsAct, &QAction::triggered, m_settingsDlg, &SettingsDialog::exec);
    fileMenu->addAction(settingsAct);

    fileMenu->addSeparator();

    // Add "quit"-action
    const auto quitAct = new QAction(tr("&Quit"), this);
    quitAct->setShortcut(QKeySequence(QKeySequence::Quit));
    fileMenu->addAction(quitAct);
    connect(quitAct, &QAction::triggered, [=] {
        emit actionTriggered(StateMachine::Action::QuitSelected);
    });

    connect(fileMenu, &QMenu::aboutToShow, [=] {
        recentFilesMenuAction->setEnabled(RecentFilesManager::instance().hasRecentFiles());
    });
}

void MainWindow::createHelpMenu()
{
    const auto helpMenu = menuBar()->addMenu(tr("&Help"));

    // Add "about"-action
    const auto aboutAct = new QAction(tr("&About"), this);
    helpMenu->addAction(aboutAct);
    connect(aboutAct, &QAction::triggered, [=] {
        m_aboutDlg->exec();
    });

    // Add "about Qt"-action
    const auto aboutQtAct = new QAction(tr("About &Qt"), this);
    helpMenu->addAction(aboutQtAct);
    connect(aboutQtAct, &QAction::triggered, [=] {
        QMessageBox::aboutQt(this, tr("About Qt"));
    });

    helpMenu->addSeparator();

    // Add "What's new"-action
    const auto whatsNewAct = new QAction(tr("What's New"), this);
    helpMenu->addAction(whatsNewAct);
    connect(whatsNewAct, &QAction::triggered, [=] {
        m_whatsNewDlg->resize(3 * width() / 5, 3 * height() / 5);
        m_whatsNewDlg->exec();
    });
}

void MainWindow::createToolBar()
{
    connect(m_toolBar, &ToolBar::edgeWidthChanged, this, &MainWindow::edgeWidthChanged);

    connect(m_toolBar, &ToolBar::fontChanged, this, &MainWindow::fontChanged);

    connect(m_toolBar, &ToolBar::textSizeChanged, this, &MainWindow::textSizeChanged);

    m_toolBar->addAction(createCornerRadiusAction());
    m_toolBar->addSeparator();
    m_toolBar->addAction(createGridSizeAction());

    const auto spacer = new QWidget;
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_toolBar->addWidget(spacer);
    m_toolBar->addAction(createSearchAction());
    m_toolBar->addSeparator();
    m_toolBar->addWidget(m_showGridCheckBox);
    m_toolBar->addWidget(m_copyOnDragCheckBox);

    connect(m_showGridCheckBox, &QCheckBox::stateChanged, this, &MainWindow::gridVisibleChanged);
    connect(m_showGridCheckBox, &QCheckBox::stateChanged, Settings::saveGridVisibleState);

    m_showGridCheckBox->setCheckState(Settings::loadGridVisibleState());
}

void MainWindow::createViewMenu()
{
    const auto viewMenu = menuBar()->addMenu(tr("&View"));

    // Add "fullScreen"-action
    m_fullScreenAction = new QAction(tr("Full Screen"), this);
    m_fullScreenAction->setCheckable(true);
    m_fullScreenAction->setChecked(false);
    m_fullScreenAction->setShortcut(QKeySequence("F11"));
    viewMenu->addAction(m_fullScreenAction);
    connect(m_fullScreenAction, &QAction::triggered, [=](bool checked) {
        if (checked) {
            Settings::saveFullScreen(true);
            showFullScreen();
        } else {
            Settings::saveFullScreen(false);
            showNormal();
            resize(Settings::loadWindowSize(calculateDefaultWindowSize().second));
        }
    });

    viewMenu->addSeparator();

    // Add "zoom in"-action
    const auto zoomIn = new QAction(tr("Zoom In"), this);
    viewMenu->addAction(zoomIn);
    zoomIn->setShortcut(QKeySequence(QKeySequence::ZoomIn));
    connect(zoomIn, &QAction::triggered, this, &MainWindow::zoomInTriggered);

    // Add "zoom out"-action
    const auto zoomOut = new QAction(tr("Zoom Out"), this);
    viewMenu->addAction(zoomOut);
    connect(zoomOut, &QAction::triggered, this, &MainWindow::zoomOutTriggered);
    zoomOut->setShortcut(QKeySequence(QKeySequence::ZoomOut));

    viewMenu->addSeparator();

    // Add "zoom to fit"-action
    const auto zoomToFit = new QAction(tr("&Zoom To Fit"), this);
    zoomToFit->setShortcut(QKeySequence("Ctrl+0"));
    viewMenu->addAction(zoomToFit);
    connect(zoomToFit, &QAction::triggered, this, &MainWindow::zoomToFitTriggered);

    connect(viewMenu, &QMenu::aboutToShow, [=] {
        zoomToFit->setEnabled(m_mediator->hasNodes());
    });
}

std::pair<QSize, QSize> MainWindow::calculateDefaultWindowSize() const
{
    // Detect screen dimensions
    const auto screen = QGuiApplication::primaryScreen();
    const auto screenGeometry = screen->geometry();
    const int height = screenGeometry.height();
    const int width = screenGeometry.width();
    const double defaultScale = 0.8;
    return { QSize(width, height), QSize(width, height) * defaultScale };
}

void MainWindow::initialize()
{
    // Read dialog size data
    const auto screenAndWindow = calculateDefaultWindowSize();
    const auto lastSize = Settings::loadWindowSize(screenAndWindow.second);
    if (screenAndWindow.first.width() < lastSize.width()
        || screenAndWindow.first.height() < lastSize.height()) {
        resize(screenAndWindow.second);
        Settings::saveWindowSize(size());
    } else {
        resize(lastSize);
    }

    // Try to center the window.
    move(screenAndWindow.first.width() / 2 - this->width() / 2, screenAndWindow.first.height() / 2 - this->height() / 2);

    populateMenuBar();

    createToolBar();

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
    if (m_mediator->isModified()) {
        setWindowTitle(appInfo + " - " + displayFileName + " - " + tr("Not Saved"));
    } else {
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

    createViewMenu();

    createHelpMenu();
}

void MainWindow::appear()
{
    if (Settings::loadFullScreen()) {
        m_fullScreenAction->trigger();
    } else {
        show();
    }
}

bool MainWindow::copyOnDragEnabled() const
{
    return m_copyOnDragCheckBox->isChecked();
}

void MainWindow::disableUndoAndRedo()
{
    m_undoAction->setEnabled(false);
    m_redoAction->setEnabled(false);
}

void MainWindow::enableConnectSelectedNodesAction(bool enable)
{
    m_connectSelectedNodesAction->setEnabled(enable);
}

void MainWindow::enableDisconnectSelectedNodesAction(bool enable)
{
    m_disconnectSelectedNodesAction->setEnabled(enable);
}

void MainWindow::enableWidgetSignals(bool enable)
{
    m_toolBar->enableWidgetSignals(enable);

    m_cornerRadiusSpinBox->blockSignals(!enable);
    m_gridSizeSpinBox->blockSignals(!enable);
}

void MainWindow::setCornerRadius(int value)
{
    if (m_cornerRadiusSpinBox->value() != value) {
        m_cornerRadiusSpinBox->setValue(value);
    }
}

void MainWindow::setEdgeWidth(double value)
{
    m_toolBar->setEdgeWidth(value);
}

void MainWindow::setTextSize(int textSize)
{
    m_toolBar->setTextSize(textSize);
}

void MainWindow::enableUndo(bool enable)
{
    m_undoAction->setEnabled(enable);
}

void MainWindow::enableRedo(bool enable)
{
    m_redoAction->setEnabled(enable);
}

void MainWindow::enableSave(bool enable)
{
    setTitle();

    m_saveAction->setEnabled(enable);
}

void MainWindow::saveWindowSize()
{
    if (!m_fullScreenAction->isChecked()) {
        Settings::saveWindowSize(size());
    }
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
    m_saveAction->setEnabled(true);
    m_saveAsAction->setEnabled(true);
}

void MainWindow::setSaveActionStatesOnOpenedMindMap()
{
    m_saveAction->setEnabled(false);
    m_saveAsAction->setEnabled(true);
}

MainWindow::~MainWindow() = default;
