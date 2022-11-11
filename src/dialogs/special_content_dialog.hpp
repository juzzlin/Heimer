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

#ifndef SPECIAL_CONTENT_DIALOG_HPP
#define SPECIAL_CONTENT_DIALOG_HPP

#include <QDialog>

#include <memory>

#include "../special_content/special_content_model.hpp"

class QDialogButtonBox;
class QLineEdit;
class QVBoxLayout;

namespace Dialogs {

class SpecialContentDialog : public QDialog
{
    Q_OBJECT

public:
    SpecialContentDialog(QWidget * parent);

    SpecialContent::SpecialContentModel specialContentModel() const;

private:
    void initWidgets();

    void initWebLinkWidgets(QVBoxLayout & mainLayout, QDialogButtonBox & buttonBox);

    void initImageWidgets(QVBoxLayout & mainLayout);

    QLineEdit * m_urlEdit;

    SpecialContent::SpecialContentModel m_specialContentModel;
};

} // namespace Dialogs

#endif // SPECIAL_CONTENT_DIALOG_HPP
