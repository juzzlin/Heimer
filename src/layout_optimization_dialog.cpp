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
#include "constants.hpp"
#include "layout_optimizer.hpp"

#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QSettings>
#include <QTimer>
#include <QVBoxLayout>

LayoutOptimizationDialog::LayoutOptimizationDialog(QWidget & parent, LayoutOptimizer & layoutOptimizer)
  : QDialog(&parent)
  , m_layoutOptimizer(layoutOptimizer)
{
    setWindowTitle(tr("Optimize Layout"));
    setMinimumWidth(480);
    initWidgets();

    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    connect(m_optimizeButton, &QPushButton::clicked, [=] {
        emit undoPointRequested();
        m_layoutOptimizer.initialize(m_aspectRatioSpinBox->value(), m_minEdgeLengthSpinBox->value());
        m_layoutOptimizer.optimize();
        m_layoutOptimizer.extract();
        finishOptimization();
    });

    connect(&m_layoutOptimizer, &LayoutOptimizer::progress, [=](double progress) {
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

    QSettings settings;
    settings.beginGroup(Constants::LayoutOptimizer::QSETTINGS_GROUP);
    settings.setValue(Constants::LayoutOptimizer::QSETTINGS_ASPECT_RATIO_KEY, m_aspectRatioSpinBox->value());
    settings.setValue(Constants::LayoutOptimizer::QSETTINGS_MIN_EDGE_LENGTH_KEY, m_minEdgeLengthSpinBox->value());
    settings.endGroup();

    QTimer::singleShot(500, this, &QDialog::accept);
}

void LayoutOptimizationDialog::initWidgets()
{
    const auto mainLayout = new QVBoxLayout(this);

    QSettings settings;
    settings.beginGroup(Constants::LayoutOptimizer::QSETTINGS_GROUP);

    const auto aspectRatioLayout = new QHBoxLayout;
    const auto imageWidthLabel = new QLabel(tr("Aspect Ratio:"));
    aspectRatioLayout->addWidget(imageWidthLabel);
    m_aspectRatioSpinBox = new QDoubleSpinBox;
    m_aspectRatioSpinBox->setMinimum(Constants::LayoutOptimizer::MIN_ASPECT_RATIO);
    m_aspectRatioSpinBox->setMaximum(Constants::LayoutOptimizer::MAX_ASPECT_RATIO);
    m_aspectRatioSpinBox->setSingleStep(0.1);
    m_aspectRatioSpinBox->setValue(settings.value(Constants::LayoutOptimizer::QSETTINGS_ASPECT_RATIO_KEY, Constants::LayoutOptimizer::DEFAULT_ASPECT_RATIO).toReal());
    aspectRatioLayout->addWidget(m_aspectRatioSpinBox);
    mainLayout->addLayout(aspectRatioLayout);

    const auto minEdgeLengthLayout = new QHBoxLayout;
    const auto minEdgeLengthLabel = new QLabel(tr("Minimum Edge Length:"));
    minEdgeLengthLayout->addWidget(minEdgeLengthLabel);
    m_minEdgeLengthSpinBox = new QDoubleSpinBox;
    m_minEdgeLengthSpinBox->setMinimum(Constants::LayoutOptimizer::MIN_EDGE_LENGTH);
    m_minEdgeLengthSpinBox->setMaximum(Constants::LayoutOptimizer::MAX_EDGE_LENGTH);
    m_minEdgeLengthSpinBox->setSingleStep(1);
    m_minEdgeLengthSpinBox->setValue(settings.value(Constants::LayoutOptimizer::QSETTINGS_MIN_EDGE_LENGTH_KEY, Constants::LayoutOptimizer::DEFAULT_MIN_EDGE_LENGTH).toReal());
    minEdgeLengthLayout->addWidget(m_minEdgeLengthSpinBox);
    mainLayout->addLayout(minEdgeLengthLayout);

    const auto progressBarLayout = new QHBoxLayout;
    m_progressBar = new QProgressBar;
    m_progressBar->setEnabled(false);
    m_progressBar->setMaximum(100);
    m_progressBar->setValue(0);
    progressBarLayout->addWidget(m_progressBar);
    mainLayout->addLayout(progressBarLayout);

    const auto buttonLayout = new QHBoxLayout;
    m_cancelButton = new QPushButton;
    m_cancelButton->setText(tr("Cancel"));
    buttonLayout->addWidget(m_cancelButton);
    m_optimizeButton = new QPushButton;
    m_optimizeButton->setText(tr("Optimize"));
    buttonLayout->addWidget(m_optimizeButton);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);

    settings.endGroup();
}
