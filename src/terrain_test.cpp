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

#include <iostream>
#include "geometry/hexagonal_iterator.h"

int main(int argc, char** argv)
{
    gfg::fractal_octahedron octahedron(2);

    std::cout << "created an octahedron of rank " << octahedron.rank()
              << std::endl;

    gfg::hexagonal_iterator hexiter(octahedron);
    for(std::size_t i = 0; i < 21; ++i)
    {
        std::cout << "hex: " << hexiter.index() << std::endl;
        ++hexiter;
    }

    
    return 0;
}
