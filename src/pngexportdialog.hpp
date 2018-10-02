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

#ifndef PNGEXPORTDIALOG_HPP
#define PNGEXPORTDIALOG_HPP

#include <QDialog>

class QCheckBox;
class QLineEdit;
class QProgressBar;
class QPushButton;
class QSpinBox;

class PngExportDialog : public QDialog
{
    Q_OBJECT

public:

    //! Constructor.
    explicit PngExportDialog(QWidget & parent);

    void setImageSize(QSize size);

    int exec() override;

public slots:

    void finishExport(bool success);

signals:

    void pngExportRequested(QString filename, QSize size, bool transparentBackground);

private slots:

    void validate();

private:

    void initWidgets();

    QLineEdit * m_filenameLineEdit = nullptr;

    QSpinBox * m_imageHeightSpinBox = nullptr;

    QSpinBox * m_imageWidthSpinBox = nullptr;

    QPushButton * m_filenameButton = nullptr;

    QPushButton * m_cancelButton = nullptr;

    QPushButton * m_exportButton = nullptr;

    QProgressBar * m_progressBar = nullptr;

    QCheckBox * m_transparentBackgroundCheckBox = nullptr;

    QString m_filenameWithExtension;

    bool m_enableSpinBoxConnection = true;

    float m_aspectRatio = 1.0;
};

#endif // PNGEEXPORTDIALOG_HPP
