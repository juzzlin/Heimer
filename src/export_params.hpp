// This file is part of Heimer.
// Copyright (C) 2023 Jussi Lind <jussi.lind@iki.fi>
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

#include <QSize>
#include <QString>

struct ExportParams
{
    ExportParams(QString fileName)
      : fileName(fileName)
    {
    }

    ExportParams(QString fileName, QSize imageSize, bool transparentBackground)
      : fileName(fileName)
      , imageSize(imageSize)
      , transparentBackground(transparentBackground)
    {
    }

    QString fileName;

    QSize imageSize;

    bool transparentBackground = false;
};
