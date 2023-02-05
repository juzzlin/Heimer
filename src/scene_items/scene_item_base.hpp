// This file is part of Heimer.
// Copyright (C) 2023 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef SCENE_ITEM_BASE_HPP
#define SCENE_ITEM_BASE_HPP

#include <QGraphicsItem>
#include <QObject>
#include <QPropertyAnimation>

namespace SceneItems {

class SceneItemBase : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
    Q_PROPERTY(qreal scale READ scale WRITE setScale)

public:
    SceneItemBase();

    virtual ~SceneItemBase();

    virtual void hideWithAnimation();

    virtual void removeFromScene();

private:
    QPropertyAnimation m_scaleAnimation;
};

} // namespace SceneItems

#endif // SCENE_ITEM_BASE_HPP
