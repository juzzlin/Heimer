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

#include <QObject>
#include <QTimer>
#include <QToolBar>

class QCheckBox;
class QDoubleSpinBox;
class QFont;
class QLineEdit;
class QPushButton;
class QSpinBox;
class QWidgetAction;

namespace Widgets {
class FontButton;
}

namespace Menus {

class ToolBar : public QToolBar
{
    Q_OBJECT

public:
    ToolBar();

    bool autoSnapEnabled() const;

    void changeFont(const QFont & font);

    bool copyOnDragEnabled() const;

    void enableWidgetSignals(bool enable);

    void loadSettings();

    void requestCurrentSearchText();

    void setArrowSize(double value);

    void setCornerRadius(int value);

    void setEdgeWidth(double value);

    void setTextSize(int textSize);

signals:
    void arrowSizeChanged(double width);

    void edgeWidthChanged(double width);

    void cornerRadiusChanged(int size);

    void fontChanged(const QFont & font);

    void gridSizeChanged(int size, bool autoSnap);

    void gridVisibleChanged(int state);

    void searchTextChanged(QString text);

    void textSizeChanged(int value);

private:
    QWidgetAction * createCornerRadiusAction();

    QWidgetAction * createArrowSizeAction();

    QWidgetAction * createEdgeWidthAction();

    QWidgetAction * createFontAction();

    QWidgetAction * createGridSizeAction();

    QWidgetAction * createSearchAction();

    QWidgetAction * createTextSizeAction();

    QCheckBox * m_autoSnapCheckBox = nullptr;

    QCheckBox * m_copyOnDragCheckBox = nullptr;

    QSpinBox * m_cornerRadiusSpinBox = nullptr;

    QDoubleSpinBox * m_arrowSizeSpinBox = nullptr;

    QDoubleSpinBox * m_edgeWidthSpinBox = nullptr;

    Widgets::FontButton * m_fontButton = nullptr;

    QSpinBox * m_gridSizeSpinBox = nullptr;

    QLineEdit * m_searchLineEdit = nullptr;

    QTimer m_searchTimer;

    QCheckBox * m_showGridCheckBox = nullptr;

    QSpinBox * m_textSizeSpinBox = nullptr;
};

} // namespace Menus

#endif // TOOL_BAR_HPP
