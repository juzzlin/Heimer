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

#include <map>
#include <memory>

class AboutDialog;
class SettingsDialog;
class EditorData;
class EditorView;
class Mediator;
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
class ToolBar;
class WhatsNewDialog;

namespace GraphicsFactory {
struct ShadowEffectParams;
}

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

    void setMediator(std::shared_ptr<Mediator> mediator);

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

    void showStatusText(QString text);

protected:
    void closeEvent(QCloseEvent * event) override;

signals:

    void actionTriggered(StateMachine::Action action);

    void arrowSizeChanged(double arrowSize);

    void cornerRadiusChanged(int size);

    void edgeWidthChanged(double width);

    void fontChanged(const QFont & font);

    void gridSizeChanged(int size, bool autoSnap);

    void gridVisibleChanged(int state);

    void searchTextChanged(QString text);

    void shadowEffectChanged(const GraphicsFactory::ShadowEffectParams & params);

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

    void createEditMenu();

    void createExportSubMenu(QMenu & fileMenu);

    void createFileMenu();

    void createHelpMenu();

    void connectToolBar();

    void createViewMenu();

    void populateMenuBar();

    AboutDialog * m_aboutDlg;

    SettingsDialog * m_settingsDlg;

    ToolBar * m_toolBar;

    WhatsNewDialog * m_whatsNewDlg;

    QAction * m_connectSelectedNodesAction = nullptr;

    QAction * m_disconnectSelectedNodesAction = nullptr;

    QAction * m_fullScreenAction = nullptr;

    QAction * m_saveAction = nullptr;

    QAction * m_saveAsAction = nullptr;

    QAction * m_undoAction = nullptr;

    QAction * m_redoAction = nullptr;

    QString m_argMindMapFile;

    std::shared_ptr<Mediator> m_mediator;

    bool m_closeNow = false;

    QLabel * m_statusText;

    static MainWindow * m_instance;
};

#endif // MAIN_WINDOW_HPP
