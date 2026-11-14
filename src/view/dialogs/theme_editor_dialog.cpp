// This file is part of Heimer.
// Copyright (C) 2026 Jussi Lind <jussi.lind@iki.fi>
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

#include "theme_editor_dialog.hpp"

#include "../../common/constants.hpp"
#include "../theme_preview.hpp"
#include "../widgets/color_button.hpp"
#include "../widgets/font_button.hpp"

#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

namespace Dialogs {

static const QSize previewSize { 320, 140 };

ThemeEditorDialog::ThemeEditorDialog(const Theme & theme, QWidget * parent)
  : QDialog(parent)
  , m_name(theme.name)
  , m_preview(new QLabel { this })
  , m_backgroundColorButton(addColorButton(tr("Background")))
  , m_gridColorButton(addColorButton(tr("Grid")))
  , m_nodeColorButton(addColorButton(tr("Node")))
  , m_nodeTextColorButton(addColorButton(tr("Node text")))
  , m_cornerRadiusSpinBox(addSpinBox(0, 20))
  , m_fontButton(new Widgets::FontButton { this })
  , m_textSizeSpinBox(addSpinBox(Constants::View::minTextSize(), Constants::View::maxTextSize()))
  , m_edgeColorButton(addColorButton(tr("Edge")))
  , m_edgeWidthSpinBox(addDoubleSpinBox(Constants::Edge::minEdgeWidth(), Constants::Edge::maxEdgeWidth(), Constants::Edge::edgeWidthStep()))
  , m_arrowSizeSpinBox(addDoubleSpinBox(Constants::Edge::minArrowSize(), Constants::Edge::maxArrowSize(), Constants::Edge::arrowSizeStep()))
  , m_edgeLabelColorButton(addColorButton(tr("Label")))
  , m_edgeLabelTextColorButton(addColorButton(tr("Label text")))
  , m_shadowOffsetSpinBox(addSpinBox(0, 10))
  , m_shadowBlurRadiusSpinBox(addSpinBox(0, 100))
  , m_shadowColorButton(addColorButton(tr("Shadow"), true))
  , m_selectedItemShadowBlurRadiusSpinBox(addSpinBox(0, 100))
  , m_selectedItemShadowColorButton(addColorButton(tr("Selected item shadow"), true))
{
    setWindowTitle(tr("Edit Theme") + " - " + theme.name);

    initWidgets();

    setTheme(theme);

    connect(m_fontButton, &Widgets::FontButton::defaultFontSizeRequested, this, [this] {
        m_fontButton->setDefaultPointSize(m_textSizeSpinBox->value());
    });
    connect(m_fontButton, &Widgets::FontButton::fontSizeChanged, m_textSizeSpinBox, &QSpinBox::setValue);
    connect(m_fontButton, &Widgets::FontButton::fontChanged, this, [this](const QFont & font) {
        m_font = font;
        updatePreview();
    });
}

Widgets::ColorButton * ThemeEditorDialog::addColorButton(QString text, bool alphaChannel)
{
    const auto button = new Widgets::ColorButton { text, alphaChannel, this };
    connect(button, &Widgets::ColorButton::colorChanged, this, &ThemeEditorDialog::updatePreview);
    return button;
}

QSpinBox * ThemeEditorDialog::addSpinBox(int minimum, int maximum)
{
    const auto spinBox = new QSpinBox { this };
    spinBox->setRange(minimum, maximum);
    connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ThemeEditorDialog::updatePreview);
    return spinBox;
}

QDoubleSpinBox * ThemeEditorDialog::addDoubleSpinBox(double minimum, double maximum, double step)
{
    const auto spinBox = new QDoubleSpinBox { this };
    spinBox->setRange(minimum, maximum);
    spinBox->setSingleStep(step);
    connect(spinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ThemeEditorDialog::updatePreview);
    return spinBox;
}

void ThemeEditorDialog::initWidgets()
{
    const auto buildGroup = [this](QString title) {
        const auto group = new QGroupBox { title, this };
        const auto layout = new QFormLayout;
        group->setLayout(layout);
        return std::make_pair(group, layout);
    };

    const auto [canvasGroup, canvasLayout] = buildGroup(tr("Canvas"));
    canvasLayout->addRow(m_backgroundColorButton);
    canvasLayout->addRow(m_gridColorButton);

    const auto [nodeGroup, nodeLayout] = buildGroup(tr("Nodes"));
    nodeLayout->addRow(m_nodeColorButton);
    nodeLayout->addRow(m_nodeTextColorButton);
    nodeLayout->addRow(tr("Corner radius:"), m_cornerRadiusSpinBox);

    const auto [textGroup, textLayout] = buildGroup(tr("Text"));
    textLayout->addRow(m_fontButton);
    textLayout->addRow(tr("Text size:"), m_textSizeSpinBox);

    const auto [edgeGroup, edgeLayout] = buildGroup(tr("Edges"));
    edgeLayout->addRow(m_edgeColorButton);
    edgeLayout->addRow(tr("Edge width:"), m_edgeWidthSpinBox);
    edgeLayout->addRow(tr("Arrow size:"), m_arrowSizeSpinBox);
    edgeLayout->addRow(m_edgeLabelColorButton);
    edgeLayout->addRow(m_edgeLabelTextColorButton);

    const auto [shadowGroup, shadowLayout] = buildGroup(tr("Shadows"));
    shadowLayout->addRow(m_shadowColorButton);
    shadowLayout->addRow(tr("Offset:"), m_shadowOffsetSpinBox);
    shadowLayout->addRow(tr("Blur radius:"), m_shadowBlurRadiusSpinBox);
    shadowLayout->addRow(m_selectedItemShadowColorButton);
    shadowLayout->addRow(tr("Selected item blur radius:"), m_selectedItemShadowBlurRadiusSpinBox);

    const auto groupLayout = new QGridLayout;
    groupLayout->addWidget(canvasGroup, 0, 0);
    groupLayout->addWidget(nodeGroup, 0, 1);
    groupLayout->addWidget(textGroup, 1, 0);
    groupLayout->addWidget(edgeGroup, 1, 1);
    groupLayout->addWidget(shadowGroup, 2, 0, 1, 2);

    m_preview->setAlignment(Qt::AlignCenter);
    m_preview->setMinimumSize(previewSize);

    const auto buttonBox = new QDialogButtonBox { QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel | QDialogButtonBox::Reset, this };
    buttonBox->button(QDialogButtonBox::Reset)->setText(tr("Reset to Classic"));
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(buttonBox->button(QDialogButtonBox::Reset), &QPushButton::clicked, this, [this] {
        setTheme({});
    });
    connect(buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, [this] {
        emit applied(theme());
    });

    const auto mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_preview);
    mainLayout->addLayout(groupLayout);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
}

void ThemeEditorDialog::setTheme(const Theme & theme)
{
    m_backgroundColorButton->setColor(theme.backgroundColor);
    m_gridColorButton->setColor(theme.gridColor);
    m_nodeColorButton->setColor(theme.nodeColor);
    m_nodeTextColorButton->setColor(theme.nodeTextColor);
    m_cornerRadiusSpinBox->setValue(theme.cornerRadius);
    m_font = theme.font;
    m_fontButton->setFont(theme.font);
    m_textSizeSpinBox->setValue(theme.textSize);
    m_edgeColorButton->setColor(theme.edgeColor);
    m_edgeWidthSpinBox->setValue(theme.edgeWidth);
    m_arrowSizeSpinBox->setValue(theme.arrowSize);
    m_edgeLabelColorButton->setColor(theme.edgeLabelColor);
    m_edgeLabelTextColorButton->setColor(theme.edgeLabelTextColor);
    m_shadowOffsetSpinBox->setValue(theme.shadowEffect.offset());
    m_shadowBlurRadiusSpinBox->setValue(theme.shadowEffect.blurRadius());
    m_shadowColorButton->setColor(theme.shadowEffect.shadowColor());
    m_selectedItemShadowBlurRadiusSpinBox->setValue(theme.shadowEffect.selectedItemBlurRadius());
    m_selectedItemShadowColorButton->setColor(theme.shadowEffect.selectedItemShadowColor());

    updatePreview();
}

Theme ThemeEditorDialog::theme() const
{
    Theme theme;
    theme.name = m_name;
    theme.backgroundColor = m_backgroundColorButton->color();
    theme.gridColor = m_gridColorButton->color();
    theme.nodeColor = m_nodeColorButton->color();
    theme.nodeTextColor = m_nodeTextColorButton->color();
    theme.cornerRadius = m_cornerRadiusSpinBox->value();
    theme.font = m_font;
    theme.textSize = m_textSizeSpinBox->value();
    theme.edgeColor = m_edgeColorButton->color();
    theme.edgeWidth = m_edgeWidthSpinBox->value();
    theme.arrowSize = m_arrowSizeSpinBox->value();
    theme.edgeLabelColor = m_edgeLabelColorButton->color();
    theme.edgeLabelTextColor = m_edgeLabelTextColorButton->color();
    theme.shadowEffect = {
        m_shadowOffsetSpinBox->value(),
        m_shadowBlurRadiusSpinBox->value(),
        m_selectedItemShadowBlurRadiusSpinBox->value(),
        m_shadowColorButton->color(),
        m_selectedItemShadowColorButton->color()
    };
    return theme;
}

void ThemeEditorDialog::updatePreview()
{
    m_preview->setPixmap(ThemePreview::render(theme(), previewSize));
}

} // namespace Dialogs
