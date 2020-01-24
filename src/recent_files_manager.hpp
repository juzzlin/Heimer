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

#ifndef RECENT_FILES_MANAGER_HPP
#define RECENT_FILES_MANAGER_HPP

#include <QList>
#include <QObject>
#include <QString>

#include <memory>

class RecentFilesManager : public QObject
{
public:
    RecentFilesManager();

    static RecentFilesManager & instance();

    const QList<QString> & getRecentFiles() const;

    bool hasRecentFiles() const;

    QString selectedFile() const;

public slots:
    void addRecentFile(QString filePath);

    void setSelectedFile(QString filePath);

private:
    static std::unique_ptr<RecentFilesManager> m_instance;

    QList<QString> m_recentFiles;

    QString m_selectedFile;
};

#endif // RECENT_FILES_MANAGER_HPP
