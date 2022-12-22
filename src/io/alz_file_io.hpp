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

#ifndef ALZ_FILE_IO_HPP
#define ALZ_FILE_IO_HPP

#include "file_io.hpp"

#include <memory>

class MindMapData;
class QThread;

namespace IO {

class AlzFileIOWorker;

class AlzFileIO : public FileIO
{
public:
    AlzFileIO();

    ~AlzFileIO();

    void finish() override;

    std::unique_ptr<MindMapData> fromFile(QString path) const override;

    bool toFile(std::shared_ptr<MindMapData> mindMapData, QString path, bool async) const override;

    std::unique_ptr<MindMapData> fromXml(QString xml) const;

    QString toXml(std::shared_ptr<MindMapData> mindMapData) const;

private:
    std::unique_ptr<AlzFileIOWorker> m_worker;

    std::unique_ptr<QThread> m_workerThread;
};

} // namespace IO

#endif // ALZ_FILE_IO_HPP