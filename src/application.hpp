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

#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "statemachine.hpp"

#include <QApplication>
#include <QColor>
#include <QObject>
#include <QTranslator>

#include <memory>

class EditorData;
class EditorScene;
class EditorView;
class ExportToPNGDialog;
class MainWindow;
class Mediator;

class Application : public QObject
{
    Q_OBJECT

public:

    Application(int & argc, char ** argv);

    ~Application();

    int run();

public slots:

    void runState(StateMachine::State state);

signals:

    void actionTriggered(StateMachine::Action action);

    void backgroundColorChanged(QColor color);

private:

    void doOpenMindMap(QString fileName);

    QString getFileDialogFileText() const;

    QString loadRecentPath() const;

    void openArgMindMap();

    void openMindMap();

    void saveMindMap();

    void saveMindMapAs();

    void saveRecentPath(QString fileName);

    void showBackgroundColorDialog();

    void showExportToPNGDialog();

    void showMessageBox(QString message);

    int showNotSavedDialog();

    void parseArgs(int argc, char ** argv);

    QApplication m_app;

    QTranslator m_appTranslator;

    QString m_mindMapFile;

    QString m_lang;

    QString m_settingsGroup = "Application";

    std::unique_ptr<StateMachine> m_stateMachine;

    std::unique_ptr<MainWindow> m_mainWindow;

    std::shared_ptr<Mediator> m_mediator;

    std::shared_ptr<EditorData> m_editorData;

    std::shared_ptr<EditorScene> m_editorScene;

    EditorView * m_editorView = nullptr;

    std::unique_ptr<ExportToPNGDialog> m_exportToPNGDialog;
};

#endif // APPLICATION_HPP
