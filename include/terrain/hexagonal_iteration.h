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

#ifndef MOOSS_HEXAGONAL_ITERATION
#define MOOSS_HEXAGONAL_ITERATION

#include <vector>
#include "fractahedron.h"
namespace gfg{
/** \brief central node of an hexagon cell and operations on said cell
 */
class hexagonal_iteration
{
  public:
    hexagonal_iteration(fractal_octahedron& target);
    hexagonal_iteration& operator++();
    fractal_octahedron::index index() const
    { return support_.index(); }
    
  private:
    void equatorial_jump_forward();
    void jump_forward_properly(unsigned int jump);
    void reposition_center();
    
    fractal_octahedron& target_;
    cascade_node support_;
};
}//namespace gfg
#endif//MOOSS_HEXAGONAL_ITERATION
