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

#include "../constants.hpp"

namespace SceneItems {

SceneItemBase::SceneItemBase()
  : m_scaleAnimation(this, "scale")
{
    m_scaleAnimation.setDuration(Constants::Node::SCALE_ANIMATION_DURATION);
    m_scaleAnimation.setEndValue(0.0);

    connect(&m_scaleAnimation, &QPropertyAnimation::finished, this, &SceneItemBase::removeFromScene);
}

void SceneItemBase::hideWithAnimation()
{
    m_scaleAnimation.setStartValue(scale());
    m_scaleAnimation.start();
}

void SceneItemBase::removeFromScene()
{
}

SceneItemBase::~SceneItemBase() = default;

} // namespace SceneItems
