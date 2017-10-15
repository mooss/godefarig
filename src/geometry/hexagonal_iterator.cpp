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
#include "geometry/hexagonal_iterator.h"

////////////////////////////
// hexagon implementation //
////////////////////////////
gfg::hexagon::hexagon(fractal_octahedron& target, const gfg::cascade_node& initial_center):
    target_(target)
{
    update(initial_center);
}

void gfg::hexagon::update(const gfg::cascade_node& center_node)
{
    center_node.neighbours(neighbour_buffer_);
    center_index_ = center_node.index();

    vertices_indexes_[0] = neighbour_buffer_[0].index();
    vertices_indexes_[1] = neighbour_buffer_[1].index();
    vertices_indexes_[2] = neighbour_buffer_[2].index();
    vertices_indexes_[3] = neighbour_buffer_[3].index();
    vertices_indexes_[4] = neighbour_buffer_[4].index();
    vertices_indexes_[5] = neighbour_buffer_[5].index();
}

unsigned int gfg::hexagon::number_at_stage(unsigned int stage)
{
    unsigned int slice_number_per_original_face = pow(2, stage) +1;
    unsigned int sl_div_3 = slice_number_per_original_face / 3;
    return 4* (sl_div_3)* (1+ 3* (sl_div_3-1) + 2* (slice_number_per_original_face %3));
}

///////////////////////////////////////
// hexagonal_iterator implementation //
///////////////////////////////////////
gfg::hexagonal_iterator::hexagonal_iterator(fractal_octahedron& iterated_fractahedron):
    support_(iterated_fractahedron.rank(), 2, 1),
    target_(iterated_fractahedron, support_)
{
    ensure_there_are_hexagons();
}

gfg::hexagonal_iterator::hexagonal_iterator(fractal_octahedron& iterated_fractahedron, unsigned int stage):
    support_(stage, 2, 1),
    target_(iterated_fractahedron, support_)
{
    ensure_there_are_hexagons();
}

gfg::hexagonal_iterator& gfg::hexagonal_iterator::operator++()
{
    if( is_last_hexagon() )
        set_end_indicator();
    else
    {
        jump_forward_properly();
        target_.update(support_);
    }
    
    return *this;
}

///////////////////////////////////////////////////////
// hexagonal_iterator private methods implementation //
///////////////////////////////////////////////////////
bool gfg::hexagonal_iterator::is_last_hexagon()
{
    return support_.slice_id() +3 == support_.slice_meta_cardinal()
        && support_.side_id() == 3;
}

void gfg::hexagonal_iterator::jump_forward_properly()
{
    if( support_.jump_forward_same_side(3) )//there was an overflow
    {
        support_.next_side();
        reposition_center();
    }
}

void gfg::hexagonal_iterator::reposition_center()
{
    unsigned int slice_mod_3 = support_.slice_mirror_id() % 3;
    if(slice_mod_3 == 2)
        support_.jump_offset_forward(1);
    else if(slice_mod_3 == 1)
        support_.jump_offset_forward(2);
}

void gfg::hexagonal_iterator::ensure_there_are_hexagons()
{
    if( support_.stage() == 0 )
        set_end_indicator();
}
