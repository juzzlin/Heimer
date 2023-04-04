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

#include "special_node_style_dialog.hpp"

#include <QDialogButtonBox>
#include <QGroupBox>
#include <QRadioButton>
#include <QVBoxLayout>

namespace Dialogs {

SpecialNodeStyleDialog::SpecialNodeStyleDialog(QWidget * parent)
  : QDialog(parent)
  , m_dynamicRadioButton(new QRadioButton(tr("Dynamic"), this))
  , m_fixedWidthRadioButton(new QRadioButton(tr("Fixed Width"), this))
  , m_widthAlignmentGroupBox(new QGroupBox(tr("Width and Text Alignment"), this))
  , m_buttonBox(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this))
{
    setWindowTitle(tr("Set special node style"));

    setMinimumWidth(480);

    initWidgets();
}

void SpecialNodeStyleDialog::initWidgets()
{
    const auto groupLayout = new QVBoxLayout(m_widthAlignmentGroupBox);
    groupLayout->addWidget(m_dynamicRadioButton);
    groupLayout->addWidget(m_fixedWidthRadioButton);
    m_widthAlignmentGroupBox->setLayout(groupLayout);

    const auto layout = new QVBoxLayout(this);
    layout->addWidget(m_widthAlignmentGroupBox);
    layout->addWidget(m_buttonBox);
    setLayout(layout);

    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

} // namespace Dialogs
