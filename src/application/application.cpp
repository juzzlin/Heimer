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

#include "../application/application_service.hpp"
#include "../application/editor_service.hpp"
#include "../application/progress_manager.hpp"
#include "../application/recent_files_manager.hpp"
#include "../application/service_container.hpp"
#include "../application/settings_proxy.hpp"
#include "../application/state_machine.hpp"
#include "../application/user_exception.hpp"
#include "../common/constants.hpp"
#include "../domain/image_manager.hpp"
#include "../domain/layout_optimizer.hpp"
#include "../infra/settings.hpp"
#include "../infra/version_checker.hpp"
#include "../view/dialogs/export/png_export_dialog.hpp"
#include "../view/dialogs/export/svg_export_dialog.hpp"
#include "../view/dialogs/layout_optimization_dialog.hpp"
#include "../view/dialogs/scene_color_dialog.hpp"
#include "../view/editor_scene.hpp"
#include "../view/editor_view.hpp"
#include "../view/main_window.hpp"
#include "../view/node_action.hpp"

#include "argengine.hpp"
#include "simple_logger.hpp"

#include <QFileDialog>
#include <QLibraryInfo>
#include <QLocale>
#include <QMessageBox>
#include <QObject>
#include <QProgressDialog>
#include <QStandardPaths>

#include <iostream>
#include <thread>

using juzzlin::Argengine;
using juzzlin::L;

Application::Application(int & argc, char ** argv)
  : m_application(argc, argv)
  , m_sc(std::make_unique<SC>())
  , m_stateMachine(new StateMachine(this)) // Parented to this
  , m_versionChecker(new VersionChecker(this)) // Parented to this
{
    parseArgs(argc, argv);

    initTranslations();

    // Instantiate components here because the possible language given
    // in the command line must have been loaded before this
    m_mainWindow = std::make_unique<MainWindow>();
    m_sc->setMainWindow(m_mainWindow);
    m_editorView = new EditorView;

    // Use raw pointers because the dialogs are parented to m_mainWindow which takes
    // the ownership and handles deletion.
    m_pngExportDialog = new Dialogs::Export::PngExportDialog(*m_mainWindow);
    m_svgExportDialog = new Dialogs::Export::SvgExportDialog(*m_mainWindow);

    // Note!!: EditorView will be parented to MainWindow.
    m_sc->applicationService()->setEditorView(*m_editorView);

    // Connect views and StateMachine together
    connect(this, &Application::actionTriggered, m_stateMachine, &StateMachine::calculateState);
    connect(m_editorView, &EditorView::actionTriggered, m_stateMachine, [this](StateMachine::Action action) {
        m_stateMachine->calculateState(action);
    });
    connect(m_mainWindow.get(), &MainWindow::actionTriggered, m_stateMachine, &StateMachine::calculateState);
    connect(m_stateMachine, &StateMachine::stateChanged, this, &Application::runState);

    connect(m_pngExportDialog, &Dialogs::Export::PngExportDialog::pngExportRequested, m_sc->applicationService().get(), &ApplicationService::exportToPng);
    connect(m_svgExportDialog, &Dialogs::Export::SvgExportDialog::svgExportRequested, m_sc->applicationService().get(), &ApplicationService::exportToSvg);

    connect(m_sc->applicationService().get(), &ApplicationService::pngExportFinished, m_pngExportDialog, &Dialogs::Export::PngExportDialog::finishExport);
    connect(m_sc->applicationService().get(), &ApplicationService::svgExportFinished, m_svgExportDialog, &Dialogs::Export::SvgExportDialog::finishExport);

    connect(m_mainWindow.get(), &MainWindow::gridVisibleChanged, m_editorView, [this](int state) {
        bool visible = state == Qt::Checked;
        m_editorView->setGridVisible(visible);
    });

    m_mainWindow->initialize();
    m_mainWindow->appear();

    // Open mind map according to CLI argument (if exists) or autoload the recent mind map (if enabled)
    if (!m_mindMapFile.isEmpty()) {
        QTimer::singleShot(0, this, &Application::openArgMindMap);
    } else if (SC::instance().settingsProxy()->autoload()) {
        if (const auto recentFile = SC::instance().recentFilesManager()->recentFile(); recentFile.has_value()) {
            // Exploit same code as used to open arg mind map
            m_mindMapFile = recentFile.value();
            QTimer::singleShot(0, this, &Application::openArgMindMap);
        }
    }

    connect(m_versionChecker, &VersionChecker::newVersionFound, this, [this](Version version, QString downloadUrl) {
        m_sc->applicationService()->showStatusText(QString(tr("A new version %1 available at <a href='%2'>%2</a>")).arg(version.toString(), downloadUrl));
    });
    m_versionChecker->checkForNewReleases();
}

QString Application::getFileDialogFileText() const
{
    return tr("Heimer Files") + " (*" + Constants::Application::fileExtension() + ")";
}

// Forces the language to the given one or chooses the best UI language
void Application::initTranslations()
{
    // See https://doc.qt.io/qt-5/qtranslator.html#load-1
    QStringList langs;
    if (m_lang.isEmpty()) {
        langs = QLocale().uiLanguages();
    } else {
        langs << m_lang;
        L().info() << "Language forced to '" << m_lang.toStdString() << "'";
    }

    // Qt's built-in translations
    for (auto && lang : langs) {
        L().debug() << "Trying Qt translations for '" << lang.toStdString() << "'";
#if QT_VERSION >= 0x60000
        if (m_qtTranslator.load("qt_" + lang, QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
#else
        if (m_qtTranslator.load("qt_" + lang, QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
#endif
            m_application.installTranslator(&m_qtTranslator);
            L().debug() << "Loaded Qt translations for '" << lang.toStdString() << "'";
            break;
        } else {
            L().warning() << "Failed to load Qt translations for '" << lang.toStdString() << "'";
        }
    }

    // Application's translations
    for (auto && lang : langs) {
        L().debug() << "Trying application translations for '" << lang.toStdString() << "'";
        if (m_appTranslator.load(Constants::Application::translationsResourceBase() + lang)) {
            m_application.installTranslator(&m_appTranslator);
            L().debug() << "Loaded application translations for '" << lang.toStdString() << "'";
            break;
        } else {
            L().warning() << "Failed to load application translations for '" << lang.toStdString() << "'";
        }
    }
}

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

int Application::run()
{
    return m_application.exec();
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
        m_sc->applicationService()->initializeNewMindMap();
        break;
    case StateMachine::State::OpenRecent:
        doOpenMindMap(SC::instance().recentFilesManager()->selectedFile());
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

void Application::updateProgress()
{
    SC::instance().progressManager()->updateProgress();
}

void Application::openArgMindMap()
{
    doOpenMindMap(m_mindMapFile);
}

void Application::openMindMap()
{
    L().debug() << "Open file";

    const auto path = Settings::Custom::loadRecentPath();
    if (const auto fileName = QFileDialog::getOpenFileName(m_mainWindow.get(), tr("Open File"), path, getFileDialogFileText()); !fileName.isEmpty()) {
        doOpenMindMap(fileName);
    } else {
        emit actionTriggered(StateMachine::Action::OpeningMindMapCanceled);
    }
}

void Application::doOpenMindMap(QString fileName)
{
    L().debug() << "Opening '" << fileName.toStdString();
    m_mainWindow->showSpinnerDialog(true, tr("Opening '%1'..").arg(fileName));
    updateProgress();
    if (m_sc->applicationService()->openMindMap(fileName)) {
        m_mainWindow->disableUndoAndRedo();
        updateProgress();
        m_mainWindow->setSaveActionStatesOnOpenedMindMap();
        updateProgress();
        Settings::Custom::saveRecentPath(fileName);
        updateProgress();
        m_mainWindow->showSpinnerDialog(false);
        updateProgress();
        emit actionTriggered(StateMachine::Action::MindMapOpened);
    } else {
        m_mainWindow->showSpinnerDialog(false);
        updateProgress();
        emit actionTriggered(StateMachine::Action::OpeningMindMapFailed);
    }
}

void Application::saveMindMap()
{
    L().debug() << "Save..";

    if (!m_sc->applicationService()->saveMindMap()) {
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
      Settings::Custom::loadRecentPath(),
      getFileDialogFileText());

    if (fileName.isEmpty()) {
        emit actionTriggered(StateMachine::Action::MindMapSaveAsCanceled);
        return;
    }

    if (!fileName.endsWith(Constants::Application::fileExtension())) {
        fileName += Constants::Application::fileExtension();
    }

    if (m_sc->applicationService()->saveMindMapAs(fileName)) {
        const auto msg = QString(tr("File '")) + fileName + tr("' saved.");
        L().debug() << msg.toStdString();
        m_mainWindow->enableSave(false);
        Settings::Custom::saveRecentPath(fileName);
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
    Dialogs::SceneColorDialog(Dialogs::ColorDialog::Role::Background).exec();
    emit actionTriggered(StateMachine::Action::BackgroundColorChanged);
}

void Application::showEdgeColorDialog()
{
    if (Dialogs::SceneColorDialog(Dialogs::ColorDialog::Role::Edge).exec() != QDialog::Accepted) {
        // Clear implicitly selected edges on cancel
        m_sc->applicationService()->clearEdgeSelectionGroup(true);
    }
    emit actionTriggered(StateMachine::Action::EdgeColorChanged);
}

void Application::showGridColorDialog()
{
    Dialogs::SceneColorDialog(Dialogs::ColorDialog::Role::Grid).exec();
    emit actionTriggered(StateMachine::Action::GridColorChanged);
}

void Application::showNodeColorDialog()
{
    if (Dialogs::SceneColorDialog(Dialogs::ColorDialog::Role::Node).exec() != QDialog::Accepted) {
        // Clear implicitly selected nodes on cancel
        m_sc->applicationService()->clearNodeSelectionGroup(true);
    }
    emit actionTriggered(StateMachine::Action::NodeColorChanged);
}

void Application::showTextColorDialog()
{
    if (Dialogs::SceneColorDialog(Dialogs::ColorDialog::Role::Text).exec() != QDialog::Accepted) {
        // Clear implicitly selected nodes on cancel
        m_sc->applicationService()->clearNodeSelectionGroup(true);
    }
    emit actionTriggered(StateMachine::Action::TextColorChanged);
}

void Application::showImageFileDialog()
{
    const auto path = Settings::Custom::loadRecentImagePath();
    const auto extensions = "(*.jpg *.jpeg *.JPG *.JPEG *.png *.PNG)";
    const auto fileName = QFileDialog::getOpenFileName(
      m_mainWindow.get(), tr("Open an image"), path, tr("Image Files") + " " + extensions);

    if (QImage image; image.load(fileName)) {
        m_sc->applicationService()->performNodeAction({ NodeAction::Type::AttachImage, image, fileName });
        Settings::Custom::saveRecentImagePath(fileName);
    } else if (fileName != "") {
        QMessageBox::critical(m_mainWindow.get(), tr("Load image"), tr("Failed to load image '") + fileName + "'");
    }
}

void Application::showPngExportDialog()
{
    m_pngExportDialog->setCurrentMindMapFileName(m_sc->applicationService()->fileName());
    m_pngExportDialog->setImageSize(m_sc->applicationService()->zoomForExport(true));
    m_pngExportDialog->exec();

    // Doesn't matter if canceled or not
    emit actionTriggered(StateMachine::Action::PngExported);
}

void Application::showSvgExportDialog()
{
    m_svgExportDialog->setCurrentMindMapFileName(m_sc->applicationService()->fileName());
    m_svgExportDialog->exec();

    // Doesn't matter if canceled or not
    emit actionTriggered(StateMachine::Action::SvgExported);
}

void Application::showLayoutOptimizationDialog()
{
    LayoutOptimizer layoutOptimizer { m_sc->applicationService()->mindMapData(), m_editorView->grid() };
    Dialogs::LayoutOptimizationDialog dialog { *m_mainWindow, *m_sc->applicationService()->mindMapData(), layoutOptimizer, *m_editorView };
    connect(&dialog, &Dialogs::LayoutOptimizationDialog::undoPointRequested, m_sc->applicationService().get(), &ApplicationService::saveUndoPoint);

    if (dialog.exec() == QDialog::Accepted) {
        m_sc->applicationService()->zoomToFit();
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
