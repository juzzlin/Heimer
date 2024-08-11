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

#include "../application/state_machine.hpp"
#include "../common/types.hpp"

namespace Dialogs {
class SpinnerDialog;
} // namespace Dialogs

namespace Menus {
class MainMenu;
class ToolBar;
} // namespace Menus

class EditorService;
class EditorView;
class Node;

class QCheckBox;
class QFont;
class QLabel;
class QLineEdit;
class QSlider;
class QSpinBox;
class QTextEdit;
class QWidgetAction;

class ShadowEffectParams;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

    ~MainWindow() override;

    void appear();

    bool copyOnDragEnabled() const;

    void disableUndoAndRedo();

    void enableConnectSelectedNodesAction(bool enable);

    void enableDisconnectSelectedNodesAction(bool enable);

    void enableWidgetSignals(bool enable);

    void initialize();

    void initializeNewMindMap();

    void saveWindowSize();

    void setSaveActionStatesOnNewMindMap();

    void setSaveActionStatesOnOpenedMindMap();

    void setTitle();

public slots:
    void changeFont(const QFont & font);

    void enableUndo(bool enable);

    void enableRedo(bool enable);

    void enableSave(bool enable);

    void requestCurrentSearchText();

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

    void zoomInRequested();

    void zoomOutRequested();

    void zoomToFitRequested();

private:
    void centerWindow();

    void connectMainMenu();

    void connectToolBar();

    QSize defaultWindowSize() const;

    QSize screenSize() const;

    void resizeWindow();

    Dialogs::SpinnerDialog * m_spinnerDlg;

    Menus::MainMenu * m_mainMenu;

    Menus::ToolBar * m_toolBar;

    QString m_argMindMapFile;

    bool m_closeNow = false;

    QLabel * m_statusText;
};

#endif // MAIN_WINDOW_HPP
