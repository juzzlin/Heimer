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

#include "layout_optimizer.hpp"
#include "constants.hpp"
#include "contrib/SimpleLogger/src/simple_logger.hpp"
#include "graph.hpp"
#include "grid.hpp"
#include "mind_map_data.hpp"
#include "node.hpp"

#include <cassert>
#include <cmath>
#include <cstddef>
#include <memory>
#include <numeric>
#include <random>
#include <vector>

class Graph;

namespace {
inline bool equals(double x, double y)
{
    return std::fabs(x - y) < 0.001;
}

inline double dot(double x1, double y1, double x2, double y2)
{
    return x1 * x2 + y1 * y2;
}
} // namespace

class LayoutOptimizer::Impl
{
public:
    Impl(MindMapDataPtr mindMapData, const Grid & grid)
      : m_mindMapData(mindMapData)
      , m_grid(grid)
    {
    }

    ~Impl() = default;

    bool initialize(double aspectRatio, double minEdgeLength)
    {
        juzzlin::L().info() << "Initializing LayoutOptimizer: aspectRatio=" << aspectRatio << ", minEdgeLength=" << minEdgeLength;

        const auto nodes = m_mindMapData->graph().getNodes();
        if (nodes.empty()) {
            juzzlin::L().info() << "No nodes";
            return false;
        }

        double area = 0;
        for (auto && node : nodes) {
            area += (node->size().width() + minEdgeLength) * (node->size().height() + minEdgeLength);
        }

        // Build initial layout

        const auto originalLayoutDimensions = calculateLayoutDimensions(nodes);
        juzzlin::L().info() << "Area: " << originalLayoutDimensions.height() * originalLayoutDimensions.width();
        const double height = std::sqrt(area / aspectRatio);
        const double width = area / height;
        m_layout = std::make_unique<Layout>();
        m_layout->cols = static_cast<size_t>(width / (Constants::Node::MIN_WIDTH + minEdgeLength)) + 1;
        m_layout->minEdgeLength = minEdgeLength;
        std::map<int, std::shared_ptr<Cell>> nodesToCells; // Used when building connections
        std::vector<std::shared_ptr<Cell>> cells;
        const auto rows = static_cast<size_t>(height / (Constants::Node::MIN_HEIGHT + minEdgeLength)) + 1;
        for (size_t j = 0; j < rows; j++) {
            const auto row = std::make_shared<Row>();
            row->rect.x = 0;
            row->rect.y = static_cast<int>(j) * Constants::Node::MIN_HEIGHT;
            for (size_t i = 0; i < m_layout->cols; i++) {
                const auto cell = std::make_shared<Cell>();
                row->cells.push_back(cell);
                cells.push_back(cell);
                cell->rect = { row->rect.x + static_cast<int>(i) * Constants::Node::MIN_WIDTH,
                               row->rect.y,
                               Constants::Node::MIN_HEIGHT,
                               Constants::Node::MIN_WIDTH };
            }
            m_layout->rows.push_back(row);
        }

        m_rowDist = std::uniform_int_distribution<size_t> { 0, m_layout->rows.size() - 1 };

        // Assign nodes to nearest cells

        double minX = std::numeric_limits<double>::max();
        double maxX = -minX;
        double minY = std::numeric_limits<double>::max();
        double maxY = -minY;
        for (auto && cell : cells) {
            minX = std::min(minX, cell->x());
            maxX = std::max(maxX, cell->x());
            minY = std::min(minY, cell->y());
            maxY = std::max(maxY, cell->y());
        }
        const double cellAreaW = maxX - minX;
        const double cellAreaH = maxY - minY;

        for (auto && node : nodes) {
            if (!cells.empty()) {
                size_t nearestCellIndex = 0;
                double nearestDistance = std::numeric_limits<double>::max();
                for (size_t i = 0; i < cells.size(); i++) {
                    const auto cell = cells.at(i);
                    const auto dx = (cell->x() - minX - cellAreaW / 2) / cellAreaW - (node->location().x() - originalLayoutDimensions.x() - originalLayoutDimensions.width() / 2) / originalLayoutDimensions.width();
                    const auto dy = (cell->y() - minY - cellAreaH / 2) / cellAreaH - (node->location().y() - originalLayoutDimensions.y() - originalLayoutDimensions.height() / 2) / originalLayoutDimensions.height();
                    if (double distance = dx * dx + dy * dy; distance < nearestDistance) {
                        nearestDistance = distance;
                        nearestCellIndex = i;
                    }
                }
                const auto cell = cells.at(nearestCellIndex);
                cell->node = node;
                cells.at(nearestCellIndex) = cells.back();
                cells.pop_back();
                nodesToCells[node->index()] = cell;
                m_layout->all.push_back(cell);
            } else {
                return false;
            }
        }

        // Setup connections

        for (auto && edge : m_mindMapData->graph().getEdges()) {
            const auto cell0 = nodesToCells[edge->sourceNode().index()];
            assert(cell0);
            const auto cell1 = nodesToCells[edge->targetNode().index()];
            assert(cell1);
            cell0->all.push_back(cell1);
            cell1->all.push_back(cell0);
        }

        return true;
    }

    OptimizationInfo optimize()
    {
        if (m_layout->all.size() < 2) {
            return {};
        }

        OptimizationInfo oi;
        oi.initialCost = calculateCost();
        oi.currentCost = oi.initialCost;
        oi.sliceSize = m_layout->all.size() * 200;
        oi.t0 = 33;
        oi.tC = oi.t0;

        juzzlin::L().info() << "Initial cost: " << oi.initialCost;

        while (oi.tC > oi.t1 && oi.currentCost > 0) {
            oi.acceptRatio = 0;
            size_t stuckCounter = 0;
            do {
                oi.accepts = 0;
                oi.rejects = 0;
                double sliceCost = oi.currentCost;
                for (size_t i = 0; i < oi.sliceSize; i++) {
                    changeLayoutAndUpdateCost(oi);
                }
                oi.acceptRatio = static_cast<double>(oi.accepts) / static_cast<double>(oi.rejects + 1);
                const double gain = (oi.currentCost - sliceCost) / sliceCost;
                juzzlin::L().debug() << "Cost: " << oi.currentCost << " (" << gain * 100 << "%)"
                                     << " acc: " << oi.acceptRatio << " t: " << oi.tC;
                stuckCounter = gain < oi.stuckTh ? stuckCounter + 1 : 0;

            } while (stuckCounter < oi.stuckLimit);

            oi.tC *= oi.cS;

            updateProgress(std::min(1.0, 1.0 - std::log(oi.tC) / std::log(oi.t0)));
        }

        oi.finalCost = oi.currentCost;

        return oi;
    }

    void updateProgress(double val)
    {
        if (m_progressCallback) {
            m_progressCallback(val);
        }
    }

    void extract()
    {
        if (m_layout->all.empty()) {
            return;
        }

        m_layout->spread();
        m_layout->applyCoordinates(m_grid);
    }

    void setProgressCallback(ProgressCallback progressCallback)
    {
        m_progressCallback = progressCallback;
    }

private:
    QRectF calculateLayoutDimensions(const Graph::NodeVector & nodes) const
    {
        if (nodes.empty()) {
            return {};
        }
        QRectF dimensions = nodes.at(0)->placementBoundingRect();
        for (auto && node : nodes) {
            dimensions = dimensions.united(node->placementBoundingRect().translated(node->location()));
        }
        return dimensions;
    }

    double calculateCost() const
    {
        return std::accumulate(std::begin(m_layout->all), std::end(m_layout->all), double {},
                               [](auto totalCost, auto && cell) {
                                   return totalCost + cell->getCost();
                               });
    }

    struct Cell;

    struct Row;

    using CellVector = std::vector<std::shared_ptr<Cell>>;

    struct Change
    {
        enum class Type : uint64_t
        {
            Move,
            Swap
        };

        Type type;

        std::shared_ptr<Cell> sourceCell;

        std::shared_ptr<Cell> targetCell;

        std::shared_ptr<Row> sourceRow;

        std::shared_ptr<Row> targetRow;

        size_t sourceIndex = 0;

        size_t targetIndex = 0;
    };

    void changeLayoutAndUpdateCost(OptimizationInfo & oi)
    {
        const auto change = planChange();
        LayoutOptimizer::Impl::Cell::globalMoveId++;
        double newCost = oi.currentCost;
        newCost -= change.sourceCell->getCost();
        newCost -= change.targetCell->getCost();
        doChange(change);
        oi.changes++;
        LayoutOptimizer::Impl::Cell::globalMoveId++;
        newCost += change.sourceCell->getCost();
        newCost += change.targetCell->getCost();
        if (const double delta = newCost - oi.currentCost; delta <= 0) {
            oi.currentCost = newCost;
            oi.accepts++;
        } else {
            if (m_saDist(m_engine) < std::exp(-delta / oi.tC)) {
                oi.currentCost = newCost;
                oi.accepts++;
            } else {
                undoChange(change);
                oi.rejects++;
            }
        }
    }

    void doChange(const Change & change)
    {
        change.sourceRow->cells.at(change.sourceIndex) = change.targetCell;
        change.targetRow->cells.at(change.targetIndex) = change.sourceCell;
        change.sourceCell->pushRect();
        change.sourceCell->rect.x = change.targetRow->rect.x + static_cast<int>(change.targetIndex) * Constants::Node::MIN_WIDTH;
        change.sourceCell->rect.y = change.targetRow->rect.y;
        change.targetCell->pushRect();
        change.targetCell->rect.x = change.sourceRow->rect.x + static_cast<int>(change.sourceIndex) * Constants::Node::MIN_WIDTH;
        change.targetCell->rect.y = change.sourceRow->rect.y;
    }

    void undoChange(const Change & change)
    {
        change.sourceRow->cells.at(change.sourceIndex) = change.sourceCell;
        change.targetRow->cells.at(change.targetIndex) = change.targetCell;
        change.sourceCell->popRect();
        change.targetCell->popRect();
    }

    // Note: Here we plan only very local changes with a very small search radius as we assume that the nodes are already relatively well placed globally.
    Change planChange()
    {
        Change change;
        change.type = Change::Type::Swap;
        size_t sourceRowIndex = 0;
        size_t targetRowIndex = 0;

        do {
            sourceRowIndex = m_rowDist(m_engine);
            change.sourceRow = m_layout->rows.at(sourceRowIndex);
            if (change.sourceRow->cells.empty()) {
                continue;
            }
            std::uniform_int_distribution<size_t> sourceCellDist { 0, change.sourceRow->cells.size() - 1 };
            change.sourceIndex = sourceCellDist(m_engine);
            change.sourceCell = change.sourceRow->cells.at(change.sourceIndex);

            const auto rowDelta = m_oneOrTwoDist(m_engine);
            targetRowIndex = sourceRowIndex + rowDelta < m_layout->rows.size() ? sourceRowIndex + rowDelta : sourceRowIndex;
            change.targetRow = m_layout->rows.at(targetRowIndex);
            if (change.targetRow->cells.empty()) {
                continue;
            }

            const auto cellDelta = m_oneOrTwoDist(m_engine);
            change.targetIndex = change.sourceIndex + cellDelta < change.targetRow->cells.size() ? change.sourceIndex + cellDelta : change.sourceIndex;
            change.targetCell = change.targetRow->cells.at(change.targetIndex);

        } while (change.sourceCell == change.targetCell);

        return change;
    }

    MindMapDataPtr m_mindMapData;

    const Grid & m_grid;

    struct Rect
    {
        Rect()
        {
        }

        Rect(int x, int y, int w, int h)
          : x(x)
          , y(y)
          , w(w)
          , h(h)
        {
        }

        int x = 0;

        int y = 0;

        int w = 0;

        int h = 0;
    };

    struct Cell
    {
        inline double getCost()
        {
            double overlapCost = getOverlapCost();
            for (auto && dependency : all) {
                overlapCost += dependency->getOverlapCost();
            }
            return overlapCost + getConnectionCost();
        }

        inline void popRect()
        {
            rect = m_stash;
        }

        inline void pushRect()
        {
            m_stash = rect;
        }

        inline double x() const
        {
            return rect.x + rect.w / 2;
        }

        inline double y() const
        {
            return rect.y + rect.h / 2;
        }

        std::weak_ptr<Node> node;

        CellVector all;

        Rect rect;

        static size_t globalMoveId;

    private:
        inline double distance(Cell & other)
        {
            const auto dx = x() - other.x();
            const auto dy = y() - other.y();
            return std::sqrt(dx * dx + dy * dy);
        }

        inline double overlapCost(Cell & c1, Cell & c2)
        {
            const auto x1 = c1.x() - x();
            const auto y1 = c1.y() - y();
            const auto x2 = c2.x() - x();
            const auto y2 = c2.y() - y();
            if (equals(x1 * y2, x2 * y1) && dot(x1, y1, x2, y2) > 0) {
                const auto l1 = x1 * x1 + y1 * y1;
                const auto l2 = x2 * x2 + y2 * y2;
                return l1 < l2 ? 2 * std::sqrt(l1) : 2 * std::sqrt(l2);
            }

            return 0;
        }

        inline double getConnectionCost()
        {
            return std::accumulate(std::begin(all), std::end(all), double {},
                                   [this](auto totalCost, auto && cell) {
                                       return totalCost + distance(*cell);
                                   });
        }

        inline double getOverlapCost()
        {
            if (m_moveId == Cell::globalMoveId) {
                return 0;
            }

            m_moveId = Cell::globalMoveId;

            double cost = 0;
            for (size_t i = 0; i < all.size(); i++) {
                const auto outer = all.at(i);
                for (size_t j = i + 1; j < all.size(); j++) {
                    const auto inner = all.at(j);
                    if (outer != inner) {
                        cost += overlapCost(*outer, *inner);
                    }
                }
            }

            return cost;
        }

        Rect m_stash;

        size_t m_moveId = 0;
    };

    struct Row
    {
        CellVector cells;

        Rect rect;
    };

    using RowVector = std::vector<std::shared_ptr<Row>>;

    struct Layout
    {
        void applyCoordinates(const Grid & grid)
        {
            double maxWidth = 0;
            double maxHeight = 0;
            for (auto && cell : all) {
                if (cell) {
                    maxHeight = std::fmax(maxHeight, cell->rect.y + cell->rect.h);
                    maxWidth = std::fmax(maxWidth, cell->rect.x + cell->rect.w);
                }
            }
            for (auto && cell : all) {
                cell->node.lock()->setLocation(grid.snapToGrid({ cell->rect.x - maxWidth / 2, cell->rect.y - maxHeight / 2 }));
            }
        }

        double getMaxColX(size_t colIndex)
        {
            double maxX = 0;
            for (auto && row : rows) {
                if (const auto cell = row->cells.at(colIndex); cell && cell->node.lock()) {
                    maxX = std::max(maxX, cell->x() + cell->node.lock()->size().width() / 2);
                }
            }
            return maxX;
        }

        std::pair<double, bool> getMinColX(size_t colIndex)
        {
            std::pair<double, bool> minX { std::numeric_limits<double>::max(), false };
            for (auto && row : rows) {
                if (const auto cell = row->cells.at(colIndex); cell && cell->node.lock()) {
                    minX = { std::min(minX.first, cell->x() - cell->node.lock()->size().width() / 2), true };
                }
            }
            return minX;
        }

        double getMaxRowY(size_t rowIndex)
        {
            double maxY = 0;
            for (auto && cell : rows.at(rowIndex)->cells) {
                if (cell && cell->node.lock()) {
                    maxY = std::max(maxY, cell->y() + cell->node.lock()->size().height() / 2);
                }
            }
            return maxY;
        }

        std::pair<double, bool> getMinRowY(size_t rowIndex)
        {
            std::pair<double, bool> minY { std::numeric_limits<double>::max(), false };
            for (auto && cell : rows.at(rowIndex)->cells) {
                if (cell && cell->node.lock()) {
                    minY = { std::min(minY.first, cell->y() - cell->node.lock()->size().height() / 2), true };
                }
            }
            return minY;
        }

        void spread()
        {
            for (size_t i = 1; i < cols; i++) {
                const auto prevMaxX = getMaxColX(i - 1) + minEdgeLength;
                const auto minX = getMinColX(i);
                if (minX.second && minX.first < prevMaxX) {
                    for (auto && row : rows) {
                        if (const auto cell = row->cells.at(i); cell) {
                            cell->rect.x += static_cast<int>(prevMaxX - minX.first);
                        }
                    }
                }
            }

            for (size_t j = 1; j < rows.size(); j++) {
                const auto prevMaxY = getMaxRowY(j - 1) + minEdgeLength;
                const auto minY = getMinRowY(j);
                if (minY.second && minY.first < prevMaxY) {
                    for (auto && cell : rows.at(j)->cells) {
                        if (cell) {
                            cell->rect.y += static_cast<int>(prevMaxY - minY.first);
                        }
                    }
                }
            }
        }

        double minEdgeLength = 0;

        CellVector all;

        RowVector rows;

        size_t cols = 0;
    };

    std::unique_ptr<Layout> m_layout;

    std::mt19937 m_engine;

    // Will be initialized once we now the row count after building the initial layout
    std::uniform_int_distribution<size_t> m_rowDist;

    std::uniform_int_distribution<size_t> m_oneOrTwoDist { 0, 1 };

    std::uniform_real_distribution<double> m_saDist { 0, 1 };

    ProgressCallback m_progressCallback = nullptr;
};

size_t LayoutOptimizer::Impl::Cell::globalMoveId = 0;

LayoutOptimizer::LayoutOptimizer(MindMapDataPtr mindMapData, const Grid & grid)
  : m_impl(std::make_unique<Impl>(mindMapData, grid))
{
}

bool LayoutOptimizer::initialize(double aspectRatio, double minEdgeLength)
{
    return m_impl->initialize(aspectRatio, minEdgeLength);
}

LayoutOptimizer::OptimizationInfo LayoutOptimizer::optimize()
{
    return m_impl->optimize();
}

void LayoutOptimizer::extract()
{
    m_impl->extract();
}

void LayoutOptimizer::setProgressCallback(ProgressCallback progressCallback)
{
    m_impl->setProgressCallback(progressCallback);
}

LayoutOptimizer::~LayoutOptimizer() = default;
