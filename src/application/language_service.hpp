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

#ifndef LANGUAGE_SERVICE_HPP
#define LANGUAGE_SERVICE_HPP

#include <QObject>
#include <QStringList>
#include <QTranslator>

class QCoreApplication;

class LanguageService : public QObject
{
    Q_OBJECT

public:
    QString activeLanguage() const;

    void setActiveLanguage(QString activeLanguage);

    QString userLanguage() const;

    void setUserLanguage(QString cliLanguage);

    void initializeTranslations(QCoreApplication & application);

    QStringList selectableLanguages() const;

    QStringList selectableLanguagesWithoutActiveLanguage() const;

    QStringList uiLanguages() const;

signals:
    void activeLanguageChanged(QString language);

private:
    QStringList userLanguageOrAvailableSystemUiLanguages() const;

    void installTranslatorForApplicationTranslations(QCoreApplication & application, QStringList languages);

    void installTranslatorForBuiltInQtTranslations(QCoreApplication & application, QStringList languages);

    QString m_activeLanguage;

    QString m_userLanguage;

    QTranslator m_appTranslator;

    QTranslator m_qtTranslator;
};

#endif // LANGUAGE_SERVICE_HPP
