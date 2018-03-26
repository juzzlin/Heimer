// This file is part of Dementia.
// Copyright (C) 2018 Jussi Lind <jussi.lind@iki.fi>
//
// Dementia is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dementia is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dementia. If not, see <http://www.gnu.org/licenses/>.

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QCloseEvent>
#include <QGraphicsScene>
#include <QString>

#include <functional>

class AboutDlg;
class EditorData;
class EditorView;
class QAction;
class QCheckBox;
class QSlider;
class QTextEdit;
class Mediator;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QString mindMapFile = "");

    ~MainWindow();

    static MainWindow * instance();

    void console(QString text);

    void setTitle(QString openFileName);

public slots:

    void enableUndo(bool enable);

    void showErrorDialog(QString message);

    void showMessageBox(QString message);

protected:

    void closeEvent(QCloseEvent * event) override;

    void resizeEvent(QResizeEvent * event) override;

private slots:

    void doOpenMindMap(QString fileName);

    void initializeNewMindMap();

    void saveMindMap();

    void saveMindMapAs();

    void setupMindMapAfterUndoOrRedo();

    void showAboutDlg();

    void showAboutQtDlg();

    void openArgMindMap();

    void openMindMap();

private:

    void addRedoAction(QMenu & menu, std::function<void ()> handler);

    void addUndoAction(QMenu & menu, std::function<void ()> handler);

    void createEditMenu();

    void createFileMenu();

    void createHelpMenu();

    void disableUndoAndRedo();

    QString getFileDialogFileText() const;

    void init();

    QString loadRecentPath() const;

    void populateMenuBar();

    void saveRecentPath(QString fileName);

    void setSaveActionStatesOnNewMindMap();

    void successLog();

    AboutDlg * m_aboutDlg;

    QAction * m_saveAction = nullptr;

    QAction * m_saveAsAction = nullptr;

    QAction * m_undoAction = nullptr;

    QAction * m_redoAction = nullptr;

    QString m_argMindMapFile;

    const char * m_settingsGroup = "MainWindow";

    Mediator * m_mediator;

    static MainWindow * m_instance;
};

#endif // MAINWINDOW_HPP

