// This file is part of Heimer.
// Copyright (C) 2018 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef TEXT_EDIT_HPP
#define TEXT_EDIT_HPP

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsTextItem>

namespace SceneItems {

class TextEdit : public QGraphicsTextItem
{
    Q_OBJECT

public:
    explicit TextEdit(QGraphicsItem * parentItem);

    QColor backgroundColor() const;

    virtual void setBackgroundColor(const QColor & backgroundColor);

    virtual void setTextSize(int textSize);

    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget) override;

    QString text() const;

    void setText(const QString & text);

    virtual ~TextEdit() override;

signals:

    void textChanged(QString text);

    void undoPointRequested();

protected:
    virtual bool event(QEvent * event) override;

    virtual void keyPressEvent(QKeyEvent * event) override;

    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event) override;

    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent * event) override;

private:
    double m_maxHeight = 0;

    double m_maxWidth = 0;

    QColor m_backgroundColor = QColor(192, 192, 192, 64);

    QString m_text;

    int m_textSize = 0;
};

} // namespace SceneItems

#endif // TEXT_EDIT_HPP
