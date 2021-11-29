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
#include "settings.hpp"
#include "simple_logger.hpp"
#include "widget_factory.hpp"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QFont>
#include <QFontDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QWidgetAction>

ToolBar::ToolBar(QWidget * parent)
  : QToolBar(parent)
  , m_copyOnDragCheckBox(new QCheckBox(tr("Copy on drag"), this))
  , m_cornerRadiusSpinBox(new QSpinBox(this))
  , m_edgeWidthSpinBox(new QDoubleSpinBox(this))
  , m_fontButton(new QPushButton(this))
  , m_gridSizeSpinBox(new QSpinBox(this))
  , m_searchLineEdit(new QLineEdit(this))
  , m_showGridCheckBox(new QCheckBox(tr("Show grid"), this))
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

    addAction(createGridSizeAction());

    const auto spacer = new QWidget;
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    addWidget(spacer);
    addAction(createSearchAction());
    addSeparator();
    addWidget(m_showGridCheckBox);
    addWidget(m_copyOnDragCheckBox);

    connect(m_showGridCheckBox, &QCheckBox::stateChanged, this, &ToolBar::gridVisibleChanged);
    connect(m_showGridCheckBox, &QCheckBox::stateChanged, Settings::saveGridVisibleState);
}

void ToolBar::changeFont(const QFont & font)
{
    updateFontButtonFont(font);
}

bool ToolBar::copyOnDragEnabled() const
{
    return m_copyOnDragCheckBox->isChecked();
}

QWidgetAction * ToolBar::createCornerRadiusAction()
{
    m_cornerRadiusSpinBox->setMinimum(0);
    m_cornerRadiusSpinBox->setMaximum(Constants::Node::MAX_CORNER_RADIUS);
    m_cornerRadiusSpinBox->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    const auto signal = QOverload<int>::of(&QSpinBox::valueChanged);
#else
    const auto signal = static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged);
#endif
    connect(m_cornerRadiusSpinBox, signal, this, &ToolBar::cornerRadiusChanged);

    return WidgetFactory::buildToolBarWidgetActionWithLabel(tr("Corner radius:"), *m_cornerRadiusSpinBox, *this).second;
}

QWidgetAction * ToolBar::createEdgeWidthAction()
{
    m_edgeWidthSpinBox->setSingleStep(Constants::Edge::STEP);
    m_edgeWidthSpinBox->setMinimum(Constants::Edge::MIN_SIZE);
    m_edgeWidthSpinBox->setMaximum(Constants::Edge::MAX_SIZE);
    m_edgeWidthSpinBox->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    const auto signal = QOverload<double>::of(&QDoubleSpinBox::valueChanged);
#else
    const auto signal = static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged);
#endif
    connect(m_edgeWidthSpinBox, signal, this, &ToolBar::edgeWidthChanged);

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

QWidgetAction * ToolBar::createGridSizeAction()
{
    m_gridSizeSpinBox->setMinimum(Constants::Grid::MIN_SIZE);
    m_gridSizeSpinBox->setMaximum(Constants::Grid::MAX_SIZE);
    m_gridSizeSpinBox->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    const auto signal = QOverload<int>::of(&QSpinBox::valueChanged);
#else
    const auto signal = static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged);
#endif
    connect(m_gridSizeSpinBox, signal, this, &ToolBar::gridSizeChanged);
    connect(m_gridSizeSpinBox, signal, Settings::saveGridSize);

    return WidgetFactory::buildToolBarWidgetActionWithLabel(tr("Grid size:"), *m_gridSizeSpinBox, *this).second;
}

QWidgetAction * ToolBar::createSearchAction()
{
    m_searchTimer.setSingleShot(true);
    connect(&m_searchTimer, &QTimer::timeout, [this, searchLineEdit = m_searchLineEdit]() {
        const auto text = searchLineEdit->text();
        juzzlin::L().debug() << "Search text changed: " << text.toStdString();
        emit searchTextChanged(text);
    });
    connect(m_searchLineEdit, &QLineEdit::textChanged, [searchTimer = &m_searchTimer](const QString & text) {
        if (text.isEmpty()) {
            searchTimer->start(0);
        } else {
            searchTimer->start(Constants::View::TEXT_SEARCH_DELAY_MS);
        }
    });
    connect(m_searchLineEdit, &QLineEdit::returnPressed, [searchTimer = &m_searchTimer] {
        searchTimer->start(0);
    });

    return WidgetFactory::buildToolBarWidgetActionWithLabel(tr("Search:"), *m_searchLineEdit, *this).second;
}

QWidgetAction * ToolBar::createTextSizeAction()
{
    m_textSizeSpinBox->setMinimum(Constants::Text::MIN_SIZE);
    m_textSizeSpinBox->setMaximum(Constants::Text::MAX_SIZE);
    m_textSizeSpinBox->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    const auto signal = QOverload<int>::of(&QSpinBox::valueChanged);
#else
    const auto signal = static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged);
#endif
    connect(m_textSizeSpinBox, signal, this, &ToolBar::textSizeChanged);

    return WidgetFactory::buildToolBarWidgetActionWithLabel(tr("Text size:"), *m_textSizeSpinBox, *this).second;
}

void ToolBar::enableWidgetSignals(bool enable)
{
    m_cornerRadiusSpinBox->blockSignals(!enable);
    m_edgeWidthSpinBox->blockSignals(!enable);
    m_gridSizeSpinBox->blockSignals(!enable);
    m_textSizeSpinBox->blockSignals(!enable);
}

void ToolBar::loadSettings()
{
    m_showGridCheckBox->setCheckState(Settings::loadGridVisibleState());

    m_gridSizeSpinBox->setValue(Settings::loadGridSize());
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
