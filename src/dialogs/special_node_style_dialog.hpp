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

#ifndef SPECIAL_NODE_STYLE_DIALOG_HPP
#define SPECIAL_NODE_STYLE_DIALOG_HPP

#include <QDialog>

class QDialogButtonBox;
class QGroupBox;
class QRadioButton;

namespace Dialogs {

class SpecialNodeStyleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SpecialNodeStyleDialog(QWidget * parent = nullptr);

private:
    QRadioButton * m_dynamicRadioButton;

    QRadioButton * m_fixedWidthRadioButton;

    QGroupBox * m_widthAlignmentGroupBox;

    QDialogButtonBox * m_buttonBox;

    void initWidgets();
};

} // namespace Dialogs

#endif // SPECIAL_NODE_STYLE_DIALOG_HPP
