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

#include "../common/types.hpp"
#include "state_machine.hpp"

#include <QApplication>
#include <QColor>
#include <QObject>
#include <QTranslator>

#include <memory>

class EditorView;
class ImageManager;
class MainWindow;
class VersionChecker;

namespace Dialogs::Export {
class PngExportDialog;
class SvgExportDialog;
} // namespace Dialogs::Export

class Application : public QObject
{
    Q_OBJECT

public:
    Application(int & argc, char ** argv);

    ~Application() override;

    int run();

public slots:

    void runState(StateMachine::State state);

signals:

    void actionTriggered(StateMachine::Action action);

    void backgroundColorChanged(QColor color);

private:
    std::string availableLanguages() const;

    void doOpenMindMap(QString fileName);

    QString getFileDialogFileText() const;

    void installTranslatorForApplicationTranslations(QStringList languages);

    void installTranslatorForBuiltInQtTranslations(QStringList languages);

    void initializeTranslations();

    void openArgMindMap();

    void openMindMap();

    void saveMindMap();

    void saveMindMapAs();

    void showBackgroundColorDialog();

    void showEdgeColorDialog();

    void showGridColorDialog();

    void showImageFileDialog();

    void showLayoutOptimizationDialog();

    void showNodeColorDialog();

    void showPngExportDialog();

    void showSvgExportDialog();

    void showTextColorDialog();

    void showMessageBox(QString message);

    int showNotSavedDialog();

    void parseArgs(int argc, char ** argv);

    void updateProgress();

    QApplication m_application;

    MainWindowS m_mainWindow;

    std::unique_ptr<SC> m_sc;

    StateMachine * m_stateMachine;

    QTranslator m_appTranslator;

    QTranslator m_qtTranslator;

    QString m_mindMapFile;

    QString m_lang;

    EditorView * m_editorView = nullptr;

    Dialogs::Export::PngExportDialog * m_pngExportDialog = nullptr;

    Dialogs::Export::SvgExportDialog * m_svgExportDialog = nullptr;

    VersionChecker * m_versionChecker;
};

#endif // APPLICATION_HPP
