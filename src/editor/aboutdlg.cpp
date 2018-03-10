// This file is part of Dementia.
// Copyright (C) 2018 Jussi Lind <jussi.lind@iki.fi>
//
// Dementia is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dementia is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dementia. If not, see <http://www.gnu.org/licenses/>.

#include "aboutdlg.hpp"
#include "config.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QVBoxLayout>

AboutDlg::AboutDlg(QWidget * parent)
: QDialog(parent)
{
    setWindowTitle(tr("About ") + Config::APPLICATION_NAME);
    initWidgets();
}

void AboutDlg::initWidgets()
{
    QVBoxLayout * vLayout = new QVBoxLayout(this);
    QLabel * pixmapLabel = new QLabel(this);

    pixmapLabel->setPixmap(QPixmap(":/about.png").scaled(512, 512));
    vLayout->addWidget(pixmapLabel);

    QLabel * infoLabel = new QLabel(this);
    infoLabel->setText(
        QString("<h2>") + Config::APPLICATION_NAME + " v" + Config::APPLICATION_VERSION + "</h2>"
        + "<p>" + Config::APPLICATION_NAME + " is licenced under GNU GPLv3.</p>"
        + "<p>" + Config::COPYRIGHT + "</p>"
        + "<a href='" + Config::WEB_SITE_URL + "'>"
        + Config::WEB_SITE_URL + "</a>");

    vLayout->addWidget(infoLabel);

    QHBoxLayout * buttonLayout = new QHBoxLayout();
    QPushButton * button = new QPushButton("&Ok", this);

    connect(button, SIGNAL(clicked()), this, SLOT(accept()));

    buttonLayout->addWidget(button);
    buttonLayout->insertStretch(0);

    vLayout->addLayout(buttonLayout);
}
