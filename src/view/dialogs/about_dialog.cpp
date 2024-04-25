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

#include "about_dialog.hpp"
#include "../../common/constants.hpp"

#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QTextBrowser>
#include <QVBoxLayout>

namespace Dialogs {

const int logoSize = 256;

AboutDialog::AboutDialog(QWidget * parent)
  : QDialog(parent)
{
    setWindowTitle(tr("About ") + Constants::Application::applicationName());
    setMinimumWidth(3 * logoSize); // Note that there should be enough space for all languages
    initWidgets();
}

void AboutDialog::initWidgets()
{
    using namespace Constants::Application;

    const auto vLayout = new QVBoxLayout(this);
    const auto pixmapLabel = new QLabel(this);
    pixmapLabel->setPixmap(QPixmap(":/about.png").scaled(logoSize, logoSize));

    const auto hLayout = new QHBoxLayout;
    hLayout->addWidget(pixmapLabel);

    const auto infoLabel = new QTextBrowser(this);
    infoLabel->setOpenExternalLinks(true);
    infoLabel->setHtml(
      QString("<h2>") + applicationName() + " v" + applicationVersion() + "</h2>"
      + "<p>" + applicationName() + tr(" is licenced under ") + "GNU GPLv3" + ".</p>"
      + "<p>" + copyright() + ".</p>"
      + "<p>" + tr("Package type: ") + applicationPackageType() + "</p>"
      + tr("Project website: ") + "<a href='" + webSiteUrl() + "'>"
      + webSiteUrl() + "</a>");
    infoLabel->setFrameStyle(QFrame::NoFrame);
    infoLabel->viewport()->setAutoFillBackground(false);
    hLayout->addWidget(infoLabel);
    vLayout->addLayout(hLayout);

    const auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    vLayout->addWidget(buttonBox);
}

} // namespace Dialogs
