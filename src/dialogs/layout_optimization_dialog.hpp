// This file is part of Heimer.
// Copyright (C) 2019 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef LAYOUT_OPTIMIZATION_DIALOG_HPP
#define LAYOUT_OPTIMIZATION_DIALOG_HPP

#include <QDialog>

#include "../types.hpp"

class EditorView;
class LayoutOptimizer;
class QDoubleSpinBox;
class QProgressBar;

namespace Dialogs {

class LayoutOptimizationDialog : public QDialog
{
    Q_OBJECT

public:
    //! Constructor.
    explicit LayoutOptimizationDialog(QWidget & parent, MindMapDataR mindMapData, LayoutOptimizer & layoutOptimizer, EditorView & editorView);

    int exec() override;

signals:

    void undoPointRequested();

private slots:

    void finishOptimization();

private:
    void initWidgets(MindMapDataCR mindMapData);

    MindMapDataR m_mindMapData;

    LayoutOptimizer & m_layoutOptimizer;

    EditorView & m_editorView;

    QDoubleSpinBox * m_aspectRatioSpinBox = nullptr;

    QDoubleSpinBox * m_minEdgeLengthSpinBox = nullptr;

    QProgressBar * m_progressBar = nullptr;
};

} // namespace Dialogs

#endif // LAYOUT_OPTIMIZATION_DIALOG_HPP
