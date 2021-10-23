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

        if (m_mindMapData->graph().numNodes() == 0) {
            juzzlin::L().info() << "No nodes";
            return false;
        }

        double area = 0;
        for (auto && node : m_mindMapData->graph().getNodes()) {
            area += (node->size().width() + minEdgeLength) * (node->size().height() + minEdgeLength);
        }

        const double height = std::sqrt(area / aspectRatio);
        const double width = area / height;

        // Builds initial layout

        auto nodes = m_mindMapData->graph().getNodes();
        m_layout = std::make_unique<Layout>();
        m_layout->cols = static_cast<size_t>(width / (Constants::Node::MIN_WIDTH + minEdgeLength)) + 1;
        m_layout->minEdgeLength = minEdgeLength;
        std::map<int, std::shared_ptr<Cell>> nodesToCells; // Used when building connections
        const auto rows = static_cast<size_t>(height / (Constants::Node::MIN_HEIGHT + minEdgeLength)) + 1;
        for (size_t j = 0; j < rows; j++) {
            const auto row = std::make_shared<Row>();
            row->rect.x = 0;
            row->rect.y = static_cast<int>(j) * Constants::Node::MIN_HEIGHT;
            for (size_t i = 0; i < m_layout->cols; i++) {
                const auto cell = std::make_shared<Cell>();
                row->cells.push_back(cell);
                cell->rect.x = row->rect.x + static_cast<int>(i) * Constants::Node::MIN_WIDTH;
                cell->rect.y = row->rect.y;
                cell->rect.h = Constants::Node::MIN_HEIGHT;
                cell->rect.w = Constants::Node::MIN_WIDTH;

                if (!nodes.empty()) {
                    m_layout->all.push_back(cell);
                    cell->node = nodes.back();
                    nodesToCells[cell->node.lock()->index()] = cell;
                    nodes.pop_back();
                }
            }
            m_layout->rows.push_back(row);
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
        double cost = calculateCost();
        oi.initialCost = cost;
        juzzlin::L().info() << "Initial cost: " << oi.initialCost;

        std::uniform_real_distribution<double> dist { 0, 1 };

        // TODO: Automatically decide optimal t
        const double t0 = 200;
        double t = t0;
        while (t > 0.05 && cost > 0) {
            double acceptRatio = 0;
            int stuck = 0;
            do {
                double accepts = 0;
                double rejects = 0;
                double sliceCost = cost;
                for (size_t i = 0; i < m_layout->all.size() * 200; i++) {
                    const auto change = planChange();

                    LayoutOptimizer::Impl::Cell::globalMoveId++;

                    double newCost = cost;
                    newCost -= change.sourceCell->getCost();
                    newCost -= change.targetCell->getCost();

                    doChange(change);
                    oi.changes++;

                    LayoutOptimizer::Impl::Cell::globalMoveId++;

                    newCost += change.sourceCell->getCost();
                    newCost += change.targetCell->getCost();

                    const double delta = newCost - cost;
                    if (delta <= 0) {
                        cost = newCost;
                        accepts++;
                    } else {
                        if (dist(m_engine) < std::exp(-delta / t)) {
                            cost = newCost;
                            accepts++;
                        } else {
                            undoChange(change);
                            rejects++;
                        }
                    }
                }

                acceptRatio = accepts / (rejects + 1);
                const double gain = (cost - sliceCost) / sliceCost;
                juzzlin::L().debug() << "Cost: " << cost << " (" << gain * 100 << "%)"
                                     << " acc: " << acceptRatio << " t: " << t;

                if (gain < 0.1) {
                    stuck++;
                } else {
                    stuck = 0;
                }

            } while (stuck < 5);

            t *= 0.7;

            updateProgress(std::min(1.0, 1.0 - std::log(t) / std::log(t0)));
        }

        oi.finalCost = cost;

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
    double calculateCost() const
    {
        double cost = 0;
        for (auto cell : m_layout->all) {
            cost += cell->getCost();
        }
        return cost;
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

    Change planChange()
    {
        std::uniform_int_distribution<size_t> rowDist { 0, m_layout->rows.size() - 1 };

        Change change;
        change.type = Change::Type::Swap;
        size_t sourceRowIndex = 0;
        size_t targetRowIndex = 0;

        do {
            sourceRowIndex = rowDist(m_engine);
            change.sourceRow = m_layout->rows.at(sourceRowIndex);
            if (change.sourceRow->cells.empty()) {
                continue;
            }
            std::uniform_int_distribution<size_t> sourceCellDist { 0, change.sourceRow->cells.size() - 1 };
            change.sourceIndex = sourceCellDist(m_engine);
            change.sourceCell = change.sourceRow->cells.at(change.sourceIndex);

            targetRowIndex = rowDist(m_engine);
            change.targetRow = m_layout->rows.at(targetRowIndex);
            if (change.targetRow->cells.empty()) {
                continue;
            }
            std::uniform_int_distribution<size_t> targetCellDist { 0, change.targetRow->cells.size() - 1 };
            change.targetIndex = targetCellDist(m_engine);
            change.targetCell = change.targetRow->cells.at(change.targetIndex);

        } while (change.sourceCell == change.targetCell);

        return change;
    }

    MindMapDataPtr m_mindMapData;

    const Grid & m_grid;

    struct Rect
    {
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
            double cost = 0;
            for (auto && cell : all) {
                cost += distance(*cell);
            }

            return cost;
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
