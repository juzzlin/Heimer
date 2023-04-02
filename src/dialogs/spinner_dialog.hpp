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

#ifndef SPINNER_DIALOG_HPP
#define SPINNER_DIALOG_HPP

#include <QDialog>

class QLabel;

namespace Dialogs {

//! The spinner dialog.
class SpinnerDialog : public QDialog
{
    Q_OBJECT

public:
    //! Constructor.
    explicit SpinnerDialog(QWidget * parent = nullptr);

    void setMessage(QString message);

private:
    void initWidgets();

    QLabel * m_messageLabel;
};

} // namespace Dialogs

#endif // SPINNER_DIALOG_HPP
