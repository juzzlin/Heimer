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

#include "state_machine.hpp"

#include <QApplication>
#include <QColor>
#include <QObject>
#include <QTranslator>

#include <memory>

class EditorService;
class EditorView;
class ImageManager;
class MainWindow;
class ApplicationService;

namespace Core {
class VersionChecker;
}

namespace Dialogs {
class PngExportDialog;
class SvgExportDialog;
} // namespace Dialogs

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
    void doOpenMindMap(QString fileName);

    QString getFileDialogFileText() const;

    void initTranslations();

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

    QTranslator m_appTranslator;

    QTranslator m_qtTranslator;

    QString m_mindMapFile;

    QString m_lang;

    std::unique_ptr<StateMachine> m_stateMachine;

    std::unique_ptr<MainWindow> m_mainWindow;

    std::shared_ptr<ApplicationService> m_applicationService;

    std::shared_ptr<EditorService> m_editorService;

    EditorView * m_editorView = nullptr;

    std::unique_ptr<Dialogs::PngExportDialog> m_pngExportDialog;

    std::unique_ptr<Dialogs::SvgExportDialog> m_svgExportDialog;

    std::unique_ptr<Core::VersionChecker> m_versionChecker;
};

#endif // APPLICATION_HPP
