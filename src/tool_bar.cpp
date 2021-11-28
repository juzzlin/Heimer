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

#include "tool_bar.hpp"

#include "constants.hpp"
#include "widget_factory.hpp"

#include <QDoubleSpinBox>
#include <QWidgetAction>

ToolBar::ToolBar(QWidget * parent)
  : QToolBar(parent)
  , m_edgeWidthSpinBox(new QDoubleSpinBox(this))

{
    addAction(createEdgeWidthAction());
    addSeparator();
}

QWidgetAction * ToolBar::createEdgeWidthAction()
{
    m_edgeWidthSpinBox->setSingleStep(Constants::Edge::STEP);
    m_edgeWidthSpinBox->setMinimum(Constants::Edge::MIN_SIZE);
    m_edgeWidthSpinBox->setMaximum(Constants::Edge::MAX_SIZE);
    m_edgeWidthSpinBox->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    connect(m_edgeWidthSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ToolBar::edgeWidthChanged);
#else
    connect(m_edgeWidthSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &ToolBar::edgeWidthChanged);
#endif
    return WidgetFactory::buildToolBarWidgetActionWithLabel(tr("Edge width:"), *m_edgeWidthSpinBox, *this).second;
}

void ToolBar::enableWidgetSignals(bool enable)
{
    m_edgeWidthSpinBox->blockSignals(!enable);
}

void ToolBar::setEdgeWidth(double value)
{
    if (!qFuzzyCompare(m_edgeWidthSpinBox->value(), value)) {
        m_edgeWidthSpinBox->setValue(value);
    }
}
