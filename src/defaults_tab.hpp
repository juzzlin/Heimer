// This file is part of Heimer.
// Copyright (C) 2021 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef DEFAULTS_TAB_HPP
#define DEFAULTS_TAB_HPP

#include "settings_proxy.hpp"
#include <QWidget>
#include <map>

class QCheckBox;
class QRadioButton;

class DefaultsTab : public QWidget
{
    Q_OBJECT

public:
    explicit DefaultsTab(QWidget * parent = nullptr);

    void apply();

private:

    void initWidgets();

    void setActiveDefaults();

    std::map<Edge::ArrowMode, QRadioButton *> m_edgeArrowStyleRadioMap;

    QCheckBox * m_edgeDirectionCheckBox = nullptr;
};

#endif // DEFAULTS_TAB_HPP
