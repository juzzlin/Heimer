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

protected:

    void closeEvent(QCloseEvent * event) override;

private slots:

    bool doOpenMindMap(QString fileName);

    void initializeNewMindMap();

    void saveMindMap();

    void saveAsMindMap();

    void setupMindMapAfterUndoOrRedo();

    void showAboutDlg();

    void showAboutQtDlg();

    void openArgMindMap();

    void openMindMap();

private:

    void init();

    void populateMenuBar();

    void setActionStatesOnNewMindMap();

    AboutDlg * m_aboutDlg;

    QAction * m_saveAction = nullptr;

    QAction * m_saveAsAction = nullptr;

    QAction * m_undoAction = nullptr;

    QAction * m_redoAction = nullptr;

    QString m_argMindMapFile;

    bool m_saved = false;

    const char * m_settingsGroup = "MainWindow";

    Mediator * m_mediator;

    static MainWindow * m_instance;
};

#endif // MAINWINDOW_HPP

