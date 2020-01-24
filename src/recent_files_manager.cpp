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

#include <stdexcept>

#include <QSettings>

std::unique_ptr<RecentFilesManager> RecentFilesManager::m_instance;

RecentFilesManager::RecentFilesManager()
{
    if (RecentFilesManager::m_instance) {
        throw std::runtime_error("RecentFilesManager already instantiated!");
    }

    QSettings settings;
    const int size = settings.beginReadArray(Constants::RecentFiles::QSETTINGS_ARRAY_KEY);
    for (int i = 0; i < size; i++) {
        settings.setArrayIndex(i);
        m_recentFiles.push_back(settings.value(Constants::RecentFiles::QSETTINGS_FILE_PATH_KEY).toString());
    }
    settings.endArray();
}

RecentFilesManager & RecentFilesManager::instance()
{
    if (!RecentFilesManager::m_instance) {
        RecentFilesManager::m_instance.reset(new RecentFilesManager);
        juzzlin::L().debug() << "RecentFilesManager created";
    }

    return *RecentFilesManager::m_instance;
}

void RecentFilesManager::addRecentFile(QString filePath)
{
    m_recentFiles.removeAll(filePath);
    m_recentFiles.push_front(filePath);

    while (m_recentFiles.size() > Constants::RecentFiles::MAX_FILES) {
        m_recentFiles.pop_back();
    }

    juzzlin::L().debug() << "Added recent file: " << filePath.toStdString();
    juzzlin::L().debug() << "Recent file count: " << m_recentFiles.size();

    QSettings settings;
    settings.beginWriteArray(Constants::RecentFiles::QSETTINGS_ARRAY_KEY);
    for (int i = 0; i < m_recentFiles.size(); i++) {
        settings.setArrayIndex(i);
        settings.setValue(Constants::RecentFiles::QSETTINGS_FILE_PATH_KEY, m_recentFiles.at(i));
    }
    settings.endArray();
}

const QList<QString> & RecentFilesManager::getRecentFiles() const
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
