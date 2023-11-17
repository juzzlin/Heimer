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

#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <QCloseEvent>
#include <QGraphicsScene>
#include <QMainWindow>
#include <QString>
#include <QTimer>

#include "state_machine.hpp"
#include "types.hpp"

#include <map>
#include <memory>

namespace Dialogs {
class AboutDialog;
class SettingsDialog;
class SpinnerDialog;
class WhatsNewDialog;
} // namespace Dialogs

namespace Menus {
class ToolBar;
} // namespace Menus

class ControlStrategy;
class EditorService;
class EditorView;
class ApplicationService;
class Node;

class QAction;
class QCheckBox;
class QFont;
class QLabel;
class QLineEdit;
class QSlider;
class QSpinBox;
class QTextEdit;
class QWidgetAction;

struct ShadowEffectParams;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

    ~MainWindow() override;

    static MainWindow * instance();

    void appear();

    bool copyOnDragEnabled() const;

    void disableUndoAndRedo();

    void enableConnectSelectedNodesAction(bool enable);

    void enableDisconnectSelectedNodesAction(bool enable);

    void enableWidgetSignals(bool enable);

    void initialize();

    void initializeNewMindMap();

    void saveWindowSize();

    void setApplicationService(std::shared_ptr<ApplicationService> applicationService);

    void setSaveActionStatesOnNewMindMap();

    void setSaveActionStatesOnOpenedMindMap();

    void setTitle();

public slots:
    void changeFont(const QFont & font);

    void enableUndo(bool enable);

    void enableRedo(bool enable);

    void enableSave(bool enable);

    void setArrowSize(double value);

    void setCornerRadius(int value);

    void setEdgeWidth(double value);

    void setTextSize(int textSize);

    void showErrorDialog(QString message);

    void showSpinnerDialog(bool show, QString message = {});

protected:
    void closeEvent(QCloseEvent * event) override;

signals:
    void actionTriggered(StateMachine::Action action);

    void arrowSizeChanged(double arrowSize);

    void autosaveEnabled(bool enabled);

    void cornerRadiusChanged(int size);

    void edgeWidthChanged(double width);

    void fontChanged(const QFont & font);

    void gridSizeChanged(int size, bool autoSnap);

    void gridVisibleChanged(int state);

    void searchTextChanged(QString text);

    void shadowEffectChanged(const ShadowEffectParams & params);

    void textSizeChanged(int value);

    void zoomInTriggered();

    void zoomOutTriggered();

    void zoomToFitTriggered();

private:
    void addConnectSelectedNodesAction(QMenu & menu);

    void addDisconnectSelectedNodesAction(QMenu & menu);

    void addRedoAction(QMenu & menu);

    void addUndoAction(QMenu & menu);

    std::pair<QSize, QSize> calculateDefaultWindowSize() const;

    void createColorSubMenu(QMenu & editMenu);

    void createEditMenu();

    void createExportSubMenu(QMenu & fileMenu);

    void createFileMenu();

    void createHelpMenu();

    void createMirrorSubMenu(QMenu & editMenu);

    void connectToolBar();

    void createViewMenu();

    void populateMenuBar();

    Dialogs::AboutDialog * m_aboutDlg;

    Dialogs::SettingsDialog * m_settingsDlg;

    Dialogs::SpinnerDialog * m_spinnerDlg;

    Menus::ToolBar * m_toolBar;

    Dialogs::WhatsNewDialog * m_whatsNewDlg;

    QAction * m_connectSelectedNodesAction = nullptr;

    QAction * m_disconnectSelectedNodesAction = nullptr;

    QAction * m_fullScreenAction = nullptr;

    QAction * m_saveAction = nullptr;

    QAction * m_saveAsAction = nullptr;

    QAction * m_undoAction = nullptr;

    QAction * m_redoAction = nullptr;

    QString m_argMindMapFile;

    std::shared_ptr<ApplicationService> m_applicationService;

    bool m_closeNow = false;

    QLabel * m_statusText;

    ControlStrategyS m_controlStrategy;

    static MainWindow * m_instance;
};

#endif // MAIN_WINDOW_HPP
