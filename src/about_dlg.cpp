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

#include "about_dlg.hpp"
#include "constants.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QVBoxLayout>

AboutDlg::AboutDlg(QWidget * parent)
  : QDialog(parent)
{
    setWindowTitle(tr("About ") + Constants::Application::APPLICATION_NAME);
    initWidgets();
}

void AboutDlg::initWidgets()
{
    const auto vLayout = new QVBoxLayout(this);
    const auto pixmapLabel = new QLabel(this);

    pixmapLabel->setPixmap(QPixmap(":/about.png").scaled(512, 512));
    vLayout->addWidget(pixmapLabel);

    const auto infoLabel = new QLabel(this);
    infoLabel->setText(
      QString("<h2>") + Constants::Application::APPLICATION_NAME + " v" + Constants::Application::APPLICATION_VERSION + "</h2>"
      + "<p>" + Constants::Application::APPLICATION_NAME + tr(" is licenced under ") + "GNU GPLv3."
      + " " + Constants::Application::COPYRIGHT + ".</p>"
      + "<p>" + tr("Package type: ") + Constants::Application::APPLICATION_PACKAGE_TYPE + "</p>"
      + tr("Project website: ") + "<a href='" + Constants::Application::WEB_SITE_URL + "'>"
      + Constants::Application::WEB_SITE_URL + "</a>"
      + "<p>" + tr("Support ") + Constants::Application::APPLICATION_NAME + tr(" on Patreon: ")
      + "<a href='" + Constants::Application::SUPPORT_SITE_URL + "'>"
      + Constants::Application::SUPPORT_SITE_URL + "</a></p>");

    vLayout->addWidget(infoLabel);

    const auto buttonLayout = new QHBoxLayout();
    const auto button = new QPushButton("&Ok", this);

    connect(button, &QPushButton::clicked, this, &AboutDlg::accept);

    buttonLayout->addWidget(button);
    buttonLayout->insertStretch(0);

    vLayout->addLayout(buttonLayout);
}
