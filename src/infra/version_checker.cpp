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

VersionChecker::VersionChecker(QObject * parent)
  : QObject(parent)
{
}

Version parseLatestReleasedVersion(QString data)
{
    Version newest;
    for (auto && line : data.split("\n")) {
        QRegularExpression tagMatch("^.*tag/(\\d.\\d.\\d).*$");
        const auto match = tagMatch.match(line);
        if (match.hasMatch()) {
            const Version version(match.captured(1));
            if (version.isValid() && version > newest) {
                newest = version;
            }
        }
    }
    return newest;
}

void VersionChecker::checkForNewReleases()
{
    L().debug() << "Checking for new releases..";
    const auto manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished,
            this, [this](auto reply) {
                const auto latestReleasedVersion = parseLatestReleasedVersion(reply->readAll());
                L().debug() << "The latest released version is " << latestReleasedVersion;
                if (latestReleasedVersion.isValid()) {
                    if (const Version currentVersion(VERSION); latestReleasedVersion > currentVersion) {
                        L().debug() << "=> NEW version available: " << latestReleasedVersion << " @ " << Constants::Application::RELEASES_URL;
                        emit newVersionFound(latestReleasedVersion, Constants::Application::RELEASES_URL);
                    } else {
                        L().debug() << "=> " << Constants::Application::APPLICATION_NAME << " is up-to-date";
                    }
                } else {
                    L().warning() << "Could not determine the latest released version!";
                }
            });
    manager->get(QNetworkRequest({ Constants::Application::RELEASES_URL }));
}
