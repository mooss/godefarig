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

#ifndef MOOSS_COLOR_HEATMAP_HPP
#define MOOSS_COLOR_HEATMAP_HPP

#include "gfg_utils.h"
#include <algorithm>
#include <vector>
#include <tuple>
#include "containerOverload.h"
#include <iostream>

namespace gfg
{

template<typename scalar>
class color_heatmap
{
  public:
    color_heatmap(std::vector<std::tuple<scalar, gfg::color>>);
    static color_heatmap merge(const color_heatmap&, const color_heatmap&);

    scalar min() const {return std::get<0>(scale_.front()); }
    scalar max() const {return std::get<0>(scale_.back()); }
    gfg::color get_color_of(scalar) const;
    
    void sort();
    void add(std::tuple<scalar, gfg::color>);
    void translate_scale(scalar low_bound, scalar high_bound);
    void reverse();

  private:
    std::vector<std::tuple<scalar, gfg::color>> scale_;

    friend std::ostream& operator<<(std::ostream& os, const color_heatmap& that)
    {
        os << "heatmap : " << that.scale_;
        return os;
    }
};

template<typename scalar>
color_heatmap<scalar>::color_heatmap(std::vector<std::tuple<scalar, gfg::color>> init):
    scale_(init)
{
    sort();
}

template<typename scalar>
gfg::color_heatmap<scalar> gfg::color_heatmap<scalar>::merge(const gfg::color_heatmap<scalar>& a, const gfg::color_heatmap<scalar>& b)
{
    color_heatmap<scalar> result(a);//copy construction
    result.scale_.reserve(a.scale_.size() + b.scale_.size());
    for(auto& el : b.scale_)
        result.scale_.push_back(el);
    result.sort();//inserting the elements directly in the right order would be more efficient
    return result;
}

template<typename scalar>
gfg::color color_heatmap<scalar>::get_color_of(scalar s) const
{
    auto it = std::find_if(scale_.begin(), scale_.end(), [&s](std::tuple<scalar, gfg::color> a)
    {
        return s < std::get<0>(a);
    });

    if(it == scale_.begin())
        return std::get<1>(*it);
    else if(it == scale_.end() )
        return std::get<1>(scale_.back());

    auto prev = it - 1;
    scalar diff = std::get<0>(*prev) - std::get<0>(*it);
    scalar ratio = (diff == 0)? 0 : (s - std::get<0>(*it)) / diff;
    //return std::get<1>(*it);
    return std::get<1>(*it) + ratio * (std::get<1>(*prev) - std::get<1>(*it) );
}

template<typename scalar>
void color_heatmap<scalar>::sort()
{
    std::sort(scale_.begin(), scale_.end(), [](std::tuple<scalar, gfg::color> a, std::tuple<scalar, gfg::color> b)
              {
                  return std::get<0>(a) < std::get<0>(b);
              });
}

template<typename scalar>
void color_heatmap<scalar>::add(std::tuple<scalar, gfg::color> el)
{
    auto it = std::find_if(scale_.begin(), scale_.end(),[&el](std::tuple<scalar, gfg::color> a)
    {
        return std::get<0>(el) < std::get<0>(a);
    });

    if(it == scale_.end())
        scale_.push_back(el);
    else
        scale_.insert(it, el);//tothink: use emplace and emplace_back ?
}

template<typename scalar>
void color_heatmap<scalar>::translate_scale(scalar low_bound, scalar high_bound)
{
    scalar oldmin(min());
    double coeff = (high_bound - low_bound) / (max() - oldmin);
    for(auto& el : scale_)
        std::get<0>(el) = low_bound + (std::get<0>(el) - oldmin) * coeff;
}

template<typename scalar>
void gfg::color_heatmap<scalar>::reverse()
{
    std::reverse(scale_.begin(), scale_.end());
}

}//namespace gfg


#endif//MOOSS_COLOR_HEATMAP_HPP
