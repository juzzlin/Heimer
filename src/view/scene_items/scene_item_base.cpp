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

#include "scene_item_base.hpp"

namespace SceneItems {

SceneItemBase::SceneItemBase()
  : m_opacityAnimation(this, "opacity")
  , m_scaleAnimation(this, "scale")
{
}

void SceneItemBase::appearWithAnimation()
{
    m_scaleAnimation.setDuration(m_animationDuration);
    m_scaleAnimation.setStartValue(0.0);
    m_scaleAnimation.setEndValue(1.0);
    m_scaleAnimation.stop();
    m_scaleAnimation.start();

    m_opacityAnimation.setDuration(m_animationDuration);
    m_opacityAnimation.setStartValue(0.0);
    m_opacityAnimation.setEndValue(m_animationOpacity);
    m_opacityAnimation.stop();
    m_opacityAnimation.start();
}

void SceneItemBase::disappearWithAnimation()
{
    m_scaleAnimation.setDuration(m_animationDuration);
    m_scaleAnimation.setStartValue(scale());
    m_scaleAnimation.setEndValue(0.0);
    m_scaleAnimation.stop();
    m_scaleAnimation.start();

    m_opacityAnimation.setDuration(m_animationDuration);
    m_opacityAnimation.setStartValue(opacity());
    m_opacityAnimation.setEndValue(0.0);
    m_opacityAnimation.stop();
    m_opacityAnimation.start();
}

void SceneItemBase::removeFromScene()
{
}

void SceneItemBase::removeFromSceneWithAnimation()
{
    connect(&m_scaleAnimation, &QPropertyAnimation::finished, this, &SceneItemBase::removeFromScene);

    m_scaleAnimation.setDuration(m_animationDuration);
    m_scaleAnimation.setStartValue(scale());
    m_scaleAnimation.setEndValue(0.0);
    m_scaleAnimation.stop();
    m_scaleAnimation.start();
}

void SceneItemBase::raiseWithAnimation(double targetScale)
{
    m_targetScale = targetScale;
    m_scaleAnimation.setDuration(m_animationDuration);
    m_scaleAnimation.setStartValue(scale());
    m_scaleAnimation.setEndValue(targetScale);
    m_scaleAnimation.stop();
    m_scaleAnimation.start();
}

void SceneItemBase::lowerWithAnimation()
{
    m_targetScale = 1.0;
    m_scaleAnimation.setDuration(m_animationDuration);
    m_scaleAnimation.setStartValue(scale());
    m_scaleAnimation.setEndValue(m_targetScale);
    m_scaleAnimation.stop();
    m_scaleAnimation.start();
}

void SceneItemBase::enableShadowEffect(bool)
{
}

void SceneItemBase::setAnimationDuration(int durationMs)
{
    m_animationDuration = durationMs;
}

void SceneItemBase::setAnimationOpacity(qreal animationOpacity)
{
    m_animationOpacity = animationOpacity;
}

qreal SceneItemBase::targetScale() const
{
    return m_targetScale;
}

SceneItemBase::~SceneItemBase() = default;

} // namespace SceneItems
