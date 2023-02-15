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

#ifndef ALZ_FILE_IO_WORKER_HPP
#define ALZ_FILE_IO_WORKER_HPP

#include <QObject>
#include <QString>

#include <memory>

#include "../types.hpp"

namespace IO {

class AlzFileIOWorker : public QObject
{
    Q_OBJECT

public:
    ~AlzFileIOWorker();

public slots:

    MindMapDataU fromFile(QString path) const;

    bool toFile(MindMapDataS mindMapData, QString path) const;

    MindMapDataU fromXml(QString xml) const;

    QString toXml(MindMapDataS mindMapData) const;
};

} // namespace IO

#endif // ALZ_FILE_IO_WORKER_HPP
