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

#include "../../constants.hpp"
#include "../../export_params.hpp"

#include "../widget_factory.hpp"

#include "utils.hpp"

#include <QCheckBox>
#include <QDialogButtonBox>
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

namespace Dialogs::Export {

SvgExportDialog::SvgExportDialog(QWidget & parent)
  : QDialog(&parent)
{
    setWindowTitle(tr("Export to an SVG File"));
    setMinimumWidth(480);
    initWidgets();

    connect(m_fileNameButton, &QPushButton::clicked, this, [=] {
        if (const auto fileName = QFileDialog::getSaveFileName(this,
                                                               tr("Export As"),
                                                               QStandardPaths::writableLocation(QStandardPaths::HomeLocation),
                                                               tr("SVG Files") + " (*" + Constants::Export::Svg::FILE_EXTENSION + ")");
            !fileName.isEmpty()) {
            m_fileNameLineEdit->setText(fileName);
        }
    });

    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, [=] {
        m_buttonBox->setEnabled(false);
        m_progressBar->setValue(50);
        emit svgExportRequested({ m_fileNameWithExtension });
    });

    connect(m_fileNameLineEdit, &QLineEdit::textChanged, this, &SvgExportDialog::validate);
}

void SvgExportDialog::setCurrentMindMapFileName(QString fileName)
{
    if (!fileName.isEmpty()) {
        m_fileNameLineEdit->setText(Utils::exportFileName(fileName, Constants::Export::Svg::FILE_EXTENSION));
    }
}

int SvgExportDialog::exec()
{
    m_progressBar->setValue(0);
    m_buttonBox->setEnabled(true);

    validate();

    return QDialog::exec();
}

void SvgExportDialog::finishExport(bool success)
{
    if (success) {
        m_progressBar->setValue(100);
        QTimer::singleShot(500, this, &QDialog::accept);
    } else {
        QMessageBox::critical(this, Constants::Application::APPLICATION_NAME, tr("Couldn't write to") + " '" + m_fileNameLineEdit->text() + "'", QMessageBox::Ok);
    }
}

void SvgExportDialog::validate()
{
    m_progressBar->setValue(0);

    m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!m_fileNameLineEdit->text().isEmpty());

    m_fileNameWithExtension = m_fileNameLineEdit->text();

    if (m_fileNameWithExtension.isEmpty()) {
        return;
    }

    if (!m_fileNameWithExtension.toLower().endsWith(Constants::Export::Svg::FILE_EXTENSION)) {
        m_fileNameWithExtension += Constants::Export::Svg::FILE_EXTENSION;
    }
}

void SvgExportDialog::initWidgets()
{
    const auto mainLayout = new QVBoxLayout(this);

    const auto filenameGroup = WidgetFactory::buildGroupBoxWithVLayout(tr("Filename"), *mainLayout);

    const auto filenameLayout = new QHBoxLayout;
    m_fileNameLineEdit = new QLineEdit;
    filenameLayout->addWidget(m_fileNameLineEdit);
    m_fileNameButton = new QPushButton;
    m_fileNameButton->setText(tr("Export as.."));
    filenameLayout->addWidget(m_fileNameButton);
    filenameGroup.second->addLayout(filenameLayout);

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

} // namespace Dialogs::Export
