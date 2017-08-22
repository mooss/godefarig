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
#include "fractahedron.h"

namespace gfg{
/** \brief mean of iteration on all hexagons of an octahedron
 */
class hexagonal_iterator
{
  public:
    /** \brief simplest constructor
     * 
     * creates an hexagonal iterator for the last stage of a fractal_octahedron
     * \param target iterated octahedron
     */
    hexagonal_iterator(fractal_octahedron& target);

    /** \brief jumps to the next hexagon center
     *
     * the iterator keeps pointing toward the last element when the end is reached
     */
    hexagonal_iterator& operator++();

    /** \brief index getter
     */
    fractal_octahedron::index index() const
    { return support_.index(); }
    
  private:
    /** \brief jump forward, going to the next slice if necessary
        \param jump number of nodes to jump
     */
    void jump_forward_properly(unsigned int jump);

    /** \brief go to next slice and make support_ point to the first hex
     */
    void reposition_center();

    /** \brief handles the tricky case of jumping around in the equator
     */
    void equatorial_jump_forward();

    /** \brief sets the end indicator
     */
    void set_end_indicator() { end_indicator_ = true;}
    
    fractal_octahedron& target_;///< target on which the iteration occurs
    cascade_node support_;///< means to do the iteration
    bool end_indicator_ = false;/// determines if the iteration is (or at least should be) over
};
}//namespace gfg
#endif//MOOSS_HEXAGONAL_ITERATOR
