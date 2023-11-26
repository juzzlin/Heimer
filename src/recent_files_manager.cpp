// This file is part of Heimer.
// Copyright (C) 2020 Jussi Lind <jussi.lind@iki.fi>
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

#include "recent_files_manager.hpp"

#include "constants.hpp"
#include "contrib/SimpleLogger/src/simple_logger.hpp"
#include "core/settings.hpp"

#include <QFileInfo>
#include <QSettings>

RecentFilesManager::RecentFilesManager()
{
    m_recentFiles = Settings::V1::loadRecentFiles();
}

void RecentFilesManager::addRecentFile(QString filePath)
{
    const QFileInfo fi(filePath);

    filePath = fi.absoluteFilePath();

    m_recentFiles.removeAll(filePath);
    m_recentFiles.push_front(filePath);

    while (m_recentFiles.size() > Constants::RecentFiles::MAX_FILES) {
        m_recentFiles.pop_back();
    }

    juzzlin::L().debug() << "Added recent file: " << filePath.toStdString();
    juzzlin::L().debug() << "Recent file count: " << m_recentFiles.size();

    Settings::V1::saveRecentFiles(m_recentFiles);
}

std::optional<QString> RecentFilesManager::recentFile() const
{
    return !m_recentFiles.empty() ? m_recentFiles.at(0) : std::optional<QString> {};
}

QStringList RecentFilesManager::recentFiles() const
{
    return m_recentFiles;
}

bool RecentFilesManager::hasRecentFiles() const
{
    return !m_recentFiles.empty();
}

void RecentFilesManager::setSelectedFile(QString selectedFile)
{
    juzzlin::L().debug() << "Recent file selected: " << selectedFile.toStdString();

    m_selectedFile = selectedFile;
}

QString RecentFilesManager::selectedFile() const
{
    return m_selectedFile;
}
