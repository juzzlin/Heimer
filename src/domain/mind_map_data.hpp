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

#ifndef MIND_MAP_DATA_HPP
#define MIND_MAP_DATA_HPP

#include <QFont>
#include <QString>

#include <memory>

#include "../common/constants.hpp"
#include "mind_map_data_base.hpp"
#include "mind_map_stats.hpp"

#include "../infra/io/alz_file_io_version.hpp"

class Grid;
class ImageManager;
class ObjectModelLoader;
class ShadowEffectParams;

class Graph;

class MindMapData : public MindMapDataBase
{
public:
    MindMapData(QString name = "");

    MindMapData(const MindMapData & other);

    virtual ~MindMapData() override;

    void applyGrid(const Grid & grid);

    double arrowSize() const;

    void setArrowSize(double arrowSize);

    double aspectRatio() const;

    void setAspectRatio(double aspectRatio);

    QColor backgroundColor() const;

    void setBackgroundColor(const QColor & backgroundColor);

    int cornerRadius() const;

    void setCornerRadius(int cornerRadius);

    QColor edgeColor() const;

    void setEdgeColor(const QColor & edgeColor);

    QColor gridColor() const;

    void setGridColor(const QColor & edgeColor);

    double edgeWidth() const;

    void setEdgeWidth(double width);

    QString fileName() const override;

    void setFileName(QString fileName) override;

    GraphR graph() override;

    GraphCR graph() const override;

    double minEdgeLength() const;

    void setMinEdgeLength(double minEdgeLength);

    void mirror(bool vertically);

    QFont font() const;

    void changeFont(QFont font);

    void setShadowEffect(const ShadowEffectParams & params);

    int textSize() const;

    void setTextSize(int textSize);

    QString applicationVersion() const;

    void setApplicationVersion(const QString & applicationVersion);

    std::optional<IO::AlzFormatVersion> alzFormatVersion() const;

    void setAlzFormatVersion(IO::AlzFormatVersion alzFormatVersion);

    MindMapStats stats() const;

    ImageManager & imageManager();

    const ImageManager & imageManager() const;

private:
    void copyGraph(const MindMapData & other);

    struct LayoutOptimizerParameters
    {
        double aspectRatio = Constants::LayoutOptimizer::Defaults::ASPECT_RATIO;

        double minEdgeLength = Constants::LayoutOptimizer::Defaults::MIN_EDGE_LENGTH;
    };

    QString m_fileName;

    QString m_applicationVersion;

    std::optional<IO::AlzFormatVersion> m_alzFormatVersion;

    struct Style;
    std::unique_ptr<Style> m_style;

    std::unique_ptr<Graph> m_graph;

    std::unique_ptr<ImageManager> m_imageManager;

    LayoutOptimizerParameters m_layoutOptimizerParameters;
};

#endif // MIND_MAP_DATA_HPP
