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

#include "whats_new_dialog.hpp"

#include "../../common/constants.hpp"

#include <QDialogButtonBox>
#include <QFile>
#include <QPixmap>
#include <QTextEdit>
#include <QVBoxLayout>

namespace Dialogs {

WhatsNewDialog::WhatsNewDialog(QWidget * parent)
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
        if (line.contains(Constants::Application::applicationVersion())) {
            versionFound = true;
        }
        if (versionFound) {
            filteredLines.append(line + "\n");
        }
    }
    return filteredLines;
}

void WhatsNewDialog::initWidgets()
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

    const auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);

    vLayout->addWidget(buttonBox);
}

} // namespace Dialogs
