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
        m_activeLanguage = activeLanguage;
        emit activeLanguageChanged(activeLanguage);
    }
}

QString LanguageService::userLanguage() const
{
    return m_userLanguage;
}

void LanguageService::setUserLanguage(QString userLanguage)
{
    L(TAG).info() << "User language: '" << userLanguage.toStdString() << "'";

    m_userLanguage = userLanguage;
}

void LanguageService::installTranslatorForApplicationTranslations(QCoreApplication & application, QStringList languages)
{
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
    for (auto && language : languages) {
        L(TAG).debug() << "Trying Qt translations for '" << language.toStdString() << "'";
#if QT_VERSION >= 0x60000
        if (m_qtTranslator.load("qt_" + lang, QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
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

QStringList LanguageService::userLanguageOrAvailableSystemUiLanguages() const
{
    // See https://doc.qt.io/qt-5/qtranslator.html#load-1
    if (!m_userLanguage.isEmpty()) {
        return { m_userLanguage };
    } else {
        return uiLanguages();
    }
}

void LanguageService::initializeTranslations(QCoreApplication & application)
{
    const auto languageOptions = userLanguageOrAvailableSystemUiLanguages();

    installTranslatorForBuiltInQtTranslations(application, languageOptions);

    installTranslatorForApplicationTranslations(application, languageOptions);
}

QStringList LanguageService::selectableLanguages() const
{
    QStringList selectableLanguages;

    const auto languageOptions = Constants::Application::languages();
    std::transform(languageOptions.begin(), languageOptions.end(),
                   std::back_inserter(selectableLanguages),
                   [](const std::string & language) {
                       return QString::fromStdString(language);
                   });

    return selectableLanguages;
}

QStringList LanguageService::selectableLanguagesWithoutActiveLanguage() const
{
    auto languageOptions = Constants::Application::languages();
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
