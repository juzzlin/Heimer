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

#include "png_export_dialog.hpp"
#include "constants.hpp"

#include <QCheckBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QSpinBox>
#include <QStandardPaths>
#include <QTimer>
#include <QVBoxLayout>

PngExportDialog::PngExportDialog(QWidget & parent)
  : QDialog(&parent)
{
    setWindowTitle(tr("Export to PNG Image"));
    setMinimumWidth(480);
    initWidgets();

    connect(m_filenameButton, &QPushButton::clicked, [=]() {
        const auto filename = QFileDialog::getSaveFileName(this,
                                                           tr("Export As"),
                                                           QStandardPaths::writableLocation(QStandardPaths::HomeLocation),
                                                           tr("PNG Files") + " (*" + Constants::Export::Png::FILE_EXTENSION + ")");

        m_filenameLineEdit->setText(filename);
    });

    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::close);

    connect(m_exportButton, &QPushButton::clicked, [=]() {
        m_exportButton->setEnabled(false);
        m_progressBar->setValue(50);
        emit pngExportRequested(m_filenameWithExtension, QSize(m_imageWidthSpinBox->value(), m_imageHeightSpinBox->value()), m_transparentBackgroundCheckBox->isChecked());
    });

    // The ugly cast is needed because there are QSpinBox::valueChanged(int) and QSpinBox::valueChanged(QString)
    // In Qt > 5.10 one can use QOverload<int>::of(...)
    connect(m_imageWidthSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [&](int value) {
        if (m_enableSpinBoxConnection) {
            m_enableSpinBoxConnection = false;
            m_imageHeightSpinBox->setValue(value / m_aspectRatio);
            m_enableSpinBoxConnection = true;
        }
    });

    connect(m_imageHeightSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [&](int value) {
        if (m_enableSpinBoxConnection) {
            m_enableSpinBoxConnection = false;
            m_imageWidthSpinBox->setValue(value * m_aspectRatio);
            m_enableSpinBoxConnection = true;
        }
    });

    connect(m_filenameLineEdit, &QLineEdit::textChanged, this, &PngExportDialog::validate);

    connect(m_imageWidthSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &PngExportDialog::validate);

    connect(m_imageHeightSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &PngExportDialog::validate);
}

void PngExportDialog::setImageSize(QSize size)
{
    m_enableSpinBoxConnection = false;

    // Set a bit bigger size as default. With the original
    // size the result would be a bit blurry.
    size *= 2;

    m_imageWidthSpinBox->setValue(size.width());
    m_imageHeightSpinBox->setValue(size.height());

    m_aspectRatio = float(size.width()) / size.height();

    m_enableSpinBoxConnection = true;
}

int PngExportDialog::exec()
{
    m_progressBar->setValue(0);

    validate();

    return QDialog::exec();
}

void PngExportDialog::finishExport(bool success)
{
    if (success) {
        m_progressBar->setValue(100);
        QTimer::singleShot(500, this, &QDialog::accept);
    } else {
        QMessageBox::critical(this, Constants::Application::APPLICATION_NAME, tr("Couldn't write to") + " '" + m_filenameLineEdit->text() + "'", QMessageBox::Ok);
    }
}

void PngExportDialog::validate()
{
    m_progressBar->setValue(0);

    m_exportButton->setEnabled(
      m_imageHeightSpinBox->value() > Constants::Export::Png::MIN_IMAGE_SIZE && // Intentionally open intervals
      m_imageHeightSpinBox->value() < Constants::Export::Png::MAX_IMAGE_SIZE && m_imageWidthSpinBox->value() > Constants::Export::Png::MIN_IMAGE_SIZE && m_imageWidthSpinBox->value() < Constants::Export::Png::MAX_IMAGE_SIZE && !m_filenameLineEdit->text().isEmpty());

    m_filenameWithExtension = m_filenameLineEdit->text();

    if (m_filenameWithExtension.isEmpty()) {
        return;
    }

    if (!m_filenameWithExtension.toLower().endsWith(Constants::Export::Png::FILE_EXTENSION)) {
        m_filenameWithExtension += Constants::Export::Png::FILE_EXTENSION;
    }
}

void PngExportDialog::initWidgets()
{
    const auto mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(new QLabel("<b>" + tr("Filename") + "</b>"));

    const auto filenameLayout = new QHBoxLayout;
    m_filenameLineEdit = new QLineEdit;
    filenameLayout->addWidget(m_filenameLineEdit);
    m_filenameButton = new QPushButton;
    m_filenameButton->setText(tr("Export as.."));
    filenameLayout->addWidget(m_filenameButton);
    mainLayout->addLayout(filenameLayout);

    mainLayout->addWidget(new QLabel("<b>" + tr("Image Size") + "</b>"));
    const auto imageSizeLayout = new QHBoxLayout;
    const auto imageWidthLabel = new QLabel(tr("Width (px):"));
    imageSizeLayout->addWidget(imageWidthLabel);
    m_imageWidthSpinBox = new QSpinBox;
    m_imageWidthSpinBox->setMinimum(Constants::Export::Png::MIN_IMAGE_SIZE);
    m_imageWidthSpinBox->setMaximum(Constants::Export::Png::MAX_IMAGE_SIZE);
    imageSizeLayout->addWidget(m_imageWidthSpinBox);
    const auto imageHeightLabel = new QLabel(tr("Height (px):"));
    imageSizeLayout->addWidget(imageHeightLabel);
    m_imageHeightSpinBox = new QSpinBox;
    m_imageHeightSpinBox->setMinimum(Constants::Export::Png::MIN_IMAGE_SIZE);
    m_imageHeightSpinBox->setMaximum(Constants::Export::Png::MAX_IMAGE_SIZE);
    imageSizeLayout->addWidget(m_imageHeightSpinBox);
    mainLayout->addLayout(imageSizeLayout);

    mainLayout->addWidget(new QLabel("<b>" + tr("Background") + "</b>"));
    const auto backgroundLayout = new QHBoxLayout;
    m_transparentBackgroundCheckBox = new QCheckBox;
    m_transparentBackgroundCheckBox->setText(tr("Transparent background"));
    backgroundLayout->addWidget(m_transparentBackgroundCheckBox);
    mainLayout->addLayout(backgroundLayout);

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
    m_exportButton = new QPushButton;
    m_exportButton->setText(tr("Export"));
    m_exportButton->setEnabled(false);
    m_exportButton->setDefault(true);
    buttonLayout->addWidget(m_exportButton);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}
