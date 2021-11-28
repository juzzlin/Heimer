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

#ifndef TOOL_BAR_HPP
#define TOOL_BAR_HPP

#include <QToolBar>
#include <QObject>

class QDoubleSpinBox;
class QWidgetAction;

class ToolBar : public QToolBar
{
    Q_OBJECT

public:
    ToolBar(QWidget * parent = nullptr);

    void enableWidgetSignals(bool enable);

    void setEdgeWidth(double value);

signals:

    void edgeWidthChanged(double width);

private:
    QWidgetAction * createEdgeWidthAction();

    QDoubleSpinBox * m_edgeWidthSpinBox = nullptr;
};

#endif // TOOL_BAR_HPP
