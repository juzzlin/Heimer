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

#include "../constants.hpp"

#include "widget_factory.hpp"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QGroupBox>
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

namespace Dialogs {

PngExportDialog::PngExportDialog(QWidget & parent)
  : QDialog(&parent)
{
    setWindowTitle(tr("Export to PNG Image"));
    setMinimumWidth(480);

    initWidgets();

    connect(m_filenameButton, &QPushButton::clicked, this, [=] {
        const auto filename = QFileDialog::getSaveFileName(this,
                                                           tr("Export As"),
                                                           QStandardPaths::writableLocation(QStandardPaths::HomeLocation),
                                                           tr("PNG Files") + " (*" + Constants::Export::Png::FILE_EXTENSION + ")");

        m_filenameLineEdit->setText(filename);
    });

#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    const auto spinBoxIntSignal = QOverload<int>::of(&QSpinBox::valueChanged);
#else
    const auto spinBoxIntSignal = static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged);
#endif

    connect(m_imageWidthSpinBox, spinBoxIntSignal, this, [&](int value) {
        if (m_enableSpinBoxConnection) {
            m_enableSpinBoxConnection = false;
            m_imageHeightSpinBox->setValue(static_cast<int>(static_cast<float>(value) / m_aspectRatio));
            m_enableSpinBoxConnection = true;
        }
    });

    connect(m_imageHeightSpinBox, spinBoxIntSignal, this, [&](int value) {
        if (m_enableSpinBoxConnection) {
            m_enableSpinBoxConnection = false;
            m_imageWidthSpinBox->setValue(static_cast<int>(static_cast<float>(value) * m_aspectRatio));
            m_enableSpinBoxConnection = true;
        }
    });

    connect(m_imageWidthSpinBox, spinBoxIntSignal, this, &PngExportDialog::validate);

    connect(m_imageHeightSpinBox, spinBoxIntSignal, this, &PngExportDialog::validate);

    connect(m_filenameLineEdit, &QLineEdit::textChanged, this, &PngExportDialog::validate);

    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, [=] {
        m_buttonBox->setEnabled(false);
        m_progressBar->setValue(50);
        emit pngExportRequested(m_filenameWithExtension, QSize(m_imageWidthSpinBox->value(), m_imageHeightSpinBox->value()), m_transparentBackgroundCheckBox->isChecked());
    });
}

void PngExportDialog::setImageSize(QSize size)
{
    m_enableSpinBoxConnection = false;

    // Set a bit bigger size as default. With the original
    // size the result would be a bit blurry.
    size *= 2;

    m_imageWidthSpinBox->setValue(size.width());
    m_imageHeightSpinBox->setValue(size.height());

    m_aspectRatio = static_cast<float>(size.width()) / static_cast<float>(size.height());

    m_enableSpinBoxConnection = true;
}

int PngExportDialog::exec()
{
    m_progressBar->setValue(0);
    m_buttonBox->setEnabled(true);

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

    m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled( //
      m_imageHeightSpinBox->value() > Constants::Export::Png::MIN_IMAGE_SIZE && // Intentionally open intervals
      m_imageHeightSpinBox->value() < Constants::Export::Png::MAX_IMAGE_SIZE && //
      m_imageWidthSpinBox->value() > Constants::Export::Png::MIN_IMAGE_SIZE && //
      m_imageWidthSpinBox->value() < Constants::Export::Png::MAX_IMAGE_SIZE && //
      !m_filenameLineEdit->text().isEmpty());

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

    const auto nameGroup = WidgetFactory::buildGroupBoxWithVLayout(tr("Filename"), *mainLayout);

    const auto filenameLayout = new QHBoxLayout;
    m_filenameLineEdit = new QLineEdit;
    filenameLayout->addWidget(m_filenameLineEdit);
    m_filenameButton = new QPushButton;
    m_filenameButton->setText(tr("Export as.."));
    filenameLayout->addWidget(m_filenameButton);
    nameGroup.second->addLayout(filenameLayout);

    const auto sizeGroup = WidgetFactory::buildGroupBoxWithVLayout(tr("Image Size"), *mainLayout);

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
    sizeGroup.second->addLayout(imageSizeLayout);

    const auto bgGroup = WidgetFactory::buildGroupBoxWithVLayout(tr("Background"), *mainLayout);

    const auto backgroundLayout = new QHBoxLayout;
    m_transparentBackgroundCheckBox = new QCheckBox;
    m_transparentBackgroundCheckBox->setText(tr("Transparent background"));
    backgroundLayout->addWidget(m_transparentBackgroundCheckBox);
    bgGroup.second->addLayout(backgroundLayout);

    const auto progressBarLayout = new QHBoxLayout;
    m_progressBar = new QProgressBar;
    m_progressBar->setEnabled(false);
    m_progressBar->setMaximum(100);
    m_progressBar->setValue(0);
    progressBarLayout->addWidget(m_progressBar);
    mainLayout->addLayout(progressBarLayout);

    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                       | QDialogButtonBox::Cancel);
    m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    mainLayout->addWidget(m_buttonBox);

    setLayout(mainLayout);
}

} // namespace Dialogs
