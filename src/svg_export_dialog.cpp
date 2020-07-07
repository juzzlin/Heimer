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

#include "svg_export_dialog.hpp"
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

SvgExportDialog::SvgExportDialog(QWidget & parent)
  : QDialog(&parent)
{
    setWindowTitle(tr("Export to an SVG File"));
    setMinimumWidth(480);
    initWidgets();

    connect(m_filenameButton, &QPushButton::clicked, [=]() {
        const auto filename = QFileDialog::getSaveFileName(this,
                                                           tr("Export As"),
                                                           QStandardPaths::writableLocation(QStandardPaths::HomeLocation),
                                                           tr("SVG Files") + " (*" + Constants::Export::Svg::FILE_EXTENSION + ")");

        m_filenameLineEdit->setText(filename);
    });

    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::close);

    connect(m_exportButton, &QPushButton::clicked, [=]() {
        m_exportButton->setEnabled(false);
        m_progressBar->setValue(50);
        emit svgExportRequested(m_filenameWithExtension);
    });

    connect(m_filenameLineEdit, &QLineEdit::textChanged, this, &SvgExportDialog::validate);
}

int SvgExportDialog::exec()
{
    m_progressBar->setValue(0);

    validate();

    return QDialog::exec();
}

void SvgExportDialog::finishExport(bool success)
{
    if (success) {
        m_progressBar->setValue(100);
        QTimer::singleShot(500, this, &QDialog::accept);
    } else {
        QMessageBox::critical(this, Constants::Application::APPLICATION_NAME, tr("Couldn't write to") + " '" + m_filenameLineEdit->text() + "'", QMessageBox::Ok);
    }
}

void SvgExportDialog::validate()
{
    m_progressBar->setValue(0);

    m_exportButton->setEnabled(!m_filenameLineEdit->text().isEmpty());

    m_filenameWithExtension = m_filenameLineEdit->text();

    if (m_filenameWithExtension.isEmpty()) {
        return;
    }

    if (!m_filenameWithExtension.toLower().endsWith(Constants::Export::Svg::FILE_EXTENSION)) {
        m_filenameWithExtension += Constants::Export::Svg::FILE_EXTENSION;
    }
}

void SvgExportDialog::initWidgets()
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
