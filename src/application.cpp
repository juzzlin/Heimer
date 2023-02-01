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

#include "application.hpp"
#include "constants.hpp"
#include "editor_data.hpp"
#include "editor_scene.hpp"
#include "editor_view.hpp"
#include "image_manager.hpp"
#include "layout_optimizer.hpp"
#include "main_window.hpp"
#include "mediator.hpp"
#include "node_action.hpp"
#include "recent_files_manager.hpp"
#include "settings.hpp"
#include "settings_proxy.hpp"
#include "single_instance_container.hpp"
#include "state_machine.hpp"
#include "user_exception.hpp"
#include "version_checker.hpp"

#include "dialogs/layout_optimization_dialog.hpp"
#include "dialogs/png_export_dialog.hpp"
#include "dialogs/scene_color_dialog.hpp"
#include "dialogs/svg_export_dialog.hpp"

#include "argengine.hpp"
#include "simple_logger.hpp"

#include <QFileDialog>
#include <QLibraryInfo>
#include <QLocale>
#include <QMessageBox>
#include <QObject>
#include <QStandardPaths>

#include <iostream>

namespace {

using juzzlin::Argengine;
using juzzlin::L;

// Forces the language to the given one or chooses the best UI language
static void initTranslations(QTranslator & appTranslator, QTranslator & qtTranslator, QGuiApplication & app, QString lang = "")
{
    // See https://doc.qt.io/qt-5/qtranslator.html#load-1
    QStringList langs;
    if (lang.isEmpty()) {
        langs = QLocale().uiLanguages();
    } else {
        langs << lang;
        L().info() << "Language forced to '" << lang.toStdString() << "'";
    }

    // Qt's built-in translations
    for (auto && lang : langs) {
        L().debug() << "Trying Qt translations for '" << lang.toStdString() << "'";
#if QT_VERSION >= 0x60000
        if (qtTranslator.load("qt_" + lang, QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
#else
        if (qtTranslator.load("qt_" + lang, QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
#endif
            app.installTranslator(&qtTranslator);
            L().debug() << "Loaded Qt translations for '" << lang.toStdString() << "'";
            break;
        } else {
            L().warning() << "Failed to load Qt translations for '" << lang.toStdString() << "'";
        }
    }

    // Application's translations
    for (auto && lang : langs) {
        L().debug() << "Trying application translations for '" << lang.toStdString() << "'";
        if (appTranslator.load(Constants::Application::TRANSLATIONS_RESOURCE_BASE + lang)) {
            app.installTranslator(&appTranslator);
            L().debug() << "Loaded application translations for '" << lang.toStdString() << "'";
            break;
        } else {
            L().warning() << "Failed to load application translations for '" << lang.toStdString() << "'";
        }
    }
}

} // namespace

void Application::parseArgs(int argc, char ** argv)
{
    Argengine ae(argc, argv);

    const std::set<std::string> languages = { "de", "en", "es", "fi", "fr", "it", "nl", "zh" };
    std::string languageHelp;
    for (auto && lang : languages) {
        languageHelp += lang + ", ";
    }
    languageHelp.pop_back();
    languageHelp.pop_back();
    languageHelp += ".";

    ae.addOption(
      { "-d", "--debug" }, [] {
          L::setLoggingLevel(L::Level::Debug);
      },
      false, "Show debug logging.");

    ae.addOption(
      { "-t", "--trace" }, [] {
          L::setLoggingLevel(L::Level::Trace);
      },
      false, "Show trace logging.");

    ae.addOption(
      { "--lang" }, [this, languages](std::string value) {
          if (!languages.count(value)) {
              L().error() << "Unsupported language: " << value;
          } else {
              m_lang = value.c_str();
          }
      },
      false, "Force language: " + languageHelp);

    ae.setPositionalArgumentCallback([=](Argengine::ArgumentVector args) {
        m_mindMapFile = args.at(0).c_str();
    });

    ae.setHelpText(std::string("\nUsage: ") + argv[0] + " [OPTIONS] [MIND_MAP_FILE]");

    ae.parse();
}

Application::Application(int & argc, char ** argv)
  : m_app(argc, argv)
  , m_stateMachine(std::make_unique<StateMachine>())
  , m_versionChecker(std::make_unique<VersionChecker>())
{
    parseArgs(argc, argv);

    initTranslations(m_appTranslator, m_qtTranslator, m_app, m_lang);

    // Instantiate components here because the possible language given
    // in the command line must have been loaded before this
    m_mainWindow = std::make_unique<MainWindow>();
    m_mediator = std::make_unique<Mediator>(*m_mainWindow);
    m_editorData = std::make_unique<EditorData>();
    m_editorView = new EditorView(*m_mediator);
    m_pngExportDialog = std::make_unique<Dialogs::PngExportDialog>(*m_mainWindow);
    m_svgExportDialog = std::make_unique<Dialogs::SvgExportDialog>(*m_mainWindow);

    m_mainWindow->setMediator(m_mediator);
    m_stateMachine->setMediator(m_mediator);

    m_mediator->setEditorData(m_editorData);
    m_mediator->setEditorView(*m_editorView);

    // Connect views and StateMachine together
    connect(this, &Application::actionTriggered, m_stateMachine.get(), &StateMachine::calculateState);
    connect(m_editorView, &EditorView::actionTriggered, m_stateMachine.get(), [this](StateMachine::Action action) {
        m_stateMachine->calculateState(action);
    });
    connect(m_mainWindow.get(), &MainWindow::actionTriggered, m_stateMachine.get(), &StateMachine::calculateState);
    connect(m_stateMachine.get(), &StateMachine::stateChanged, this, &Application::runState);

    connect(m_editorData.get(), &EditorData::isModifiedChanged, m_mainWindow.get(), [=](bool isModified) {
        m_mainWindow->enableSave(isModified || m_mediator->canBeSaved());
    });

    connect(m_pngExportDialog.get(), &Dialogs::PngExportDialog::pngExportRequested, m_mediator.get(), &Mediator::exportToPng);
    connect(m_svgExportDialog.get(), &Dialogs::SvgExportDialog::svgExportRequested, m_mediator.get(), &Mediator::exportToSvg);

    connect(m_mediator.get(), &Mediator::pngExportFinished, m_pngExportDialog.get(), &Dialogs::PngExportDialog::finishExport);
    connect(m_mediator.get(), &Mediator::svgExportFinished, m_svgExportDialog.get(), &Dialogs::SvgExportDialog::finishExport);

    connect(m_mainWindow.get(), &MainWindow::arrowSizeChanged, m_mediator.get(), &Mediator::setArrowSize);
    connect(m_mainWindow.get(), &MainWindow::autosaveEnabled, m_mediator.get(), &Mediator::enableAutosave);
    connect(m_mainWindow.get(), &MainWindow::cornerRadiusChanged, m_mediator.get(), &Mediator::setCornerRadius);
    connect(m_mainWindow.get(), &MainWindow::edgeWidthChanged, m_mediator.get(), &Mediator::setEdgeWidth);
    connect(m_mainWindow.get(), &MainWindow::fontChanged, m_mediator.get(), &Mediator::changeFont);
    connect(m_mainWindow.get(), &MainWindow::textSizeChanged, m_mediator.get(), &Mediator::setTextSize);
    connect(m_mainWindow.get(), &MainWindow::gridSizeChanged, m_mediator.get(), &Mediator::setGridSize);
    connect(m_mainWindow.get(), &MainWindow::gridVisibleChanged, m_editorView, [this](int state) {
        bool visible = state == Qt::Checked;
        m_editorView->setGridVisible(visible);
    });
    connect(m_mainWindow.get(), &MainWindow::searchTextChanged, m_mediator.get(), &Mediator::setSearchText);
    connect(m_mainWindow.get(), &MainWindow::shadowEffectChanged, m_mediator.get(), &Mediator::setShadowEffect);

    m_mainWindow->initialize();
    m_mainWindow->appear();

    // Open mind map according to CLI argument (if exists) or autoload the recent mind map (if enabled)
    if (!m_mindMapFile.isEmpty()) {
        QTimer::singleShot(0, this, &Application::openArgMindMap);
    } else if (SingleInstanceContainer::instance().settingsProxy().autoload()) {
        if (const auto recentFile = RecentFilesManager::instance().recentFile(); recentFile.has_value()) {
            // Exploit same code as used to open arg mind map
            m_mindMapFile = recentFile.value();
            QTimer::singleShot(0, this, &Application::openArgMindMap);
        }
    }

    connect(m_versionChecker.get(), &VersionChecker::newVersionFound, this, [this](Version version, QString downloadUrl) {
        m_mainWindow->showStatusText(QString(tr("A new version %1 available at <a href='%2'>%2</a>")).arg(version.toString(), downloadUrl));
    });
    m_versionChecker->checkForNewReleases();
}

QString Application::getFileDialogFileText() const
{
    return tr("Heimer Files") + " (*" + Constants::Application::FILE_EXTENSION + ")";
}

int Application::run()
{
    return m_app.exec();
}

void Application::runState(StateMachine::State state)
{
    switch (state) {
    case StateMachine::State::TryCloseWindow:
        m_mainWindow->saveWindowSize();
        m_mainWindow->close();
        break;
    case StateMachine::State::Exit:
        m_mainWindow->saveWindowSize();
        QApplication::exit(EXIT_SUCCESS);
        break;
    default:
    case StateMachine::State::Edit:
        m_mainWindow->setTitle();
        break;
    case StateMachine::State::InitializeNewMindMap:
        m_mediator->initializeNewMindMap();
        break;
    case StateMachine::State::OpenRecent:
        doOpenMindMap(RecentFilesManager::instance().selectedFile());
        break;
    case StateMachine::State::OpenDrop:
        doOpenMindMap(m_editorView->dropFile());
        break;
    case StateMachine::State::Save:
        saveMindMap();
        break;
    case StateMachine::State::ShowBackgroundColorDialog:
        showBackgroundColorDialog();
        break;
    case StateMachine::State::ShowEdgeColorDialog:
        showEdgeColorDialog();
        break;
    case StateMachine::State::ShowGridColorDialog:
        showGridColorDialog();
        break;
    case StateMachine::State::ShowNodeColorDialog:
        showNodeColorDialog();
        break;
    case StateMachine::State::ShowTextColorDialog:
        showTextColorDialog();
        break;
    case StateMachine::State::ShowImageFileDialog:
        showImageFileDialog();
        break;
    case StateMachine::State::ShowPngExportDialog:
        showPngExportDialog();
        break;
    case StateMachine::State::ShowLayoutOptimizationDialog:
        showLayoutOptimizationDialog();
        break;
    case StateMachine::State::ShowNotSavedDialog:
        switch (showNotSavedDialog()) {
        case QMessageBox::Save:
            emit actionTriggered(StateMachine::Action::NotSavedDialogAccepted);
            break;
        case QMessageBox::Discard:
            emit actionTriggered(StateMachine::Action::NotSavedDialogDiscarded);
            break;
        case QMessageBox::Cancel:
            emit actionTriggered(StateMachine::Action::NotSavedDialogCanceled);
            break;
        }
        break;
    case StateMachine::State::ShowSaveAsDialog:
        saveMindMapAs();
        break;
    case StateMachine::State::ShowSvgExportDialog:
        showSvgExportDialog();
        break;
    case StateMachine::State::ShowOpenDialog:
        openMindMap();
        break;
    }
}

void Application::openArgMindMap()
{
    doOpenMindMap(m_mindMapFile);
}

void Application::openMindMap()
{
    L().debug() << "Open file";

    const auto path = Settings::V1::loadRecentPath();
    if (const auto fileName = QFileDialog::getOpenFileName(m_mainWindow.get(), tr("Open File"), path, getFileDialogFileText()); !fileName.isEmpty()) {
        doOpenMindMap(fileName);
    } else {
        emit actionTriggered(StateMachine::Action::OpeningMindMapCanceled);
    }
}

void Application::doOpenMindMap(QString fileName)
{
    L().debug() << "Opening '" << fileName.toStdString();

    if (m_mediator->openMindMap(fileName)) {
        m_mainWindow->disableUndoAndRedo();
        m_mainWindow->setSaveActionStatesOnOpenedMindMap();
        Settings::V1::saveRecentPath(fileName);
        emit actionTriggered(StateMachine::Action::MindMapOpened);
    } else {
        emit actionTriggered(StateMachine::Action::OpeningMindMapFailed);
    }
}

void Application::saveMindMap()
{
    L().debug() << "Save..";

    if (!m_mediator->saveMindMap()) {
        const auto msg = QString(tr("Failed to save file."));
        L().error() << msg.toStdString();
        showMessageBox(msg);
        emit actionTriggered(StateMachine::Action::MindMapSaveFailed);
        return;
    }

    m_mainWindow->enableSave(false);
    emit actionTriggered(StateMachine::Action::MindMapSaved);
}

void Application::saveMindMapAs()
{
    L().debug() << "Save as..";

    QString fileName = QFileDialog::getSaveFileName(
      m_mainWindow.get(),
      tr("Save File As"),
      Settings::V1::loadRecentPath(),
      getFileDialogFileText());

    if (fileName.isEmpty()) {
        emit actionTriggered(StateMachine::Action::MindMapSaveAsCanceled);
        return;
    }

    if (!fileName.endsWith(Constants::Application::FILE_EXTENSION)) {
        fileName += Constants::Application::FILE_EXTENSION;
    }

    if (m_mediator->saveMindMapAs(fileName)) {
        const auto msg = QString(tr("File '")) + fileName + tr("' saved.");
        L().debug() << msg.toStdString();
        m_mainWindow->enableSave(false);
        Settings::V1::saveRecentPath(fileName);
        emit actionTriggered(StateMachine::Action::MindMapSavedAs);
    } else {
        const auto msg = QString(tr("Failed to save file as '") + fileName + "'.");
        L().error() << msg.toStdString();
        showMessageBox(msg);
        emit actionTriggered(StateMachine::Action::MindMapSaveAsFailed);
    }
}

void Application::showBackgroundColorDialog()
{
    Dialogs::SceneColorDialog(Dialogs::ColorDialog::Role::Background, m_mediator).exec();
    emit actionTriggered(StateMachine::Action::BackgroundColorChanged);
}

void Application::showEdgeColorDialog()
{
    Dialogs::SceneColorDialog(Dialogs::ColorDialog::Role::Edge, m_mediator).exec();
    emit actionTriggered(StateMachine::Action::EdgeColorChanged);
}

void Application::showGridColorDialog()
{
    Dialogs::SceneColorDialog(Dialogs::ColorDialog::Role::Grid, m_mediator).exec();
    emit actionTriggered(StateMachine::Action::GridColorChanged);
}

void Application::showNodeColorDialog()
{
    if (Dialogs::SceneColorDialog(Dialogs::ColorDialog::Role::Node, m_mediator).exec() != QDialog::Accepted) {
        // Clear implicitly selected nodes on cancel
        m_mediator->clearSelectionGroup(true);
    }
    emit actionTriggered(StateMachine::Action::NodeColorChanged);
}

void Application::showTextColorDialog()
{
    if (Dialogs::SceneColorDialog(Dialogs::ColorDialog::Role::Text, m_mediator).exec() != QDialog::Accepted) {
        // Clear implicitly selected nodes on cancel
        m_mediator->clearSelectionGroup(true);
    }
    emit actionTriggered(StateMachine::Action::TextColorChanged);
}

void Application::showImageFileDialog()
{
    const auto path = Settings::V1::loadRecentImagePath();
    const auto extensions = "(*.jpg *.jpeg *.JPG *.JPEG *.png *.PNG)";
    const auto fileName = QFileDialog::getOpenFileName(
      m_mainWindow.get(), tr("Open an image"), path, tr("Image Files") + " " + extensions);

    if (QImage image; image.load(fileName)) {
        m_mediator->performNodeAction({ NodeAction::Type::AttachImage, image, fileName });
        Settings::V1::saveRecentImagePath(fileName);
    } else if (fileName != "") {
        QMessageBox::critical(m_mainWindow.get(), tr("Load image"), tr("Failed to load image '") + fileName + "'");
    }
}

void Application::showPngExportDialog()
{
    m_pngExportDialog->setImageSize(m_mediator->zoomForExport(true));
    m_pngExportDialog->exec();

    // Doesn't matter if canceled or not
    emit actionTriggered(StateMachine::Action::PngExported);
}

void Application::showSvgExportDialog()
{
    m_svgExportDialog->exec();

    // Doesn't matter if canceled or not
    emit actionTriggered(StateMachine::Action::SvgExported);
}

void Application::showLayoutOptimizationDialog()
{
    LayoutOptimizer layoutOptimizer { m_mediator->mindMapData(), m_editorView->grid() };
    Dialogs::LayoutOptimizationDialog dialog { *m_mainWindow, *m_mediator->mindMapData(), layoutOptimizer };
    connect(&dialog, &Dialogs::LayoutOptimizationDialog::undoPointRequested, m_mediator.get(), &Mediator::saveUndoPoint);

    if (dialog.exec() == QDialog::Accepted) {
        m_mediator->zoomToFit();
    }

    emit actionTriggered(StateMachine::Action::LayoutOptimized);
}

void Application::showMessageBox(QString message)
{
    QMessageBox msgBox(m_mainWindow.get());
    msgBox.setText(message);
    msgBox.exec();
}

int Application::showNotSavedDialog()
{
    QMessageBox msgBox(m_mainWindow.get());
    msgBox.setText(tr("The mind map has been modified."));
    msgBox.setInformativeText(tr("Do you want to save your changes?"));
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    return msgBox.exec();
}

Application::~Application() = default;
