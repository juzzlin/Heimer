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

#include "../../common/constants.hpp"
#include "../../infra/settings.hpp"
#include "../widgets/font_button.hpp"
#include "widget_factory.hpp"

#include "simple_logger.hpp"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QFont>
#include <QFontDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QWidgetAction>

namespace Menus {

ToolBar::ToolBar(QWidget * parent)
  : QToolBar(parent)
  , m_autoSnapCheckBox(new QCheckBox(tr("Auto snap"), this))
  , m_copyOnDragCheckBox(new QCheckBox(tr("Copy on drag"), this))
  , m_cornerRadiusSpinBox(new QSpinBox(this))
  , m_arrowSizeSpinBox(new QDoubleSpinBox(this))
  , m_edgeWidthSpinBox(new QDoubleSpinBox(this))
  , m_fontButton(new Widgets::FontButton(this))
  , m_gridSizeSpinBox(new QSpinBox(this))
  , m_searchLineEdit(new QLineEdit(this))
  , m_showGridCheckBox(new QCheckBox(tr("Show grid"), this))
  , m_textSizeSpinBox(new QSpinBox(this))
{
    addAction(createArrowSizeAction());

    addSeparator();

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
    connect(m_showGridCheckBox, &QCheckBox::stateChanged, this, &ToolBar::gridVisibleChanged);
    connect(m_showGridCheckBox, &QCheckBox::stateChanged, Settings::Custom::saveGridVisibleState);
    addSeparator();

    addWidget(m_autoSnapCheckBox);
    connect(m_autoSnapCheckBox, &QCheckBox::stateChanged, Settings::Custom::saveAutoSnapState);
    addSeparator();

    addWidget(m_copyOnDragCheckBox);
    m_copyOnDragCheckBox->setToolTip(tr("Copy style from parent node on drag"));

    m_autoSnapCheckBox->setToolTip(tr("Automatically snap existing nodes to the grid when grid size changes"));

    m_searchLineEdit->setClearButtonEnabled(true);
}

bool ToolBar::autoSnapEnabled() const
{
    return m_autoSnapCheckBox->isChecked();
}

void ToolBar::changeFont(const QFont & font)
{
    m_fontButton->setFont(font);
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

QWidgetAction * ToolBar::createArrowSizeAction()
{
    m_arrowSizeSpinBox->setSingleStep(Constants::Edge::ARROW_SIZE_STEP);
    m_arrowSizeSpinBox->setMinimum(Constants::Edge::MIN_ARROW_SIZE);
    m_arrowSizeSpinBox->setMaximum(Constants::Edge::MAX_ARROW_SIZE);
    m_arrowSizeSpinBox->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    const auto signal = QOverload<double>::of(&QDoubleSpinBox::valueChanged);
#else
    const auto signal = static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged);
#endif
    connect(m_arrowSizeSpinBox, signal, this, &ToolBar::arrowSizeChanged);

    return WidgetFactory::buildToolBarWidgetActionWithLabel(tr("Arrow size:"), *m_arrowSizeSpinBox, *this).second;
}

QWidgetAction * ToolBar::createEdgeWidthAction()
{
    m_edgeWidthSpinBox->setSingleStep(Constants::Edge::EDGE_WIDTH_STEP);
    m_edgeWidthSpinBox->setMinimum(Constants::Edge::MIN_EDGE_WIDTH);
    m_edgeWidthSpinBox->setMaximum(Constants::Edge::MAX_EDGE_WIDTH);
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
    connect(m_fontButton, &Widgets::FontButton::defaultFontSizeRequested, this, [=] {
        m_fontButton->setDefaultPointSize(m_textSizeSpinBox->value());
    });

    connect(m_fontButton, &Widgets::FontButton::fontSizeChanged, m_textSizeSpinBox, &QSpinBox::setValue);

    connect(m_fontButton, &Widgets::FontButton::fontChanged, this, &ToolBar::fontChanged);

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
    connect(m_gridSizeSpinBox, signal, this, [=](int size) {
        emit gridSizeChanged(size, autoSnapEnabled());
    });
    connect(m_gridSizeSpinBox, signal, Settings::Custom::saveGridSize);

    return WidgetFactory::buildToolBarWidgetActionWithLabel(tr("Grid size:"), *m_gridSizeSpinBox, *this).second;
}

QWidgetAction * ToolBar::createSearchAction()
{
    m_searchTimer.setSingleShot(true);
    connect(&m_searchTimer, &QTimer::timeout, this, [this, searchLineEdit = m_searchLineEdit]() {
        const auto text = searchLineEdit->text();
        juzzlin::L().debug() << "Search text changed: " << text.toStdString();
        emit searchTextChanged(text);
    });
    connect(m_searchLineEdit, &QLineEdit::textChanged, this, [searchTimer = &m_searchTimer](const QString & text) {
        if (text.isEmpty()) {
            searchTimer->start(0);
        } else {
            searchTimer->start(Constants::View::TEXT_SEARCH_DELAY_MS);
        }
    });
    connect(m_searchLineEdit, &QLineEdit::returnPressed, this, [searchTimer = &m_searchTimer] {
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
    m_arrowSizeSpinBox->blockSignals(!enable);
    m_cornerRadiusSpinBox->blockSignals(!enable);
    m_edgeWidthSpinBox->blockSignals(!enable);
    m_gridSizeSpinBox->blockSignals(!enable);
    m_textSizeSpinBox->blockSignals(!enable);
}

void ToolBar::loadSettings()
{
    m_autoSnapCheckBox->setCheckState(Settings::Custom::loadAutoSnapState());
    m_gridSizeSpinBox->setValue(Settings::Custom::loadGridSize());
    m_showGridCheckBox->setCheckState(Settings::Custom::loadGridVisibleState());
}

void ToolBar::requestCurrentSearchText()
{
    emit searchTextChanged(m_searchLineEdit->text());
}

void ToolBar::setArrowSize(double value)
{
    if (!qFuzzyCompare(m_arrowSizeSpinBox->value(), value)) {
        m_arrowSizeSpinBox->setValue(value);
    }
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

} // namespace Menus
