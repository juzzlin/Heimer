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

#ifndef THEME_EDITOR_DIALOG_HPP
#define THEME_EDITOR_DIALOG_HPP

#include "../../domain/theme.hpp"

#include <QDialog>

class QDoubleSpinBox;
class QLabel;
class QSpinBox;

namespace Widgets {
class ColorButton;
class FontButton;
} // namespace Widgets

namespace Dialogs {

//! Edits the visual properties of a theme. The name is not edited.
class ThemeEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ThemeEditorDialog(const Theme & theme, QWidget * parent = nullptr);

    Theme theme() const;

signals:
    //! Emitted when the Apply button is clicked. The dialog stays open.
    void applied(const Theme & theme);

private:
    void initWidgets();

    void setTheme(const Theme & theme);

    void updatePreview();

    Widgets::ColorButton * addColorButton(QString text, bool alphaChannel = false);

    QSpinBox * addSpinBox(int minimum, int maximum);

    QDoubleSpinBox * addDoubleSpinBox(double minimum, double maximum, double step);

    QString m_name;

    //! Not taken from the font button, because its font is a resolved widget font
    QFont m_font;

    QLabel * m_preview;

    Widgets::ColorButton * m_backgroundColorButton;

    Widgets::ColorButton * m_gridColorButton;

    Widgets::ColorButton * m_nodeColorButton;

    Widgets::ColorButton * m_nodeTextColorButton;

    QSpinBox * m_cornerRadiusSpinBox;

    Widgets::FontButton * m_fontButton;

    QSpinBox * m_textSizeSpinBox;

    Widgets::ColorButton * m_edgeColorButton;

    QDoubleSpinBox * m_edgeWidthSpinBox;

    QDoubleSpinBox * m_arrowSizeSpinBox;

    Widgets::ColorButton * m_edgeLabelColorButton;

    Widgets::ColorButton * m_edgeLabelTextColorButton;

    QSpinBox * m_shadowOffsetSpinBox;

    QSpinBox * m_shadowBlurRadiusSpinBox;

    Widgets::ColorButton * m_shadowColorButton;

    QSpinBox * m_selectedItemShadowBlurRadiusSpinBox;

    Widgets::ColorButton * m_selectedItemShadowColorButton;
};

} // namespace Dialogs

#endif // THEME_EDITOR_DIALOG_HPP
