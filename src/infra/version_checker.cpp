// This file is part of Heimer.
// Copyright (C) 2022 Jussi Lind <jussi.lind@iki.fi>
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

#include "version_checker.hpp"

#include "../common/constants.hpp"
#include "simple_logger.hpp"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include <QRegularExpression>

using juzzlin::L;

static const auto TAG = "VersionChecker";

VersionChecker::VersionChecker(QObject * parent)
  : QObject { parent }
{
}

Version parseLatestReleasedVersion(QString data)
{
    Version newest;
    static QRegularExpression tagMatch("^.*tag/(\\d.\\d.\\d).*$");
    for (auto && line : data.split("\n")) {
        if (const auto match = tagMatch.match(line); match.hasMatch()) {
            if (const Version version { match.captured(1) }; version.isValid() && version > newest) {
                newest = version;
            }
        }
    }
    return newest;
}

void VersionChecker::checkForNewReleases()
{
    L(TAG).debug() << "Checking for new releases..";
    const auto manager = new QNetworkAccessManager { this };
    connect(manager, &QNetworkAccessManager::finished,
            this, [this](auto reply) {
                if (const auto latestReleasedVersion = parseLatestReleasedVersion(reply->readAll()); latestReleasedVersion.isValid()) {
                    L(TAG).debug() << "The latest released version is " << latestReleasedVersion;
                    if (latestReleasedVersion > Version { Constants::Application::applicationVersion() }) {
                        L(TAG).debug() << "=> NEW version available: " << latestReleasedVersion << " @ " << Constants::Application::releasesUrl().toStdString();
                        emit newVersionFound(latestReleasedVersion, Constants::Application::releasesUrl());
                    } else {
                        L(TAG).debug() << "=> " << Constants::Application::applicationName().toStdString() << " is up-to-date";
                    }
                } else {
                    L(TAG).warning() << "Could not determine the latest released version!";
                }
            });
    manager->get(QNetworkRequest(QUrl { Constants::Application::releasesUrl() }));
}
