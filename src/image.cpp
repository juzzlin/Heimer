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

#include "image.hpp"

Image::Image()
{
}

Image::Image(QImage image, std::string path)
  : m_image(image)
  , m_path(path)
{
}

QImage Image::image() const
{
    return m_image;
}

std::string Image::path() const
{
    return m_path;
}

size_t Image::id() const
{
    return m_id;
}

void Image::setId(size_t id)
{
    m_id = id;
}
