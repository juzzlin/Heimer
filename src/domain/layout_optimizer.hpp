// This file is part of Heimer.
// Copyright (C) 2019 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef LAYOUT_OPTIMIZER_HPP
#define LAYOUT_OPTIMIZER_HPP

#include <functional>
#include <memory>

#include "../common/types.hpp"

class Grid;

class LayoutOptimizer
{
public:
    LayoutOptimizer(MindMapDataS mindMapData, const Grid & grid);

    ~LayoutOptimizer();

    bool initialize(double aspectRatio, double minEdgeLength);

    struct OptimizationInfo
    {
        double acceptRatio = 0;

        double currentCost = 0;

        double finalCost = 0;

        double initialCost = 0;

        double tC = 0;

        double t0 = 0;

        double t1 = 0.05;

        double cS = 0.7;

        size_t accepts = 0;

        size_t changes = 0;

        size_t rejects = 0;

        size_t sliceSize = 0;

        size_t stuckLimit = 5;

        double stuckTh = 0.1;
    };

    OptimizationInfo optimize();

    void extract();

    using ProgressCallback = std::function<void(double)>;
    void setProgressCallback(ProgressCallback progressCallback);

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

#endif // LAYOUT_OPTIMIZER_HPP
