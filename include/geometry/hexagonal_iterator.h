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

#ifndef MOOSS_HEXAGONAL_ITERATOR
#define MOOSS_HEXAGONAL_ITERATOR

#include <vector>
#include <array>
#include "fractahedron.h"

namespace gfg{

class hexagon
{
  public:
    /** \param target targeted fractahedron
     * \param initial_center center of the to-be-constructed hexagon
     */
    hexagon(fractal_octahedron& target, const cascade_node& initial_center);

    /** \brief read-write access to a vertex of the hexagon
     * \param vertex_number number of the vertex to access
     */
    fractal_octahedron::elevation& vertex(std::size_t vertex_number)
    {
        return target_.elevations()[vertices_indexes_[vertex_number]];
    }

    /** \brief read-only access to a vertex of the hexagon
     * \param vertex_number number of the vertex to access
     */
    const fractal_octahedron::elevation& vertex(std::size_t vertex_number) const
    {
        return target_.elevations()[vertices_indexes_[vertex_number]];
    }

    /** \brief get index of vertex
     * \param vertex number of the vertex
     */
    unsigned int vertex_index(std::size_t vertex) const
    {
        return vertices_indexes_[vertex];
    }

    /** \brief read-write access to the center of the hexagon
     * \return the center of the hexagon
     */
    fractal_octahedron::elevation& center(std::size_t neighbour)
    {
        return target_.elevations()[center_index_];
    }

    /** \brief read-only access to the center of the hexagon
     * \return the center of the hexagon
     */
    const fractal_octahedron::elevation& center(std::size_t neighbour) const
    {
        return target_.elevations()[center_index_];
    }

    /** \brief get index of center
     */
    unsigned int center_index() const { return center_index_; }

    /** \brief equality operator
     * \param that comparison
     */
    bool operator==(const hexagon& that) const
    {
        return &target_ == &that.target_
            && center_index_ == that.center_index_;
    }

    /** \brief difference operator
     * \param that comparison
     */
    bool operator!=(const hexagon& that) const
    {
        return !(*this == that);
    }

    /** \brief make this point to another hexagon
     * \param center center of the newly pointed-to hexagon
     */
    void update(const cascade_node& center);

    /** \brief number of hexagons present on an octahedron at a given stage
     * \param stage stage
     */
    static unsigned int number_at_stage(unsigned int stage);

  private:
    fractal_octahedron& target_;///< structure on which is stored the hexagon
    std::vector<cascade_node> neighbour_buffer_;///buffer used to store the offset from cascade_node::neighbours()
    unsigned int center_index_;///< index of the center
    std::array<unsigned int, 6> vertices_indexes_;///< indexes of the vertices
};

/** \brief make iteration possible on all hexagons of an octahedron
 */
class hexagonal_iterator
{
  public:
    /** \brief simplest constructor
     * 
     * creates an hexagonal iterator for the last stage of a fractal_octahedron
     * \param iterated_fractahedron iterated octahedron
     *
     * after construction, an hexagonal_iterator points to the first hexagon
     */
    hexagonal_iterator(fractal_octahedron& iterated_fractahedron);

    /** \brief stage constructor
     * 
     * creates an hexagonal iterator for a parametrised stage of a fractal_octahedron
     * \param iterated_fractahedron iterated octahedron
     * \param stage iteration stage
     *
     * after construction, an hexagonal_iterator points to the first hexagon
     */
    hexagonal_iterator(fractal_octahedron& iterated_fractahedron, unsigned int stage);

    /** \brief jumps to the next hexagon center
     *
     * the iterator keeps pointing toward the last element when the end is reached
     */
    hexagonal_iterator& operator++();

    /** \return the targeted hexagon
     */
    hexagon& operator*() {return target_;}

    /** \return the targeted hexagon
     */
    const hexagon& operator*() const {return target_;}

    /** \brief arrow operator overload
     * \return the targeted hexagon
     */
    hexagon* operator->() { return &target_; }
    
    /** \brief const arrow operator overload
     * \return the targeted hexagon
     */
    const hexagon* operator->() const { return &target_; } 

    /** \brief index getter
     */
    unsigned int index() const
    { return support_.index(); }

    class end
    {
      public:
        end(){}
    };
    
    bool operator==(const hexagonal_iterator::end& that) const
    {
        return end_indicator_;
    }

    bool operator!=(const hexagonal_iterator::end& that) const
    {
        return !(*this == that);
    }

    bool is_last_hexagon();///< \return true if the targeted hexagon is the last
    
  private:
    /** \brief jump forward, going to the next side, or the next slice if necessary
     *
     * the process is side-based : we know how to go on the next hexagon of a side and we know where is the first hexagon of a side.
     * so once one side is overflowing, we go to the next side and to the first hexagon of that side.
     */
    void jump_forward_properly();

    /** \brief go to next slice and make support_ point to the first hex
     */
    // void reposition_center();

    /** \brief make this point to the first hexagon of the current side
    */
    void reposition_center();

    /** \brief handles the tricky case of jumping around in the equator
     */
    void equatorial_jump_forward();

    /** \brief sets the end indicator
     */
    void set_end_indicator() { end_indicator_ = true;}

    /** \brief sets the end indicator if there are no hexagons to iterate through
     */
    void ensure_there_are_hexagons();
    
    cascade_node support_;///< iteration helper
    hexagon target_;///< target on which the iteration occurs
    bool end_indicator_ = false;/// determines if the iteration is (or at least should be) over
};
}//namespace gfg
#endif//MOOSS_HEXAGONAL_ITERATOR
