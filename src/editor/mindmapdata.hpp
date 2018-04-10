// This file is part of Dementia.
// Copyright (C) 2018 Jussi Lind <jussi.lind@iki.fi>
//
// Dementia is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dementia is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dementia. If not, see <http://www.gnu.org/licenses/>.

#ifndef MINDMAPDATA_HPP
#define MINDMAPDATA_HPP

#include <QString>

#include "mindmapdatabase.hpp"
#include "graph.hpp"

class ObjectModelLoader;

class MindMapData : public MindMapDataBase
{
public:

    MindMapData(QString name = "");

    MindMapData(const MindMapData & other);

    virtual ~MindMapData();

    QColor backgroundColor() const;

    void setBackgroundColor(const QColor & backgroundColor);

    QString fileName() const;

    void setFileName(QString fileName);

    Graph & graph();

    const Graph & graph() const;

    QString version() const;

    void setVersion(const QString &version);

private:

    void copyGraph(const MindMapData & other);

    QString m_fileName;

    QString m_version;

    QColor m_backgroundColor = QColor(128, 200, 255, 255);

    Graph m_graph;
};

typedef std::shared_ptr<MindMapData> MindMapDataPtr;

#endif // MINDMAPDATA_HPP
