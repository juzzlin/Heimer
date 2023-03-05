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

#include "alz_file_io.hpp"
#include "alz_file_io_worker.hpp"

#include "../core/mind_map_data.hpp"

#include <QApplication>
#include <QThread>

Q_DECLARE_METATYPE(MindMapDataS)

namespace IO {

AlzFileIO::AlzFileIO(AlzFormatVersion outputVersion)
  : m_worker(std::make_unique<AlzFileIOWorker>(outputVersion))
  , m_workerThread(std::make_unique<QThread>())
{
    qRegisterMetaType<MindMapDataS>();

    if (!m_workerThread->isRunning()) {
        m_workerThread->start();
        m_worker->moveToThread(m_workerThread.get());
    }
}

void AlzFileIO::finish()
{
    m_workerThread->quit();
    m_workerThread->wait();
}

MindMapDataU AlzFileIO::fromFile(QString path) const
{
    return m_worker->fromFile(path);
}

bool AlzFileIO::toFile(MindMapDataS mindMapData, QString path, bool async) const
{
    const auto connectionType = async ? Qt::QueuedConnection : Qt::BlockingQueuedConnection;

    return QMetaObject::invokeMethod(m_worker.get(), "toFile", connectionType,
                                     Q_ARG(MindMapDataS, mindMapData),
                                     Q_ARG(QString, path));
}

MindMapDataU AlzFileIO::fromXml(QString xml) const
{
    return m_worker->fromXml(xml);
}

QString AlzFileIO::toXml(MindMapDataS mindMapData) const
{
    return m_worker->toXml(mindMapData);
}

AlzFileIO::~AlzFileIO()
{
    AlzFileIO::finish();
}

} // namespace IO
