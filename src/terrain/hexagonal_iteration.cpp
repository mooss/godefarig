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

#include <stdexcept>//runtime_error
#include "terrain/hexagonal_iteration.h"

////////////////////////////////////////
// hexagonal_iteration implementation //
////////////////////////////////////////

gfg::hexagonal_iteration::hexagonal_iteration(fractal_octahedron& target):
    target_(target),
    support_(target.rank(), 2, 1)
{}

gfg::hexagonal_iteration& gfg::hexagonal_iteration::operator++()
{
    
    if(support_.slice_id() == 2 ||
       support_.slice_meta_cardinal() - support_.slice_id() == 2)
    {//edge case, hex are separated by 2 nodes only
        jump_forward_properly(2);
    }
    else if(support_.is_equator())
    {//equator corners are weird af
        equatorial_jump_forward();
    }
    else
    {//normal case
        jump_forward_properly(3);
    }
    
    return *this;
}
    
void gfg::hexagonal_iteration::equatorial_jump_forward()
{
    const unsigned int slice_id_mod_3 = support_.slice_id() % 3;
    if(slice_id_mod_3 == 0)
        throw std::runtime_error("equatorial slice id should never be a multiple of 3");
    else if(support_.side_offset() + 3 >=  support_.side_cardinal())
    {//arriving to the end of current side
        if(slice_id_mod_3 == 1)
            jump_forward_properly(4);
        else
            jump_forward_properly(2);
    }
    else
    {//everything is normal
        if(support_.jump_forward_same_slice(3))
            throw std::runtime_error("overflowing jump in the middle of the equator");
    }
}

void gfg::hexagonal_iteration::jump_forward_properly( unsigned int jump)
{
    if( support_.jump_forward_same_slice(jump) )
    {//an overflow occured
        reposition_center();
    }
}

void gfg::hexagonal_iteration::reposition_center()
{
    support_.next_slice();//reset the side and to offset to 0
    const unsigned int slice_id_mod_3 = support_.slice_id() % 3;
    if(slice_id_mod_3 == 2)
        support_.jump_forward_same_slice(1);
    else if(slice_id_mod_3 == 1)
        support_.jump_forward_same_slice(2);
    //if slice_id_mod_3 == 0 then support_ already points to the center of the first hexagon
}