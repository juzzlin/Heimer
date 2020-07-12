// This file is part of Heimer.
// Copyright (C) 2020 Jussi Lind <jussi.lind@iki.fi>
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

#include "layout_optimizer_test.hpp"
#include "contrib/SimpleLogger/src/simple_logger.hpp"
#include "grid.hpp"
#include "layout_optimizer.hpp"
#include "mind_map_data.hpp"
#include "test_mode.hpp"

#include <iostream>
#include <random>

LayoutOptimizerTest::LayoutOptimizerTest()
{
    TestMode::setEnabled(true);
}

void LayoutOptimizerTest::testSingleNode_ShouldNotDoAnything()
{
    auto data = std::make_shared<MindMapData>();
    auto node = std::make_shared<Node>();
    data->graph().addNode(node);

    Grid grid;
    LayoutOptimizer lol { data, grid };
    lol.initialize(1.0, 50);
    const auto optimizationInfo = lol.optimize();

    QCOMPARE(optimizationInfo.initialCost, 0.0);
    QCOMPARE(optimizationInfo.finalCost, 0.0);
    QCOMPARE(optimizationInfo.changes, static_cast<size_t>(0));
}

void LayoutOptimizerTest::testMultipleNodes_ShouldReduceCost()
{
    auto data = std::make_shared<MindMapData>();
    const size_t nodeCount = 50;
    std::uniform_real_distribution<double> xDist { -1000, 1000 };
    std::uniform_real_distribution<double> yDist { -1000, 1000 };
    std::mt19937 engine;
    std::vector<std::shared_ptr<Node>> nodes;
    for (size_t i = 0; i < nodeCount; i++) {
        auto node = std::make_shared<Node>();
        data->graph().addNode(node);
        node->setPos({ xDist(engine), yDist(engine) });
        nodes.push_back(node);
    }
    std::uniform_int_distribution<size_t> iDist { 0, nodes.size() - 1 };
    for (auto && node : nodes) {
        for (size_t i = 0; i < (i % 2) + 1; i++) {
            const auto otherNode = data->graph().getNode(static_cast<int>(iDist(engine)));
            data->graph().addEdge(std::make_shared<Edge>(*node, *otherNode));
        }
    }

    Grid grid;
    grid.setSize(10);
    LayoutOptimizer lol { data, grid };
    lol.initialize(1.0, 50);
    double progress = 0;
    lol.setProgressCallback([&](double progress_) {
        progress = progress_;
    });
    const auto optimizationInfo = lol.optimize();
    QCOMPARE(progress, 1.0);
    QVERIFY(optimizationInfo.changes > 100);
    const double gain = (optimizationInfo.finalCost - optimizationInfo.initialCost) / optimizationInfo.initialCost;
    juzzlin::L().info() << "Final cost: " << optimizationInfo.finalCost << " (" << gain * 100 << "%)";
    QVERIFY(gain < -0.3);

    lol.extract();
    for (auto && node : nodes) {
        QCOMPARE(node->pos().x(), static_cast<double>(static_cast<int>(node->pos().x() / grid.size()) * grid.size()));
        QCOMPARE(node->pos().y(), static_cast<double>(static_cast<int>(node->pos().y() / grid.size()) * grid.size()));
    }
}

QTEST_GUILESS_MAIN(LayoutOptimizerTest)
