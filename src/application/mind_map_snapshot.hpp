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

#ifndef MIND_MAP_SNAPSHOT_HPP
#define MIND_MAP_SNAPSHOT_HPP

#include "../common/types.hpp"
#include "../domain/image_manager.hpp"
#include "../infra/io/alz_file_io_version.hpp"

#include <QString>

#include <optional>

namespace IO {
class AlzFileIO;
}

//! A compact, restorable snapshot of MindMapData, e.g. for the undo stack.
//!
//! Copying MindMapData copies all of its nodes and edges, which are full scene items and
//! thus quite heavy. Instead, the snapshot stores the mind map as XML without the image data
//! plus a copy of the image manager, whose images are implicitly shared.
class MindMapSnapshot
{
public:
    MindMapSnapshot(const IO::AlzFileIO & alzFileIO, MindMapDataS mindMapData);

    MindMapDataU restore(const IO::AlzFileIO & alzFileIO) const;

    //! \return Approximate memory usage of the serialized mind map.
    size_t xmlSizeInBytes() const;

private:
    QString m_xml;

    ImageManager m_imageManager;

    QString m_fileName;

    QString m_applicationVersion;

    std::optional<IO::AlzFormatVersion> m_alzFormatVersion;
};

#endif // MIND_MAP_SNAPSHOT_HPP
