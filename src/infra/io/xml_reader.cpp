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

#include "xml_reader.hpp"

#include "file_exception.hpp"

#include <QFile>
#include <QObject>

namespace IO {

QDomDocument XmlReader::readFromFile(QString filePath)
{
    QDomDocument doc;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        throw FileException(QObject::tr("Cannot open file: '") + filePath + "'");
    }

    if (!doc.setContent(&file)) {
        file.close();

        throw FileException(QObject::tr("Corrupted file: '") + filePath + "'");
    }

    file.close();

    return doc;
}

} // namespace IO
