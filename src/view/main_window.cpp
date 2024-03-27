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

#include "../application/application_service.hpp"
#include "../application/control_strategy.hpp"
#include "../application/recent_files_manager.hpp"
#include "../application/service_container.hpp"
#include "../common/constants.hpp"
#include "../infra/settings.hpp"
#include "node_action.hpp"

#include "dialogs/about_dialog.hpp"
#include "dialogs/settings_dialog.hpp"
#include "dialogs/spinner_dialog.hpp"
#include "dialogs/whats_new_dialog.hpp"

#include "menus/main_menu.hpp"
#include "menus/recent_files_menu.hpp"
#include "menus/tool_bar.hpp"

#include "simple_logger.hpp"

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
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidgetAction>

MainWindow::MainWindow()
  : m_aboutDlg(new Dialogs::AboutDialog(this))
  , m_settingsDlg(new Dialogs::SettingsDialog(this))
  , m_spinnerDlg(new Dialogs::SpinnerDialog(this))
  , m_mainMenu(new Menus::MainMenu)
  , m_toolBar(new Menus::ToolBar)
  , m_whatsNewDlg(new Dialogs::WhatsNewDialog(this))
  , m_connectSelectedNodesAction(new QAction(tr("Connect selected nodes"), this))
  , m_disconnectSelectedNodesAction(new QAction(tr("Disconnect selected nodes"), this))
  , m_undoAction(new QAction(tr("Undo"), this))
  , m_redoAction(new QAction(tr("Redo"), this))
  , m_statusText(new QLabel(this))
{
    addToolBar(Qt::BottomToolBarArea, m_toolBar);

    setMenuBar(m_mainMenu);

    connectToolBar();

    connect(m_settingsDlg, &Dialogs::SettingsDialog::shadowEffectChanged, this, &MainWindow::shadowEffectChanged);

    connect(m_settingsDlg, &Dialogs::SettingsDialog::autosaveEnabled, this, &MainWindow::autosaveEnabled);

    m_statusText->setOpenExternalLinks(true);

    juzzlin::L().debug() << "MainWindow instantiated";
}

void MainWindow::addConnectSelectedNodesAction(QMenu & menu)
{
    m_connectSelectedNodesAction->setShortcut(QKeySequence("Ctrl+Shift+C"));
    connect(m_connectSelectedNodesAction, &QAction::triggered, this, [] {
        juzzlin::L().debug() << "Connect selected triggered";
        SC::instance().applicationService()->performNodeAction({ NodeAction::Type::ConnectSelected });
    });
    menu.addAction(m_connectSelectedNodesAction);
    connect(&menu, &QMenu::aboutToShow, this, [=] {
        m_connectSelectedNodesAction->setEnabled(SC::instance().applicationService()->areSelectedNodesConnectable());
    });
}

void MainWindow::addDisconnectSelectedNodesAction(QMenu & menu)
{
    m_disconnectSelectedNodesAction->setShortcut(QKeySequence("Ctrl+Shift+D"));
    connect(m_disconnectSelectedNodesAction, &QAction::triggered, this, [] {
        juzzlin::L().debug() << "Disconnect selected triggered";
        SC::instance().applicationService()->performNodeAction({ NodeAction::Type::DisconnectSelected });
    });
    menu.addAction(m_disconnectSelectedNodesAction);
    connect(&menu, &QMenu::aboutToShow, this, [=] {
        m_disconnectSelectedNodesAction->setEnabled(SC::instance().applicationService()->areSelectedNodesDisconnectable());
    });
}

void MainWindow::addRedoAction(QMenu & menu)
{
    m_redoAction->setShortcut(QKeySequence(QKeySequence::Redo));

    connect(m_redoAction, &QAction::triggered, SC::instance().applicationService().get(), &ApplicationService::redo);

    m_redoAction->setEnabled(false);

    menu.addAction(m_redoAction);
}

void MainWindow::addUndoAction(QMenu & menu)
{
    m_undoAction->setShortcut(QKeySequence(QKeySequence::Undo));

    connect(m_undoAction, &QAction::triggered, SC::instance().applicationService().get(), &ApplicationService::undo);

    m_undoAction->setEnabled(false);

    menu.addAction(m_undoAction);
}

void MainWindow::changeFont(const QFont & font)
{
    m_toolBar->changeFont(font);
}

void MainWindow::connectToolBar()
{
    connect(m_toolBar, &Menus::ToolBar::gridVisibleChanged, this, &MainWindow::gridVisibleChanged);

    connect(m_toolBar, &Menus::ToolBar::arrowSizeChanged, this, &MainWindow::arrowSizeChanged);

    connect(m_toolBar, &Menus::ToolBar::cornerRadiusChanged, this, &MainWindow::cornerRadiusChanged);

    connect(m_toolBar, &Menus::ToolBar::edgeWidthChanged, this, &MainWindow::edgeWidthChanged);

    connect(m_toolBar, &Menus::ToolBar::fontChanged, this, &MainWindow::fontChanged);

    connect(m_toolBar, &Menus::ToolBar::gridSizeChanged, this, &MainWindow::gridSizeChanged);

    connect(m_toolBar, &Menus::ToolBar::gridVisibleChanged, this, &MainWindow::gridVisibleChanged);

    connect(m_toolBar, &Menus::ToolBar::searchTextChanged, this, &MainWindow::searchTextChanged);

    connect(m_toolBar, &Menus::ToolBar::textSizeChanged, this, &MainWindow::textSizeChanged);
}

void MainWindow::createColorSubMenu(QMenu & editMenu)
{
    const auto colorMenu = new QMenu;
    const auto colorMenuAction = editMenu.addMenu(colorMenu);
    colorMenuAction->setText(tr("General &colors"));

    const auto backgroundColorAction = new QAction(tr("Set background color") + Constants::Misc::THREE_DOTS, this);
    backgroundColorAction->setShortcut(QKeySequence("Ctrl+B"));

    connect(backgroundColorAction, &QAction::triggered, this, [=] {
        emit actionTriggered(StateMachine::Action::BackgroundColorChangeRequested);
    });

    colorMenu->addAction(backgroundColorAction);

    colorMenu->addSeparator();

    const auto edgeColorAction = new QAction(tr("Set edge color") + Constants::Misc::THREE_DOTS, this);
    edgeColorAction->setShortcut(QKeySequence("Ctrl+E"));

    connect(edgeColorAction, &QAction::triggered, this, [=] {
        emit actionTriggered(StateMachine::Action::EdgeColorChangeRequested);
    });

    colorMenu->addAction(edgeColorAction);

    colorMenu->addSeparator();

    const auto gridColorAction = new QAction(tr("Set grid color") + Constants::Misc::THREE_DOTS, this);
    gridColorAction->setShortcut(QKeySequence("Ctrl+G"));

    connect(gridColorAction, &QAction::triggered, this, [=] {
        emit actionTriggered(StateMachine::Action::GridColorChangeRequested);
    });

    colorMenu->addAction(gridColorAction);
}

void MainWindow::createMirrorSubMenu(QMenu & editMenu)
{
    const auto mirrorMenu = new QMenu;
    auto mirrorAction = editMenu.addMenu(mirrorMenu);
    mirrorAction->setText(tr("&Mirror layout"));

    const auto mirrorHorizontallyAction = new QAction(tr("Horizontally"), this);
    mirrorHorizontallyAction->setShortcut(QKeySequence(SC::instance().controlStrategy()->mirrorLayoutHorizontallyShortcut()));
    mirrorMenu->addAction(mirrorHorizontallyAction);
    connect(mirrorHorizontallyAction, &QAction::triggered, this, [=] {
        SC::instance().applicationService()->performNodeAction(NodeAction::Type::MirrorLayoutHorizontally);
    });
    mirrorMenu->addSeparator();

    const auto mirrorVerticallyAction = new QAction(tr("Vertically"), this);
    mirrorVerticallyAction->setShortcut(QKeySequence(SC::instance().controlStrategy()->mirrorLayoutVerticallyShortcut()));
    mirrorMenu->addAction(mirrorVerticallyAction);
    connect(mirrorVerticallyAction, &QAction::triggered, this, [=] {
        SC::instance().applicationService()->performNodeAction(NodeAction::Type::MirrorLayoutVertically);
    });
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

    createColorSubMenu(*editMenu);

    editMenu->addSeparator();

    createMirrorSubMenu(*editMenu);

    editMenu->addSeparator();

    auto optimizeLayoutAction = new QAction(tr("Optimize layout") + Constants::Misc::THREE_DOTS, this);
    optimizeLayoutAction->setShortcut(QKeySequence("Ctrl+Shift+O"));

    connect(optimizeLayoutAction, &QAction::triggered, this, [=] {
        emit actionTriggered(StateMachine::Action::LayoutOptimizationRequested);
    });

    editMenu->addAction(optimizeLayoutAction);
}

void MainWindow::createHelpMenu()
{
    const auto helpMenu = menuBar()->addMenu(tr("&Help"));

    // Add "about"-action
    const auto aboutAct = new QAction(tr("&About"), this);
    helpMenu->addAction(aboutAct);
    connect(aboutAct, &QAction::triggered, m_aboutDlg, &Dialogs::AboutDialog::exec);

    // Add "about Qt"-action
    const auto aboutQtAct = new QAction(tr("About &Qt"), this);
    helpMenu->addAction(aboutQtAct);
    connect(aboutQtAct, &QAction::triggered, this, [=] {
        QMessageBox::aboutQt(this, tr("About Qt"));
    });

    helpMenu->addSeparator();

    // Add "What's new"-action
    const auto whatsNewAct = new QAction(tr("What's New"), this);
    helpMenu->addAction(whatsNewAct);
    connect(whatsNewAct, &QAction::triggered, this, [=] {
        m_whatsNewDlg->resize(3 * width() / 5, 3 * height() / 5);
        m_whatsNewDlg->exec();
    });
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
    connect(m_fullScreenAction, &QAction::triggered, this, [=](bool checked) {
        if (checked) {
            Settings::Custom::saveFullScreen(true);
            showFullScreen();
        } else {
            Settings::Custom::saveFullScreen(false);
            showNormal();
            resize(Settings::Custom::loadWindowSize(calculateDefaultWindowSize().second));
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

    connect(viewMenu, &QMenu::aboutToShow, this, [=] {
        zoomToFit->setEnabled(SC::instance().applicationService()->hasNodes());
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
    const auto lastSize = Settings::Custom::loadWindowSize(screenAndWindow.second);
    if (screenAndWindow.first.width() < lastSize.width()
        || screenAndWindow.first.height() < lastSize.height()) {
        resize(screenAndWindow.second);
        Settings::Custom::saveWindowSize(size());
    } else {
        resize(lastSize);
    }

    // Try to center the window.
    move(screenAndWindow.first.width() / 2 - this->width() / 2, screenAndWindow.first.height() / 2 - this->height() / 2);

    populateMenuBar();

    m_toolBar->loadSettings();

    setFocus(); // Without this the tool bar has focus when application starts

    setWindowIcon(QIcon(":/heimer.png"));

    emit actionTriggered(StateMachine::Action::MainWindowInitialized);
}

void MainWindow::setTitle()
{
    const auto appInfo = QString(Constants::Application::APPLICATION_NAME) + " " + Constants::Application::APPLICATION_VERSION;
    const auto displayFileName = SC::instance().applicationService()->fileName().isEmpty() ? tr("New File") : SC::instance().applicationService()->fileName();
    if (SC::instance().applicationService()->isModified()) {
        setWindowTitle(appInfo + " - " + displayFileName + " - " + tr("Not Saved"));
    } else {
        setWindowTitle(appInfo + " - " + displayFileName);
    }
}

void MainWindow::closeEvent(QCloseEvent * event)
{
    event->ignore();
    emit actionTriggered(StateMachine::Action::QuitSelected);
}

void MainWindow::populateMenuBar()
{
    createEditMenu();

    createViewMenu();

    createHelpMenu();
}

void MainWindow::appear()
{
    if (Settings::Custom::loadFullScreen()) {
        m_fullScreenAction->trigger();
    } else {
        show();
    }
}

bool MainWindow::copyOnDragEnabled() const
{
    return m_toolBar->copyOnDragEnabled();
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
}

void MainWindow::setArrowSize(double value)
{
    m_toolBar->setArrowSize(value);
}

void MainWindow::setCornerRadius(int value)
{
    m_toolBar->setCornerRadius(value);
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

    m_mainMenu->setSaveActionEnabled(enable);
}

void MainWindow::requestCurrentSearchText()
{
    m_toolBar->requestCurrentSearchText();
}

void MainWindow::saveWindowSize()
{
    if (!m_fullScreenAction->isChecked()) {
        Settings::Custom::saveWindowSize(size());
    }
}

void MainWindow::showErrorDialog(QString message)
{
    QMessageBox::critical(this, Constants::Application::APPLICATION_NAME, message, QMessageBox::Ok);
}

void MainWindow::showSpinnerDialog(bool show, QString message)
{
    if (show) {
        m_spinnerDlg->setMessage(message);
        m_spinnerDlg->show();
    } else {
        m_spinnerDlg->hide();
    }
}

void MainWindow::initializeNewMindMap()
{
    disableUndoAndRedo();
    setSaveActionStatesOnNewMindMap();

    emit actionTriggered(StateMachine::Action::NewMindMapInitialized);
}

void MainWindow::setSaveActionStatesOnNewMindMap()
{
    m_mainMenu->setSaveActionEnabled(true);
    m_mainMenu->setSaveAsActionEnabled(true);
}

void MainWindow::setSaveActionStatesOnOpenedMindMap()
{
    m_mainMenu->setSaveActionEnabled(false);
    m_mainMenu->setSaveAsActionEnabled(true);
}

MainWindow::~MainWindow() = default;
