// This file is part of Heimer.
// Copyright (C) 2026 Jussi Lind <jussi.lind@iki.fi>
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

#include "mind_map_snapshot.hpp"

#include "../domain/mind_map_data.hpp"
#include "../infra/io/alz_file_io.hpp"

MindMapSnapshot::MindMapSnapshot(const IO::AlzFileIO & alzFileIO, MindMapDataS mindMapData)
  : m_xml(alzFileIO.toSnapshotXml(mindMapData))
  , m_imageManager(mindMapData->imageManager())
  , m_fileName(mindMapData->fileName())
  , m_applicationVersion(mindMapData->applicationVersion())
  , m_alzFormatVersion(mindMapData->alzFormatVersion())
{
}

MindMapDataU MindMapSnapshot::restore(const IO::AlzFileIO & alzFileIO) const
{
    auto mindMapData = alzFileIO.fromXml(m_xml);
    mindMapData->imageManager() = m_imageManager;
    mindMapData->setFileName(m_fileName);
    mindMapData->setApplicationVersion(m_applicationVersion);
    if (m_alzFormatVersion) {
        mindMapData->setAlzFormatVersion(*m_alzFormatVersion);
    }
    return mindMapData;
}

size_t MindMapSnapshot::xmlSizeInBytes() const
{
    return static_cast<size_t>(m_xml.size()) * sizeof(QChar);
}
