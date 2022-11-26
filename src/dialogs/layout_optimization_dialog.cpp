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

#include "layout_optimization_dialog.hpp"

#include "../constants.hpp"
#include "../layout_optimizer.hpp"
#include "../mind_map_data.hpp"

#include "widget_factory.hpp"

#include "simple_logger.hpp"

#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QProgressBar>
#include <QTimer>
#include <QVBoxLayout>

namespace Dialogs {

LayoutOptimizationDialog::LayoutOptimizationDialog(QWidget & parent, MindMapData & mindMapData, LayoutOptimizer & layoutOptimizer)
  : QDialog(&parent)
  , m_mindMapData(mindMapData)
  , m_layoutOptimizer(layoutOptimizer)
{
    setWindowTitle(tr("Optimize Layout"));

    setMinimumWidth(480);

    initWidgets(mindMapData);

    m_layoutOptimizer.setProgressCallback([=](double progress) {
        m_progressBar->setValue(static_cast<int>(100.0 * progress));
    });
}

int LayoutOptimizationDialog::exec()
{
    m_progressBar->setValue(0);

    return QDialog::exec();
}

void LayoutOptimizationDialog::finishOptimization()
{
    m_progressBar->setValue(100);

    m_mindMapData.setAspectRatio(m_aspectRatioSpinBox->value());
    m_mindMapData.setMinEdgeLength(m_minEdgeLengthSpinBox->value());

    QTimer::singleShot(500, this, &QDialog::accept);
}

void LayoutOptimizationDialog::initWidgets(const MindMapData & mindMapData)
{
    const auto mainLayout = new QVBoxLayout(this);

    const auto mainGroup = WidgetFactory::buildGroupBoxWithVLayout(tr("Parameters"), *mainLayout);

    const auto aspectRatioLayout = new QHBoxLayout;
    const auto imageWidthLabel = new QLabel(tr("Aspect Ratio:"));
    aspectRatioLayout->addWidget(imageWidthLabel);
    m_aspectRatioSpinBox = new QDoubleSpinBox;
    m_aspectRatioSpinBox->setMinimum(Constants::LayoutOptimizer::MIN_ASPECT_RATIO);
    m_aspectRatioSpinBox->setMaximum(Constants::LayoutOptimizer::MAX_ASPECT_RATIO);
    m_aspectRatioSpinBox->setSingleStep(0.1);
    m_aspectRatioSpinBox->setValue(mindMapData.aspectRatio());
    aspectRatioLayout->addWidget(m_aspectRatioSpinBox);
    mainGroup.second->addLayout(aspectRatioLayout);

    const auto minEdgeLengthLayout = new QHBoxLayout;
    const auto minEdgeLengthLabel = new QLabel(tr("Minimum Edge Length:"));
    minEdgeLengthLayout->addWidget(minEdgeLengthLabel);
    m_minEdgeLengthSpinBox = new QDoubleSpinBox;
    m_minEdgeLengthSpinBox->setMinimum(Constants::LayoutOptimizer::MIN_EDGE_LENGTH);
    m_minEdgeLengthSpinBox->setMaximum(Constants::LayoutOptimizer::MAX_EDGE_LENGTH);
    m_minEdgeLengthSpinBox->setSingleStep(1);
    m_minEdgeLengthSpinBox->setValue(mindMapData.minEdgeLength());
    minEdgeLengthLayout->addWidget(m_minEdgeLengthSpinBox);
    mainGroup.second->addLayout(minEdgeLengthLayout);

    const auto progressBarLayout = new QHBoxLayout;
    m_progressBar = new QProgressBar;
    m_progressBar->setEnabled(false);
    m_progressBar->setMaximum(100);
    m_progressBar->setValue(0);
    progressBarLayout->addWidget(m_progressBar);
    mainLayout->addLayout(progressBarLayout);

    const auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    connect(buttonBox, &QDialogButtonBox::accepted, this, [=] {
        buttonBox->setEnabled(false);
        QTimer::singleShot(0, this, [=] { // Trick to make disabled buttons apply immediately
            emit undoPointRequested();
            if (m_layoutOptimizer.initialize(m_aspectRatioSpinBox->value(), m_minEdgeLengthSpinBox->value())) {
                if (const auto optimizationInfo = m_layoutOptimizer.optimize(); optimizationInfo.changes) {
                    const double gain = (optimizationInfo.finalCost - optimizationInfo.initialCost) / optimizationInfo.initialCost;
                    juzzlin::L().info() << "Final cost: " << optimizationInfo.finalCost << " (" << gain * 100 << "%)";
                } else {
                    juzzlin::L().info() << "No changes";
                }
                m_layoutOptimizer.extract();
            }
            finishOptimization();
        });
    });

    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);
}

} // namespace Dialogs
