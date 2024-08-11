// This file is part of Heimer.
// Copyright (C) 2024 Jussi Lind <jussi.lind@iki.fi>
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

#include "language_service.hpp"

#include "argengine.hpp"
#include "common/constants.hpp"
#include "service_container.hpp"
#include "settings_proxy.hpp"
#include "simple_logger.hpp"

#include <QCoreApplication>
#include <QLibraryInfo>
#include <QLocale>

using juzzlin::Argengine;
using juzzlin::L;

static const auto TAG = "LanguageService";

QString LanguageService::activeLanguage() const
{
    return m_activeLanguage;
}

void LanguageService::setActiveLanguage(QString activeLanguage)
{
    if (m_activeLanguage != activeLanguage) {
        L(TAG).info() << "Changing active language via UI: '" << activeLanguage.toStdString() << "'";
        m_activeLanguage = activeLanguage;
        installTranslatorForBuiltInQtTranslations(*QCoreApplication::instance(), { m_activeLanguage });
        installTranslatorForApplicationTranslations(*QCoreApplication::instance(), { m_activeLanguage });
        SC::instance().settingsProxy()->setUserLanguage(m_activeLanguage);
        emit activeLanguageChanged(m_activeLanguage);
    }
}

QString LanguageService::commandLineLanguage() const
{
    return m_commandLineLanguage;
}

void LanguageService::setCommandLineLanguage(QString commandLineLanguage)
{
    m_commandLineLanguage = commandLineLanguage;
}

QString LanguageService::savedUserLanguage() const
{
    return SC::instance().settingsProxy()->userLanguage();
}

void LanguageService::installTranslatorForApplicationTranslations(QCoreApplication & application, QStringList languages)
{
    // See https://doc.qt.io/qt-5/qtranslator.html#load-1

    for (auto && language : languages) {
        L(TAG).debug() << "Trying application translations for '" << language.toStdString() << "'";
        if (m_appTranslator.load(Constants::Application::translationsResourceBase() + language)) {
            application.installTranslator(&m_appTranslator);
            setActiveLanguage(language);
            L(TAG).info() << "Loaded application translations for '" << language.toStdString() << "'";
            break;
        } else {
            L(TAG).warning() << "Failed to load application translations for '" << language.toStdString() << "'";
        }
    }
}

void LanguageService::installTranslatorForBuiltInQtTranslations(QCoreApplication & application, QStringList languages)
{
    // See https://doc.qt.io/qt-5/qtranslator.html#load-1

    for (auto && language : languages) {
        L(TAG).debug() << "Trying Qt translations for '" << language.toStdString() << "'";
#if QT_VERSION >= 0x60000
        if (m_qtTranslator.load("qt_" + language, QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
#else
        if (m_qtTranslator.load("qt_" + language, QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
#endif
            application.installTranslator(&m_qtTranslator);
            L(TAG).info() << "Loaded Qt translations for '" << language.toStdString() << "'";
            break;
        } else {
            L(TAG).warning() << "Failed to load Qt translations for '" << language.toStdString() << "'";
        }
    }
}

QStringList LanguageService::commandLineLanguageOrSavedLanguageOrAvailableSystemUiLanguages() const
{
    if (!commandLineLanguage().isEmpty()) {
        L(TAG).info() << "Selecting language given via CLI: '" << commandLineLanguage().toStdString() << "'";
        return { commandLineLanguage() };
    } else if (!savedUserLanguage().isEmpty()) {
        L(TAG).info() << "Selecting previous language set via UI: '" << savedUserLanguage().toStdString() << "'";
        return { savedUserLanguage() };
    } else {
        return uiLanguages();
    }
}

void LanguageService::initializeTranslations(QCoreApplication & application)
{
    const auto languageOptions = commandLineLanguageOrSavedLanguageOrAvailableSystemUiLanguages();

    installTranslatorForBuiltInQtTranslations(application, languageOptions);

    installTranslatorForApplicationTranslations(application, languageOptions);
}

QStringList LanguageService::selectableLanguages() const
{
    QStringList selectableLanguages;

    auto languageOptions = Constants::Application::supportedLanguages();

    // Add UI languages so that the user can switch back to the original language,
    // even though it might not be supported by Heimer, it might be supported by Qt
    for (auto && uiLanguage : uiLanguages()) {
        languageOptions.insert(uiLanguage.toStdString());
    }

    std::transform(languageOptions.begin(), languageOptions.end(),
                   std::back_inserter(selectableLanguages),
                   [](const std::string & language) {
                       return QString::fromStdString(language);
                   });

    return selectableLanguages;
}

QStringList LanguageService::selectableLanguagesWithoutActiveLanguage() const
{
    auto languageOptions = Constants::Application::supportedLanguages();
    languageOptions.erase(m_activeLanguage.toStdString());
    QStringList selectableLanguages;

    std::transform(languageOptions.begin(), languageOptions.end(),
                   std::back_inserter(selectableLanguages),
                   [](const std::string & language) {
                       return QString::fromStdString(language);
                   });

    return selectableLanguages;
}

QStringList LanguageService::uiLanguages() const
{
    return QLocale {}.uiLanguages();
}
