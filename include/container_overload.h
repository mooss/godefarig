//////////////////////////////////////////////////////////////////////////
// godefarig, (kind of) spherical terrain generator                     //
// Copyright (C) 2016-2017 mooss <mooss@protonmail.com>                 //
// This file is part of godefarig.                                      //
//                                                                      //
// godefarig is free software: you can redistribute it and/or modify    //
// it under the terms of the GNU General Public License as published by //
// the Free Software Foundation, either version 3 of the License, or    //
// (at your option) any later version.                                  //
//                                                                      //
// godefarig is distributed in the hope that it will be useful,         //
// but WITHOUT ANY WARRANTY; without even the implied warranty of       //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        //
// GNU General Public License for more details.                         //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with godefarig.  If not, see <http://www.gnu.org/licenses/>.   //
//////////////////////////////////////////////////////////////////////////

#ifndef MOOSS_CONTAINER_OVERLOAD
#define MOOSS_CONTAINER_OVERLOAD

#include <vector>
#include <array>
#include <tuple>
//#include "fractahedron.h"//PointLocation
#include "glm/glm.hpp"
#include "gfg_utils.h"
#include <ostream>

template<class element, std::size_t size>
std::ostream& operator<<(std::ostream& os, const std::array<element, size>& container)
{
    auto elementIter = container.begin();
    os << "[";
    if(size > 0)
    {
        os << *elementIter;
        for(++elementIter; elementIter != container.end(); ++elementIter)
            os << "; " << *elementIter;
    }
    os << "]";
    return os;
}

template<class element>
std::ostream& operator<<(std::ostream& os, const std::vector<element>& container)
{
   
    auto elementIter = container.begin();
    os << "[";
    if(container.size() > 0)
    {
        os << *elementIter;
        for(++elementIter; elementIter != container.end(); ++elementIter)
            os << "; " << *elementIter;
    }
    os << "]";
    return os;
}

template<class element, std::size_t size>
void setAsMiddleOf(std::array<element, size>& dest, const std::array<element, size>& first, const std::array<element, size>& second)
{
    dest = first;
    auto deit = dest.begin();
    for(auto sit = second.begin(); sit != second.end(); ++sit, ++deit)
    {
        *deit += *sit;
        *deit /= 2;
    }
}

template<class element>
void setAsMiddleOf(glm::tvec3<element>& dest, const glm::tvec3<element>& first, const glm::tvec3<element>& second)
{
    dest = first;
    dest+=second;
    dest/=2;
}

template<class element>
std::ostream& operator<<(std::ostream& os, const glm::tvec3<element>& input)
{
    os << "[" << input.x << "; " << input.y << "; " << input.z << "]";
    return os;
}

template<class element>
std::ostream& operator<<(std::ostream& os, const glm::tvec2<element>& input)
{
    os << "[" << input.x << "; " << input.y << "]";
    return os;
}

template<typename scalar>
std::ostream& operator<<(std::ostream& os, const std::tuple<scalar, gfg::color>& that)
{
    os << " " << std::get<0>(that) << " -> " << std::get<1>(that);
    return os;
}

std::ostream& operator<<(std::ostream& os, const glm::mat4& matrice);
std::ostream& operator<<(std::ostream& os, const glm::mat3& matrice);
std::ostream& operator<<(std::ostream& os, const glm::vec3& vecteur);
std::ostream& operator<<(std::ostream& os, const glm::vec4& vecteur);

#endif//MOOSS_CONTAINER_OVERLOAD
