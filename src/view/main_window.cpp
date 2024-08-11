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
#include "../application/service_container.hpp"
#include "../common/constants.hpp"
#include "../infra/settings.hpp"

#include "dialogs/about_dialog.hpp"
#include "dialogs/settings_dialog.hpp"
#include "dialogs/spinner_dialog.hpp"
#include "dialogs/whats_new_dialog.hpp"

#include "menus/main_menu.hpp"
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

static const auto TAG = "MainWindow";

MainWindow::MainWindow()
  : m_spinnerDlg(new Dialogs::SpinnerDialog(this))
  , m_mainMenu(new Menus::MainMenu)
  , m_toolBar(new Menus::ToolBar)
  , m_statusText(new QLabel(this))
{
    addToolBar(Qt::BottomToolBarArea, m_toolBar);

    connectToolBar();

    setMenuBar(m_mainMenu);

    connectMainMenu();

    m_statusText->setOpenExternalLinks(true);

    juzzlin::L(TAG).debug() << "MainWindow instantiated";
}

void MainWindow::changeFont(const QFont & font)
{
    m_toolBar->changeFont(font);
}

void MainWindow::connectMainMenu()
{
    connect(m_mainMenu, &Menus::MainMenu::aboutDialogRequested, this, [] {
        Dialogs::AboutDialog {}.exec();
    });

    connect(m_mainMenu, &Menus::MainMenu::aboutQtDialogRequested, this, [=] {
        QMessageBox::aboutQt(this, tr("About Qt"));
    });

    connect(m_mainMenu, &Menus::MainMenu::actionTriggered, this, &MainWindow::actionTriggered);

    connect(m_mainMenu, &Menus::MainMenu::fullScreenChanged, this, [=](bool fullScreen) {
        Settings::Custom::saveFullScreen(fullScreen);
        if (fullScreen) {
            showFullScreen();
        } else {
            showNormal();
            resize(Settings::Custom::loadWindowSize(defaultWindowSize()));
        }
    });

    connect(m_mainMenu, &Menus::MainMenu::settingsDialogRequested, this, [=] {
        Dialogs::SettingsDialog settingsDialog;
        connect(&settingsDialog, &Dialogs::SettingsDialog::shadowEffectChanged, this, &MainWindow::shadowEffectChanged);
        connect(&settingsDialog, &Dialogs::SettingsDialog::autosaveEnabled, this, &MainWindow::autosaveEnabled);
        settingsDialog.exec();
    });

    connect(m_mainMenu, &Menus::MainMenu::zoomInRequested, this, &MainWindow::zoomInRequested);

    connect(m_mainMenu, &Menus::MainMenu::zoomOutRequested, this, &MainWindow::zoomOutRequested);

    connect(m_mainMenu, &Menus::MainMenu::zoomToFitRequested, this, &MainWindow::zoomToFitRequested);

    connect(m_mainMenu, &Menus::MainMenu::whatsNewDialogRequested, this, [=] {
        Dialogs::WhatsNewDialog whatsNewDialog;
        whatsNewDialog.resize(3 * width() / 5, 3 * height() / 5);
        whatsNewDialog.exec();
    });
}

void MainWindow::connectToolBar()
{
    connect(m_toolBar, &Menus::ToolBar::arrowSizeChanged, this, &MainWindow::arrowSizeChanged);

    connect(m_toolBar, &Menus::ToolBar::cornerRadiusChanged, this, &MainWindow::cornerRadiusChanged);

    connect(m_toolBar, &Menus::ToolBar::edgeWidthChanged, this, &MainWindow::edgeWidthChanged);

    connect(m_toolBar, &Menus::ToolBar::fontChanged, this, &MainWindow::fontChanged);

    connect(m_toolBar, &Menus::ToolBar::gridSizeChanged, this, &MainWindow::gridSizeChanged);

    connect(m_toolBar, &Menus::ToolBar::gridVisibleChanged, this, &MainWindow::gridVisibleChanged);

    connect(m_toolBar, &Menus::ToolBar::searchTextChanged, this, &MainWindow::searchTextChanged);

    connect(m_toolBar, &Menus::ToolBar::textSizeChanged, this, &MainWindow::textSizeChanged);
}

void MainWindow::centerWindow()
{
    move(screenSize().width() / 2 - width() / 2, screenSize().height() / 2 - height() / 2);
}

QSize MainWindow::defaultWindowSize() const
{
    const double defaultScale = 0.8;
    return screenSize() * defaultScale;
}

QSize MainWindow::screenSize() const
{
    return QGuiApplication::primaryScreen()->geometry().size();
}

void MainWindow::resizeWindow()
{
    if (const auto lastSize = Settings::Custom::loadWindowSize(defaultWindowSize()); screenSize().width() < lastSize.width()
        || screenSize().height() < lastSize.height()) {
        resize(defaultWindowSize());
        Settings::Custom::saveWindowSize(size());
    } else {
        resize(lastSize);
    }
}

void MainWindow::initialize()
{
    resizeWindow();

    centerWindow();

    m_mainMenu->initialize();

    m_toolBar->loadSettings();

    setFocus(); // Without this the tool bar has focus when application starts

    setWindowIcon(QIcon(":/heimer.png"));

    emit actionTriggered(StateMachine::Action::MainWindowInitialized);
}

void MainWindow::setTitle()
{
    const auto appInfo = QString { "%1 %2" }.arg(Constants::Application::applicationName(), Constants::Application::applicationVersion());
    const auto displayFileName = SC::instance().applicationService()->fileName().isEmpty() ? tr("New File") : SC::instance().applicationService()->fileName();
    if (SC::instance().applicationService()->isModified()) {
        setWindowTitle(QString { "%1 - %2 - %3" }.arg(appInfo, displayFileName, tr("Not Saved")));
    } else {
        setWindowTitle(QString { "%1 - %2" }.arg(appInfo, displayFileName));
    }
}

void MainWindow::closeEvent(QCloseEvent * event)
{
    event->ignore();
    emit actionTriggered(StateMachine::Action::QuitSelected);
}

void MainWindow::appear()
{
    if (Settings::Custom::loadFullScreen()) {
        m_mainMenu->triggerFullScreenAction();
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
    m_mainMenu->setUndoActionEnabled(false);
    m_mainMenu->setRedoActionEnabled(false);
}

void MainWindow::enableConnectSelectedNodesAction(bool enable)
{
    m_mainMenu->setConnectSelectedNodesActionEnabled(enable);
}

void MainWindow::enableDisconnectSelectedNodesAction(bool enable)
{
    m_mainMenu->setDisconnectSelectedNodesActionEnabled(enable);
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
    m_mainMenu->setUndoActionEnabled(enable);
}

void MainWindow::enableRedo(bool enable)
{
    m_mainMenu->setRedoActionEnabled(enable);
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
    if (!m_mainMenu->isFullScreenActionChecked()) {
        Settings::Custom::saveWindowSize(size());
    }
}

void MainWindow::showErrorDialog(QString message)
{
    QMessageBox::critical(this, Constants::Application::applicationName(), message, QMessageBox::Ok);
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
