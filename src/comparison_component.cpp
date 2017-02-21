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

#include "comparison_component.h"
#include <algorithm>

using namespace gfg::input;

comparison_leaf::comparison_leaf(pression_status status):
    status_(status)
{}

button_comparison::button_comparison(button id, pression_status status):
    comparison_leaf(status),
    id_(id)
{}

bool button_comparison::conditions_are_met( const input_manager& comp_ref) const
{
    return comp_ref[id_].status() == status_;
}

key_comparison::key_comparison(key id, pression_status status):
    comparison_leaf(status),
    id_(id)
{}

bool key_comparison::conditions_are_met( const input_manager& comp_ref) const
{
    return comp_ref[id_].status() == status_;
}


comparison_composite::comparison_composite()
{}

bool comparison_composite::conditions_are_met( const input_manager& comp_ref) const
{
    for(auto & el : components_)
        if(! el->conditions_are_met(comp_ref) )
            return false;
    return true;
}

void comparison_composite::add( std::shared_ptr<comparison_component> el)
{
    if( std::find(components_.begin(), components_.end(), el)
        == components_.end())
        components_.push_back(el);
}

void comparison_composite::remove( std::shared_ptr<comparison_component> el)
{
    auto it = std::find(components_.begin(), components_.end(), el);
    if( it != components_.end())
        components_.erase(it);
}
