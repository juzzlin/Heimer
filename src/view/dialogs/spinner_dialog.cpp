// This file is part of Heimer.
// Copyright (C) 2023 Jussi Lind <jussi.lind@iki.fi>
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

#include "spinner_dialog.hpp"

#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>

namespace Dialogs {

SpinnerDialog::SpinnerDialog(QWidget * parent)
  : QDialog(parent, Qt::Popup)
  , m_messageLabel(new QLabel(this))
{
    setMinimumWidth(400);
    initWidgets();
}

void SpinnerDialog::initWidgets()
{
    const auto vLayout = new QVBoxLayout(this);
    const auto spinner = new QProgressBar;
    spinner->setMinimum(0);
    spinner->setMaximum(0);
    vLayout->addWidget(m_messageLabel);
    vLayout->addWidget(spinner);
    setLayout(vLayout);
}

void SpinnerDialog::setMessage(QString message)
{
    m_messageLabel->setText(message);
    update();
}

} // namespace Dialogs
