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

#include "state_machine.hpp"

#include <memory>

class AboutDlg;
class EditorData;
class EditorView;
class QAction;
class QCheckBox;
class QDoubleSpinBox;
class QSlider;
class QSpinBox;
class QTextEdit;
class QWidgetAction;
class Mediator;
class Node;
class WhatsNewDlg;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

    ~MainWindow() override;

    static MainWindow * instance();

    bool copyOnDragEnabled() const;

    void disableUndoAndRedo();

    void initialize();

    void initializeNewMindMap();

    void saveWindowSize();

    void setMediator(std::shared_ptr<Mediator> mediator);

    void setSaveActionStatesOnNewMindMap();

    void setSaveActionStatesOnOpenedMindMap();

    void setTitle();

public slots:

    void enableUndo(bool enable);

    void enableSave(bool enable);

    void setCornerRadius(int value);

    void setEdgeWidth(double value);

    void setTextSize(int textSize);

    void showErrorDialog(QString message);

protected:
    void closeEvent(QCloseEvent * event) override;

signals:

    void actionTriggered(StateMachine::Action action);

    void cornerRadiusChanged(int size);

    void edgeWidthChanged(double width);

    void gridSizeChanged(int size);

    void textSizeChanged(int value);

    void zoomInTriggered();

    void zoomOutTriggered();

    void zoomToFitTriggered();

private slots:

    void setupMindMapAfterUndoOrRedo();

    void showAboutDlg();

    void showAboutQtDlg();

    void showWhatsNewDlg();

private:
    void addRedoAction(QMenu & menu);

    void addUndoAction(QMenu & menu);

    QWidgetAction * createCornerRadiusAction();

    QWidgetAction * createEdgeWidthAction();

    QWidgetAction * createGridSizeAction();

    QWidgetAction * createTextSizeAction();

    void createEditMenu();

    void createExportSubMenu(QMenu & fileMenu);

    void createFileMenu();

    void createHelpMenu();

    void createToolBar();

    void createViewMenu();

    void populateMenuBar();

    AboutDlg * m_aboutDlg;

    WhatsNewDlg * m_whatsNewDlg;

    QAction * m_saveAction = nullptr;

    QAction * m_saveAsAction = nullptr;

    QAction * m_undoAction = nullptr;

    QAction * m_redoAction = nullptr;

    QDoubleSpinBox * m_edgeWidthSpinBox = nullptr;

    QSpinBox * m_cornerRadiusSpinBox = nullptr;

    QSpinBox * m_gridSizeSpinBox = nullptr;

    QSpinBox * m_textSizeSpinBox = nullptr;

    QCheckBox * m_copyOnDragCheckBox = nullptr;

    QString m_argMindMapFile;

    std::shared_ptr<Mediator> m_mediator;

    const char * m_settingsGroup = "MainWindow";

    bool m_closeNow = false;

    QSize m_sizeBeforeFullScreen;

    static MainWindow * m_instance;
};

#endif // MAIN_WINDOW_HPP
