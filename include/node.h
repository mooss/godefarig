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

/**
 * @file
 * @brief declaration of the node class and its helper classes
 * @date 28 october 2016
 * @author mooss
 */
#ifndef MOOSS_NODE_H
#define MOOSS_NODE_H

#include "slice.h"
#include "make_unique_cpp11.h"
#include <array>

namespace gfg
{

/** @class offset
 * \ref cascade_node helper class.
 * handles basic operations on a fixed-size offset
*/
class offset
{
  public:
    /**
     * @param cardinal the cardinal of the offset
     * @param current the initial offset
     */
    offset(unsigned int cardinal, unsigned int current);
    
    /**
     * @param cardinal the cardinal of the offset
     */
    offset(unsigned int cardinal);
    offset()=delete;

    /** @brief factory method
     * @param stage the stage from which to create the offset
     * @param id the slice id from which to create the offset
     * @param offset the slice offset from which to create the offset
     * @return the corresponding offset
     */
    static offset create_from_node_data(unsigned int stage, unsigned int id, unsigned int offset);
    /** @brief current value getter
     * @return the current offset
     */
    unsigned int current() const {return m_current;}
    /** @brief cardinal getter
     * @return the cardinal
     */
    unsigned int cardinal() const {return m_cardinal;}
    /** @brief overflow test
     * @return true if the current offset is beyond his maximum
     */
    bool has_overflowed() const { return m_current >= m_cardinal; }
    
    /** @brief even test
     * @return true if the current offset is even
     */
    bool is_even() const { return m_current % 2 == 0; }
    /** @brief odd test
     * @return true if the current offset is even
     */
    bool is_odd() const { return m_current %2 == 1; }///<@return true if the offset is odd

    /** @brief increments the offset if it was not maximal
     * @return true if the offset was changed
     */
    bool try_next();

    /** \brief move the offset forward
        \return 0 if no overflow occured, otherwise the size of the overflow
     */
    unsigned int jump_forward(unsigned int value);
    
    /** @brief decrements the offset if it was not minimal
     * @return true if the offset was changed
     */
    bool try_prev();
    void reset() { m_current = 0; }///<reset the offset to the minimum
    void reset_to_max() { m_current = m_cardinal -1;}///<reset the offset to the maximum
    void expand() { ++m_cardinal; }///<increments the cardinal
    void contract() { --m_cardinal; }///<decrements the cardinal

    /** @brief add an integer to the offset without overflowing
     * @param value the integer to add
     * @return the overflowing value (0 if no overflow)
     */
    int add(int value);

    /**
     *@param that the offset to compare with
     *@return true if this and that are equals
     */
    bool operator==(offset const& that) const;
    /**
     *@param that the offset to compare with
     *@return true if this and that are differents
     */
    bool operator!=(offset const& that) const {return !(*this == that); }

  private:
    unsigned int m_cardinal;///<the cardinal
    unsigned int m_current;///<the current value
};

/** @class side
 * \ref cascade_node helper class.
 * represents the side on which a node is on a four-sided slice.
 */
class side
{
  public:
    side(unsigned int id);///<@param id the initial side identifier
    side();/**< the initial side identifier is set to 0*/
    /** @brief id getter
     * @return the id
     */
    unsigned int id() const {return m_id;}
    /** @brief go to next side if the current is <3
     * @return true if the operation was successfull
     */
    bool try_next();
    /** @brief go to prev side if the current is >0
     * @return true if the operation was successfull
     */
    bool try_prev();
    void reset() { m_id = 0; }///< reset the side to 0
    void reset_to_max() { m_id = 3; }///< reset the side to max (3)

    void set_first() { m_id = 0; }
    void set_second() { m_id = 1; }
    void set_third() { m_id = 2; }
    void set_fourth() { m_id = 3; }

    /**
     *@param that the side to compare with
     *@return true if this and that are equals
     */
    bool operator==(side const& that) const;
    /**
     *@param that the side to compare with
     *@return true if this and that are not equals
     */
    bool operator!=(side const& that) const { return !(*this == that); }
    
  protected:
    unsigned int m_id;///< the current side identifier

};

//todo: find a way to document the following:
template<class node_type>
using t_diamond = std::array<node_type, 4>;

template<class node_type>
using t_edge = std::array<node_type, 2>;

/**@class node
 * abstract class representing a node in a \ref fractal_octahedron
 *
 * a node is uniquely identified by three integers:
 * * its **stage**
 * * its **slice**
 * * its **offset**
 *
 * those integers translate to the **index** of the node in the array containing all the nodes
 */
class node
{
  public:
    virtual unsigned int stage() const=0;///<@return the stage
    virtual unsigned int slice_id() const=0;///<@return the slice
    virtual unsigned int slice_offset() const=0;///<@return the offset
    virtual gfg::index index() const=0;///<@return the index
};//class node

class cascade_node : public node
{
  public:
    cascade_node(unsigned int stage, unsigned int slice_id, unsigned int offset);
    cascade_node(unsigned int stage, unsigned int slice_id, unsigned int side_id, unsigned int offset);
    cascade_node()=delete;
    //cascade_node(const cascade_node&);
    //const cascade_node& operator=(const cascade_node&);
    
    //static cascade_node create_from_position(unsigned int stage, gfg::index);

    unsigned int side_cardinal() const { return m_offset.cardinal(); }
    unsigned int side_id() const { return m_side.id(); }
    unsigned int side_offset() const { return m_offset.current(); }
    unsigned int slice_cardinal() const { return 4 * side_cardinal(); }
    unsigned int slice_meta_cardinal() const { return m_slice.meta_cardinal(); }
    unsigned int slice_depth() const { return m_slice.depth(); }
    
    unsigned int depth() const { //return m_depth;
        return gfg::binary_degree(m_slice.depth(), slice_offset());
    }
    unsigned int creation_stage() const { return stage() - depth();}
    unsigned int same_depth_current_slice_offset() const;
    unsigned int prev_depths_offset() const { return gfg::point::number_at_stage(creation_stage() -1); }
    unsigned int same_depth_prev_slice_offset() const { return gfg::slice::correctDataOffset(creation_stage(), slice_id() >> depth()); }//des noms très clairs

    //facade pour slice
    bool is_south() const { return m_slice.is_south(); }///<@return true if this node is the south
    bool is_north() const { return m_slice.is_north(); }///<@return true if this node is the north
    bool is_polar() const { return m_slice.is_polar(); }///<@return true if this node is a polar node (i.e. north or south)
    bool is_equator() const { return m_slice.is_equator(); }///<@return true if this node is an equatorial node
    bool southern_hemisphere() const { return m_slice.southern_hemisphere(); }///<@return true if this node is in the southern hemisphere
    bool northern_hemisphere() const { return m_slice.northern_hemisphere(); }///<@return true if this node is in the northern hemisphere

    //methodes specifiques a cascade_node
    bool is_angular() const { return side_offset() == 0;}///<@return true if this node is angular
    bool is_linear() const { return !is_angular(); }///<@return true if this node is linear
    bool is_corner() const { return is_equator() && is_angular();}///<@return true if this node is one of the four corners
    bool is_beyond_north() const { return is_north() && side_id() != 0; }///<@return true if the forward iteration overflowed
    bool is_initial() const { return depth() == slice_depth(); }///<@return true if this node was created on the same stage as its slice was
    bool is_spontaneous() const { return depth() == 0; }///<@return true if this node was created on the last stage

    unsigned int slice_mirror_id() const { return m_slice.mirror_id(); }///< mirror id of current slice
    //redefinitions de node
    unsigned int stage() const override { return m_slice.stage();}
    unsigned int slice_id() const override {return m_slice.id(); }
    unsigned int slice_offset() const override {return side_offset() + side_cardinal() * side_id(); }
    gfg::index index() const override;

    const gfg::heavy_slice& slice() const { return m_slice; }///< slice getter
    const gfg::offset& offset() const { return m_offset; }///< offset getter
    const gfg::side& side() const { return m_side; }///< side getter

    //methodes d'iteration
    /**@brief go to the next node
     */
    void next();
    
    /**@brief go to the previous node
     */
    void prev();

    /** \brief jumps to another node without further verification
     * \param jump number of nodes to jump
     * \return the number of nodes overflowing
     */
    unsigned int jump_offset_forward(unsigned int jump)
    {
        return m_offset.jump_forward(jump);
    }

    /** \brief jumps to another node but stays on the current slice
     *  \param jump number of nodes to jump
     *  \return the number of nodes overflowing to the next slice
     */
    unsigned int jump_forward_same_slice(unsigned int jump);

    /** \brief jumps to another node but stays on the current side
     *  \param jump number of nodes to jump
     *  \return the number of nodes overflowing to the next side
     */
    unsigned int jump_forward_same_side(unsigned int jump);
    
    /**@brief go to the next node but stay on the same slice, eventually going round*/
    cascade_node& ring_next();
    
    /**@brief go to the previous node but stay on the same slice, eventually going round*/
    cascade_node& ring_prev();
    
    /**@brief go to the next node of the same stage*/
    void last_stage_next();//todo: fix this fonction so that it works even if the current stage is not the last. same thing for prev.
    
    /**@brief go to the previous node of the same stage*/
    void last_stage_prev();

    /**@brief go to the next stage
     *
     * increments the stage while still representing the same node, i.e. the index stays the same.
     */
    cascade_node& next_stage();
    /**@brief go to the prev stage
     *
     * decrements the stage while still representing the same node, i.e. the index stays the same.
     * undefined behaviour if the node was spontaneous.
     */
    cascade_node& prev_stage();

    /** \brief go to the start of next slice
     *
     *  resets m_offset and m_side.
     */
    void next_slice();

    /** \brief goes to the next side
     * 
     * if the current side is the last, goes to the first side of the next slice.
     *
     * resets m_offset.
     */
    void next_side();

    void reset();
    std::vector<gfg::cascade_node>& neighbours(std::vector<gfg::cascade_node>&) const;
    t_diamond<cascade_node> diamond_neighbours() const;
    t_edge<cascade_node> generators() const;
    
    bool operator==(cascade_node const&) const;
    bool operator!=(cascade_node const& comp) const {return !(*this == comp);}
    
  private:
    gfg::heavy_slice m_slice;
    gfg::offset m_offset;
    gfg::side m_side;

    cascade_node& northwards_from_equator();
    cascade_node& southwards_from_equator();
    cascade_node& northwards_to_equator();
    cascade_node& southwards_to_equator();
    void offset_correction();
    // std::vector<cascade_node> neighbours() const;

    //gfg::side& side() { return m_side; }
};//class cascade_node

}//namespace gfg

std::ostream& operator<<(std::ostream& os, gfg::node const& n);
std::ostream& operator<<(std::ostream& os, gfg::cascade_node const& n);
#endif//MOOSS_NODE_H
