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

#ifndef SETTINGS_TAB_BASE_HPP
#define SETTINGS_TAB_BASE_HPP

#include <QWidget>

#include "../../common/types.hpp"

class SettingsProxy;

namespace Dialogs {

class SettingsTabBase : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsTabBase(QString name, QWidget * parent = nullptr);

    virtual void apply();

    virtual void reject();

    QString name() const;

protected:
    SettingsProxyS settingsProxy() const;

private:
    QString m_name;
};

} // namespace Dialogs

#endif // SETTINGS_TAB_BASE_HPP
