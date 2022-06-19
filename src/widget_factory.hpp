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

#ifndef WIDGET_FACTORY_HPP
#define WIDGET_FACTORY_HPP

#include <QString>

#include <utility>

class QGroupBox;
class QHBoxLayout;
class QLayout;
class QPushButton;
class QVBoxLayout;
class QWidget;
class QWidgetAction;

namespace WidgetFactory {

std::pair<QGroupBox *, QVBoxLayout *> buildGroupBoxWithVLayout(QString title, QLayout & parentLayout);

std::pair<QPushButton *, QHBoxLayout *> buildResetToDefaultsButtonWithHLayout();

std::pair<QWidget *, QWidgetAction *> buildToolBarWidgetAction(QWidget & widget, QWidget & parent);

std::pair<QWidget *, QWidgetAction *> buildToolBarWidgetActionWithLabel(QString label, QWidget & widget, QWidget & parent);
} // namespace WidgetFactory

#endif // WIDGET_FACTORY_HPP
