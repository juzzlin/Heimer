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
#include "simple_logger.hpp"
#include "widget_factory.hpp"

#include <QDoubleSpinBox>
#include <QFont>
#include <QFontDialog>
#include <QPushButton>
#include <QSpinBox>
#include <QWidgetAction>

ToolBar::ToolBar(QWidget * parent)
  : QToolBar(parent)
  , m_cornerRadiusSpinBox(new QSpinBox(this))
  , m_edgeWidthSpinBox(new QDoubleSpinBox(this))
  , m_fontButton(new QPushButton(this))
  , m_textSizeSpinBox(new QSpinBox(this))
{
    addAction(createEdgeWidthAction());

    addSeparator();

    addAction(createTextSizeAction());

    addSeparator();

    addAction(createFontAction());

    addSeparator();

    addAction(createCornerRadiusAction());

    addSeparator();
}

void ToolBar::changeFont(const QFont & font)
{
    updateFontButtonFont(font);
}

QWidgetAction * ToolBar::createCornerRadiusAction()
{
    m_cornerRadiusSpinBox->setMinimum(0);
    m_cornerRadiusSpinBox->setMaximum(Constants::Node::MAX_CORNER_RADIUS);
    m_cornerRadiusSpinBox->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    connect(m_cornerRadiusSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ToolBar::cornerRadiusChanged);
#else
    connect(m_cornerRadiusSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &ToolBar::cornerRadiusChanged);
#endif
    return WidgetFactory::buildToolBarWidgetActionWithLabel(tr("Corner radius:"), *m_cornerRadiusSpinBox, *this).second;
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

QWidgetAction * ToolBar::createFontAction()
{
    m_fontButton->setText(tr("Font") + Constants::Misc::THREE_DOTS);
    connect(m_fontButton, &QPushButton::clicked, [=] {
        bool ok;
        QFont defaultFont = m_fontButton->font();
        defaultFont.setPointSize(m_textSizeSpinBox->value());
        const auto font = QFontDialog::getFont(&ok, defaultFont, this);
        if (ok) {
            // Note: Support for multiple families implemented in Qt 5.13 =>
            juzzlin::L().debug() << "Font family selected: '" << font.family().toStdString() << "'";
            juzzlin::L().debug() << "Font weight selected: " << font.weight();
            updateFontButtonFont(font);
            m_textSizeSpinBox->setValue(font.pointSize());
            emit textSizeChanged(font.pointSize());
            emit fontChanged(font);
        }
    });
    return WidgetFactory::buildToolBarWidgetAction(*m_fontButton, *this).second;
}

QWidgetAction * ToolBar::createTextSizeAction()
{
    m_textSizeSpinBox->setMinimum(Constants::Text::MIN_SIZE);
    m_textSizeSpinBox->setMaximum(Constants::Text::MAX_SIZE);
    m_textSizeSpinBox->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    connect(m_textSizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ToolBar::textSizeChanged);
#else
    connect(m_textSizeSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &ToolBar::textSizeChanged);
#endif
    return WidgetFactory::buildToolBarWidgetActionWithLabel(tr("Text size:"), *m_textSizeSpinBox, *this).second;
}

void ToolBar::enableWidgetSignals(bool enable)
{
    m_cornerRadiusSpinBox->blockSignals(!enable);
    m_edgeWidthSpinBox->blockSignals(!enable);
    m_textSizeSpinBox->blockSignals(!enable);
}

void ToolBar::setCornerRadius(int value)
{
    if (m_cornerRadiusSpinBox->value() != value) {
        m_cornerRadiusSpinBox->setValue(value);
    }
}

void ToolBar::setEdgeWidth(double value)
{
    if (!qFuzzyCompare(m_edgeWidthSpinBox->value(), value)) {
        m_edgeWidthSpinBox->setValue(value);
    }
}

void ToolBar::setTextSize(int textSize)
{
    if (textSize <= 0) {
        return;
    }

    if (m_textSizeSpinBox->value() != textSize) {
        m_textSizeSpinBox->setValue(textSize);
    }
}

void ToolBar::updateFontButtonFont(const QFont & font)
{
    QFont newFont(font);
    newFont.setPointSize(m_fontButton->font().pointSize());
    m_fontButton->setFont(newFont);
}
