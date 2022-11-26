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

#include "widget_factory.hpp"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QObject>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QWidgetAction>

namespace WidgetFactory {

std::pair<QWidget *, QWidgetAction *> buildToolBarWidgetAction(QWidget & widget, QWidget & parent)
{
    const auto action = new QWidgetAction(&parent);
    action->setDefaultWidget(&widget);
    return { &widget, action };
}

std::pair<QWidget *, QWidgetAction *> buildToolBarWidgetActionWithLabel(QString labelText, QWidget & widget, QWidget & parent)
{
    const auto dummyWidget = new QWidget(&parent);
    const auto layout = new QHBoxLayout(dummyWidget);
    dummyWidget->setLayout(layout);
    layout->addWidget(new QLabel(labelText));
    layout->addWidget(&widget);
    layout->setContentsMargins(0, 0, 0, 0);
    const auto action = new QWidgetAction(&parent);
    action->setDefaultWidget(dummyWidget);
    return { dummyWidget, action };
}

} // namespace WidgetFactory
