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

#include "node.h"

//####################################################################
//########################### offset #################################
//###                                                              ###

gfg::offset::offset(unsigned int cardinal, unsigned int current):
    m_cardinal(cardinal),
    m_current(current)
{}

gfg::offset::offset(unsigned int cardinal):
    offset(cardinal, 0)
{}

gfg::offset gfg::offset::create_from_node_data(unsigned int stage, unsigned int id, unsigned int offset)
{
    id = gfg::slice::mirror_id(stage, id);

    if(id != 0)
        offset %= id;
    else
        offset = 0;
    
    gfg::offset result(id, offset);
    return result;
}


bool gfg::offset::try_next()
{
    if(m_current +1 < m_cardinal)
    {
        ++m_current;
        return true;
    }
    return false;
}

bool gfg::offset::try_prev()
{
    if(m_current > 0)
    {
        --m_current;
        return true;
    }
    return false;
}

unsigned int gfg::offset::jump_forward(unsigned int value)
{
    m_current += value;
    if(has_overflowed())
        return m_current - m_cardinal + 1;//+1 because m_current == m_cardinal corresponds to an overflow of one
    return 0;
}

int gfg::offset::add(int value)
{
    if(value >= 0)
    {
        if(m_current + value >= m_cardinal)
        {
            reset_to_max();
            return m_current + value - m_cardinal +1;
        }
        m_current += value;
        return 0;
    }
    else
        if( static_cast<unsigned int>(std::abs(value)) > m_current )
        {
            value += m_current;
            reset();
            return value;
        }
        else
        {
            m_current += value;
            return 0;
        }
}

bool gfg::offset::operator==(gfg::offset const& that) const
{
    return m_current == that.m_current
        && m_cardinal == that.m_cardinal;
}

//##############################################################
//########################## side ##############################
//###                                                        ###

gfg::side::side(unsigned int id):
    m_id(id)
{}

gfg::side::side():
    side(0)
{}

bool gfg::side::try_next()
{
    if(m_id <3)
    {
        ++m_id;
        return true;//return ++m_id ?
    }
    return false;
}

bool gfg::side::try_prev()
{
    if(m_id > 0)
    {
        --m_id;
        return true;
    }
    return false;
}

bool gfg::side::operator==(gfg::side const& that) const
{
    return m_id == that.m_id;
}

//###################################################################
//########################## cascade_node ###########################
//###                                                             ###

gfg::cascade_node::cascade_node(unsigned int stage, unsigned int slice_id, unsigned int offset) :
    m_slice(stage, slice_id),
    m_offset(gfg::offset::create_from_node_data(stage, slice_id, offset)),
    m_side((m_offset.cardinal() != 0) ? offset / m_offset.cardinal() : 0)
{}

gfg::cascade_node::cascade_node(unsigned int stage, unsigned int slice_id, unsigned int side_id, unsigned int offset ):
    m_slice(stage, slice_id),
    m_offset(slice_id, offset),
    m_side(side_id)
{}

// gfg::cascade_node::cascade_node(const gfg::cascade_node& that) :
//     m_slice(that.m_slice),
//     m_offset(that.m_offset),
//     m_side(that.m_side)
// {}

// const gfg::cascade_node& gfg::cascade_node::operator=(const gfg::cascade_node& that)
// {
//     m_slice = that.m_slice;
//     m_offset = that.m_offset;
//     m_side = that.m_side;
//     return *this;
// }


unsigned int gfg::cascade_node::same_depth_current_slice_offset() const
{
    unsigned int result = slice_offset() >> depth();
    if( !is_initial() )//the current node is not an initial node
    {
        --result;
        result >>= 1;
    }
    return result;
}

void gfg::cascade_node::next()
{
    //if(!m_offset.try_next() && !is_north())//pour ne pas "dépasser" le nord
    if(!m_offset.try_next())
        next_side();
}

void gfg::cascade_node::prev()//todo: make this work
{
    if(!m_offset.try_prev())
    {
        if(!m_side.try_prev())
        {
            m_side.reset_to_max();
            m_slice.down();
            if(southern_hemisphere())
            {
                m_offset.contract();
                if(is_south())
                {
                    m_side.reset();
                    return;
                }
            }
            else
                m_offset.expand();
        }
        m_offset.reset_to_max();
    }
}

unsigned int gfg::cascade_node::jump_forward_same_slice(unsigned int value)
{
    unsigned int buffer;
    if( (buffer = m_offset.jump_forward(value)) )// buffer > 0 ; overflow
    {
        if( !m_side.try_next() || is_south() )
            return buffer;
        else
        {
            m_offset.reset();
            return jump_forward_same_slice(buffer-1);
        }
    }
    return 0;
}

gfg::cascade_node& gfg::cascade_node::ring_next()
{
    if(!m_offset.try_next())
    {
        m_offset.reset();
        if(!m_side.try_next())
        {
            m_side.reset();
        }
    }
    return *this;
}

gfg::cascade_node& gfg::cascade_node::ring_prev()
{
    if(!m_offset.try_prev())
    {
        m_offset.reset_to_max();
        if(!m_side.try_prev())
        {
            m_side.reset_to_max();
        }
    }
    return *this;
}

void gfg::cascade_node::last_stage_next()
{
    if(m_slice.is_even())
    {
        if(m_offset.add(2) != 0)
        {
            m_offset.reset();
            if(!m_side.try_next() || is_south())
            {
                m_side.reset();
                m_slice.up();

                if(northern_hemisphere())
                    m_offset.contract();
                else
                    m_offset.expand();
            }
            else
                m_offset.try_next();
        }
    }
    else
    {
        if(!m_offset.try_next())//all offset of an odd spiral are generated on the last stage
        {
            if(!m_side.try_next())
            {
                m_side.reset();
                next_slice();
                m_offset.try_next();
            }
            else
                m_offset.reset();
        }
    }
}

// void gfg::cascade_node::last_stage_prev()
// {
    
// }

gfg::cascade_node& gfg::cascade_node::prev_stage()
{
    m_slice.prev_stage();
    m_offset = gfg::offset( gfg::slice::mirror_id(stage(), slice_id()),
                            m_offset.current() >> 1);
    return *this;
}

gfg::cascade_node& gfg::cascade_node::next_stage()
{
    m_slice.next_stage();
    m_offset = gfg::offset( gfg::slice::mirror_id(stage(), slice_id()),
                            m_offset.current() << 1);
    return *this;
}

void gfg::cascade_node::next_side()
{
    if(!m_side.try_next() || is_south())
        next_slice();
    else
        m_offset.reset();
}

void gfg::cascade_node::next_slice()
{
    m_side.reset();
    m_offset.reset();
    m_slice.up();

    if(northern_hemisphere())
        m_offset.contract();
    else
        m_offset.expand();
}

void gfg::cascade_node::reset()
{
    m_slice.reset();
    m_side.reset();
    m_offset.reset();
}

gfg::index gfg::cascade_node::index() const
{
    return same_depth_current_slice_offset() + same_depth_prev_slice_offset() + prev_depths_offset();
}

std::vector<gfg::cascade_node>& gfg::cascade_node::neighbours(std::vector<cascade_node>& result) const
{
    result.resize(6, *this);
    for(auto & el : result)
        el = *this;
    
    if(southern_hemisphere())
    {
        if(is_south())
        {
            result.resize(4, *this);
            for(auto & el : result)
                el.northwards_to_equator();
            result[3].m_side.set_second();
            result[2].m_side.set_third();
            result[1].m_side.set_fourth();
        }
        else if(is_angular())
        {
            result[0].southwards_from_equator();
            result[5].ring_prev();
            result[1].ring_next();
            result[4].northwards_to_equator().ring_prev();
            result[3].northwards_to_equator();
            result[2].northwards_to_equator().ring_next();
        }
        else//southern_hemisphere linear node
        {
            result[0].southwards_from_equator().ring_prev();
            result[1].southwards_from_equator();
            result[5].ring_prev();
            result[2].ring_next();
            result[4].northwards_to_equator();
            result[3].northwards_to_equator().ring_next();
        }
    }
    else if(northern_hemisphere())
    {
        if(is_north())
        {
            result.resize(4, *this);
            for(auto& el : result)
                el.southwards_to_equator();
            result[1].m_side.set_second();
            result[2].m_side.set_third();
            result[3].m_side.set_fourth();
        }
        else if(is_angular())
        {
            result[0].southwards_to_equator().ring_prev();
            result[1].southwards_to_equator();
            result[2].southwards_to_equator().ring_next();
            result[5].ring_prev();
            result[3].ring_next();
            result[4].northwards_from_equator();
        }
        else//northern_hemisphere linear node
        {
            result[0].southwards_to_equator();
            result[1].southwards_to_equator().ring_next();
            result[5].ring_prev();
            result[2].ring_next();
            result[4].northwards_from_equator().ring_prev();
            result[3].northwards_from_equator();
        }
    }
    else//equator
    {
        if(is_angular())//front, back, left and right
        {
            result.resize(4, *this);
            result[0].southwards_from_equator();
            result[3].ring_prev();
            result[1].ring_next();
            result[2].northwards_from_equator();
        }
        else
        {
            result[0].southwards_from_equator().ring_prev();
            result[1].southwards_from_equator();
            result[5].ring_prev();
            result[2].ring_next();
            result[4].northwards_from_equator().ring_prev();
            result[3].northwards_from_equator();
        }
    }
    return result;
}//neighbours

gfg::t_edge<gfg::cascade_node> gfg::cascade_node::generators() const//only work if spontaneous
{
    gfg::t_edge<gfg::cascade_node> result{ *this, *this };
    
    if(stage() > 0)
    {
        if(m_slice.is_even())
        {
            result[0].m_offset.try_prev();
            result[0].prev_stage();
            
            result[1].m_offset.try_prev();
            result[1].prev_stage();
            result[1].ring_next();
        }
        else if(m_offset.is_even())
        {
            result[0].m_slice.down();
            result[0].prev_stage();
            
            result[1].m_slice.up();
            result[1].prev_stage();

            result[0].offset_correction();
            result[1].offset_correction();
        }
        else if(southern_hemisphere())
        {
            result[0].m_slice.down();
            result[0].m_offset.try_prev();
            result[0].prev_stage();
            
            result[1].m_slice.up();
            result[1].m_offset.try_next();
            result[1].prev_stage();
        }
        else//north && odd slice && odd offset
        {
            result[0].m_slice.down();
            result[0].m_offset.try_next();
            result[0].prev_stage();
            
            result[1].m_slice.up();
            result[1].m_offset.try_prev();
            result[1].prev_stage();
            
        }
    }//if(stage() > 0)
    
    return result;
}//generators

//todo: a method setting a cascade_node to its spontaneous stage and use it in generators and diamond_neighbours

gfg::t_diamond<gfg::cascade_node> gfg::cascade_node::diamond_neighbours() const//only work if spontaneous
{
    gfg::t_diamond<gfg::cascade_node> result{*this, *this, *this, *this};
    if(stage() > 0)
    {
        if(southern_hemisphere())
        {
            if(is_angular())
            {
                result[0].southwards_from_equator().prev_stage();
                result[1].northwards_to_equator().prev_stage().ring_prev();
                result[2].northwards_to_equator().prev_stage();
                result[3].northwards_to_equator().prev_stage().ring_next();
            }
            else if(m_offset.is_odd())
            {
                result[0].southwards_from_equator().ring_prev().prev_stage();
                result[1].southwards_from_equator().ring_next().prev_stage();
                result[2].northwards_to_equator().ring_prev().prev_stage();
                result[3].northwards_to_equator().ring_next().prev_stage();
            }
            else//south linear even
            {
                result[0].southwards_from_equator().prev_stage().ring_prev();
                result[1].southwards_from_equator().prev_stage();
                result[2].northwards_to_equator().prev_stage();
                result[3].northwards_to_equator().prev_stage().ring_next();
            }
        }
        else if(northern_hemisphere())
        {
            if(is_angular())
            {
                result[0].southwards_to_equator().prev_stage().ring_prev();
                result[1].southwards_to_equator().prev_stage();
                result[2].southwards_to_equator().prev_stage().ring_next();
                result[3].northwards_from_equator().prev_stage();
            }
            else if(m_offset.is_odd())
            {
                result[0].southwards_to_equator().ring_prev().prev_stage();
                result[1].southwards_to_equator().ring_next().prev_stage();
                result[2].northwards_from_equator().ring_prev().prev_stage();
                result[3].northwards_from_equator().ring_next().prev_stage();
            }
            else//north linear even
            {
                result[0].southwards_to_equator().prev_stage();
                result[1].southwards_to_equator().prev_stage().ring_next();
                result[2].northwards_from_equator().prev_stage().ring_prev();
                result[3].northwards_from_equator().prev_stage();
            }
        }
        else//equator
        {
            result[0].ring_prev().prev_stage().southwards_from_equator();//probably incorrect for stage 1
            result[1].ring_prev().prev_stage();
            result[2].ring_next().prev_stage();
            result[3].ring_prev().prev_stage().northwards_from_equator();
        }
    }//if stage() > 0
    return result;
    
}//diamond_neighbours

bool gfg::cascade_node::operator==(gfg::cascade_node const& comp) const
{
    return m_slice == comp.m_slice
        && m_offset == comp.m_offset
        && m_side == comp.m_side;
}

gfg::cascade_node& gfg::cascade_node::northwards_from_equator()
{
    m_slice.up();
    m_offset.contract();
    offset_correction();//peut être facultatif
    return *this;
}

gfg::cascade_node& gfg::cascade_node::southwards_from_equator()
{
    m_slice.down();
    m_offset.contract();
    offset_correction();//peut être facultatif
    return *this;
}

gfg::cascade_node& gfg::cascade_node::northwards_to_equator()
{
    m_slice.up();
    m_offset.expand();
    return *this;
}

gfg::cascade_node& gfg::cascade_node::southwards_to_equator()
{
    m_slice.down();
    m_offset.expand();
    return *this;
}

void gfg::cascade_node::offset_correction()
{
    if(m_offset.has_overflowed())
    {
        m_offset.reset();
        if(!m_side.try_next())
            m_side.reset();
    }
}

//#####################################################################
//######################## stream overloads ###########################
//###                                                               ###

std::ostream& operator<<(std::ostream& os, gfg::node const& n)
{
    os << "node(" << n.stage() << ", " << n.slice_id() << ", " << n.slice_offset() << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, gfg::cascade_node const& n)
{
    os << n.index();
    return os;
}
