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

#include "../../domain/mind_map_data.hpp"
#include "../../common/constants.hpp"
#include "../../domain/layout_optimizer.hpp"
#include "../editor_view.hpp"
#include "widget_factory.hpp"

#include "simple_logger.hpp"

#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

namespace Dialogs {

LayoutOptimizationDialog::LayoutOptimizationDialog(QWidget & parent, MindMapDataR mindMapData, LayoutOptimizer & layoutOptimizer, EditorView & editorView)
  : QDialog(&parent)
  , m_mindMapData(mindMapData)
  , m_layoutOptimizer(layoutOptimizer)
  , m_editorView(editorView)
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

void LayoutOptimizationDialog::initWidgets(MindMapDataCR mindMapData)
{
    const auto mainLayout = new QVBoxLayout(this);
    const auto mainGroup = WidgetFactory::buildGroupBoxWithVLayout(tr("Parameters"), *mainLayout);
    const auto parameterWidgetLayout = new QGridLayout;
    mainGroup.second->addLayout(parameterWidgetLayout);

    const auto imageWidthLabel = new QLabel(tr("Aspect Ratio:"));
    imageWidthLabel->setToolTip(tr("Aspect ratio = layout width / layout height"));
    parameterWidgetLayout->addWidget(imageWidthLabel, 0, 0);
    m_aspectRatioSpinBox = new QDoubleSpinBox;
    m_aspectRatioSpinBox->setMinimum(Constants::LayoutOptimizer::MIN_ASPECT_RATIO);
    m_aspectRatioSpinBox->setMaximum(Constants::LayoutOptimizer::MAX_ASPECT_RATIO);
    m_aspectRatioSpinBox->setSingleStep(0.1);
    m_aspectRatioSpinBox->setValue(mindMapData.aspectRatio());
    parameterWidgetLayout->addWidget(m_aspectRatioSpinBox, 0, 1);

    const auto leftArrowLabelStr = tr("<--");
    parameterWidgetLayout->addWidget(new QLabel(leftArrowLabelStr), 0, 2);

    const auto calculateARFromLayout = new QPushButton(tr("From layout"));
    calculateARFromLayout->setToolTip(tr("Calculate aspect ratio from current layout"));
    connect(calculateARFromLayout, &QPushButton::clicked, this, [=] {
        m_aspectRatioSpinBox->setValue(m_mindMapData.stats().layoutAspectRatio.value_or(m_aspectRatioSpinBox->value()));
    });
    parameterWidgetLayout->addWidget(calculateARFromLayout, 0, 3);

    const auto calculateARFromView = new QPushButton(tr("From view"));
    calculateARFromView->setToolTip(tr("Calculate aspect ratio from current view geometry"));
    connect(calculateARFromView, &QPushButton::clicked, this, [=] {
        const auto geometry = m_editorView.viewport()->geometry();
        m_aspectRatioSpinBox->setValue(static_cast<double>(geometry.width()) / geometry.height());
    });
    parameterWidgetLayout->addWidget(calculateARFromView, 0, 4);

    const auto minEdgeLengthLabel = new QLabel(tr("Minimum Edge Length:"));
    minEdgeLengthLabel->setToolTip(tr("Minimum edge length in the optimized layout"));
    parameterWidgetLayout->addWidget(minEdgeLengthLabel, 1, 0);
    m_minEdgeLengthSpinBox = new QDoubleSpinBox;
    m_minEdgeLengthSpinBox->setMinimum(Constants::LayoutOptimizer::MIN_EDGE_LENGTH);
    m_minEdgeLengthSpinBox->setMaximum(Constants::LayoutOptimizer::MAX_EDGE_LENGTH);
    m_minEdgeLengthSpinBox->setSingleStep(1);
    m_minEdgeLengthSpinBox->setValue(mindMapData.minEdgeLength());
    parameterWidgetLayout->addWidget(m_minEdgeLengthSpinBox, 1, 1);

    parameterWidgetLayout->addWidget(new QLabel(leftArrowLabelStr), 1, 2);

    const auto calculateMinELFromLayout = new QPushButton(tr("Min from layout"));
    connect(calculateMinELFromLayout, &QPushButton::clicked, this, [=] {
        m_minEdgeLengthSpinBox->setValue(m_mindMapData.stats().minimumEdgeLength.value_or(m_minEdgeLengthSpinBox->value()));
    });
    calculateMinELFromLayout->setToolTip(tr("Calculate minimum edge length from current layout"));
    parameterWidgetLayout->addWidget(calculateMinELFromLayout, 1, 3);

    const auto calculateMaxELFromLayout = new QPushButton(tr("Max from layout"));
    connect(calculateMaxELFromLayout, &QPushButton::clicked, this, [=] {
        m_minEdgeLengthSpinBox->setValue(m_mindMapData.stats().maximumEdgeLength.value_or(m_minEdgeLengthSpinBox->value()));
    });
    calculateMaxELFromLayout->setToolTip(tr("Calculate maximum edge length from current layout"));
    parameterWidgetLayout->addWidget(calculateMaxELFromLayout, 1, 4);

    const auto calculateAvgELFromLayout = new QPushButton(tr("Avg from layout"));
    connect(calculateAvgELFromLayout, &QPushButton::clicked, this, [=] {
        m_minEdgeLengthSpinBox->setValue(m_mindMapData.stats().averageEdgeLength.value_or(m_minEdgeLengthSpinBox->value()));
    });
    calculateAvgELFromLayout->setToolTip(tr("Calculate average edge length from current layout"));
    parameterWidgetLayout->addWidget(calculateAvgELFromLayout, 1, 5);

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
