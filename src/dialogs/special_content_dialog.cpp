// This file is part of Heimer.
// Copyright (C) 2022 Jussi Lind <jussi.lind@iki.fi>
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

#include "special_content_dialog.hpp"

#include "image_file_dialog.hpp"
#include "image_file_display.hpp"
#include "widget_factory.hpp"

#include "../mediator.hpp"

#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QUrl>
#include <QVBoxLayout>

namespace Dialogs {

bool isValidUrl(QString url)
{
    return !url.isEmpty() && QUrl(url).isValid();
}

SpecialContentDialog::SpecialContentDialog(QWidget * parent)
  : QDialog(parent)
  , m_urlEdit(new QLineEdit(this))
{
    setWindowTitle(tr("Add special content"));
    setMinimumWidth(640);

    initWidgets();
}

SpecialContent::SpecialContentModel SpecialContentDialog::specialContentModel() const
{   
    return m_specialContentModel;
}

void SpecialContentDialog::initWidgets()
{
    const auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, [=]() {
        if (isValidUrl(m_urlEdit->text())) {
            m_specialContentModel.url = m_urlEdit->text();
        }
        QDialog::accept();
    });
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    const auto mainLayout = new QVBoxLayout;
    initWebLinkWidgets(*mainLayout, *buttonBox);
    initImageWidgets(*mainLayout);

    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
}

void SpecialContentDialog::initWebLinkWidgets(QVBoxLayout & mainLayout, QDialogButtonBox & buttonBox)
{
    const auto webLinkGroup = WidgetFactory::buildGroupBoxWithVLayout(tr("Web link"), mainLayout);
    connect(m_urlEdit, &QLineEdit::textChanged, this, [&buttonBox](QString text) {
        buttonBox.button(QDialogButtonBox::Ok)->setEnabled(isValidUrl(text) || text.isEmpty());
    });
    webLinkGroup.second->addWidget(m_urlEdit);
}

void SpecialContentDialog::initImageWidgets(QVBoxLayout & mainLayout)
{
    const auto imageGroup = WidgetFactory::buildGroupBoxWithVLayout(tr("Image file"), mainLayout);
    const auto imageButton = new QPushButton(tr("Select image file"));
    imageGroup.second->addWidget(imageButton);
    const auto imageFileDisplay = new ImageFileDisplay;
    imageGroup.second->addWidget(imageFileDisplay);
    connect(imageButton, &QPushButton::clicked, this, [=] {
        ImageFileDialog dialog(this);
        if (const auto image = dialog.loadImage(); image.has_value()) {
            imageFileDisplay->setImage(image.value());
            m_specialContentModel.image = image;
        }
    });
}

} // namespace Dialogs
