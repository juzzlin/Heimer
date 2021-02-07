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

#include "whats_new_dlg.hpp"
#include "constants.hpp"

#include <QFile>
#include <QHBoxLayout>
#include <QPixmap>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

WhatsNewDlg::WhatsNewDlg(QWidget * parent)
  : QDialog(parent)
{
    setWindowTitle(tr("What's New"));
    initWidgets();
}

QString filterChangeLog(QString data)
{
    const auto lines = data.split("\n");
    bool versionFound = false;
    QString filteredLines;
    for (auto && line : lines) {
        if (line.contains(VERSION)) {
            versionFound = true;
        }
        if (versionFound) {
            filteredLines.append(line + "\n");
        }
    }
    return filteredLines;
}

void WhatsNewDlg::initWidgets()
{
    const auto vLayout = new QVBoxLayout(this);
    const auto textEdit = new QTextEdit(this);

    QString data;
    QFile file(":/CHANGELOG");
    if (file.open(QIODevice::ReadOnly)) {
        data = filterChangeLog(file.readAll());
    }
    file.close();

    textEdit->setText(data);
    textEdit->setReadOnly(true);
    QFont font = textEdit->font();
    font.setFamily("Courier New");
    font.setStyleHint(QFont::Monospace);
    textEdit->setFont(font);
    vLayout->addWidget(textEdit);

    const auto buttonLayout = new QHBoxLayout();
    const auto button = new QPushButton("&Ok", this);

    connect(button, &QPushButton::clicked, this, &WhatsNewDlg::accept);

    buttonLayout->addWidget(button);
    buttonLayout->insertStretch(0);

    vLayout->addLayout(buttonLayout);
}
