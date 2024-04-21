// This file is part of Heimer.
// Copyright (C) 2024 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef SCENE_ITEM_BASE_GRAPHICS_TEXT_ITEM_HPP
#define SCENE_ITEM_BASE_GRAPHICS_TEXT_ITEM_HPP

#include <QGraphicsTextItem>
#include <QObject>
#include <QPropertyAnimation>

namespace SceneItems {

class SceneItemBaseGraphicsTextItem : public QGraphicsTextItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
    Q_PROPERTY(qreal scale READ scale WRITE setScale)
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
    SceneItemBaseGraphicsTextItem(QGraphicsItem * parentItem);

    virtual ~SceneItemBaseGraphicsTextItem() override;

    virtual void appearWithAnimation();

    virtual void disappearWithAnimation();

    virtual void enableShadowEffect(bool enable);

    virtual void removeFromScene();

    virtual void removeFromSceneWithAnimation();

    virtual void raiseWithAnimation(double targetScale);

    virtual void lowerWithAnimation();

    void setAnimationDuration(int durationMs);

    void setAnimationOpacity(qreal newAnimationOpacity);

    qreal targetScale() const;

private:
    int m_animationDuration = 75;

    qreal m_animationOpacity = 1.0;

    qreal m_targetScale = 1.0;

    QPropertyAnimation m_opacityAnimation;

    QPropertyAnimation m_scaleAnimation;
};

} // namespace SceneItems

#endif // SCENE_ITEM_BASE_GRAPHICS_TEXT_ITEM_HPP
