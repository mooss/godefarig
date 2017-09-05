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
    gfg::fractal_octahedron octahedron(3);

    std::cout << "created an octahedron of rank " << octahedron.rank()
              << std::endl;

    gfg::hexagonal_iterator hexiter(octahedron, 2);
    unsigned int hex_card = 0;
//    for(std::size_t i = 0; i < 21; ++i)
    for(; hexiter != gfg::hexagonal_iterator::end(); ++hexiter)
    {
        ++hex_card;
        std::cout << "hex: " << hexiter.index() << std::endl;
        std::cout << "\tneighbour 0 : " << hexiter->vertex(0) << std::endl;
        std::cout << "\tneighbour 1 : " << hexiter->vertex(1) << std::endl;
        std::cout << "\tneighbour 2 : " << hexiter->vertex(2) << std::endl;
        std::cout << "\tneighbour 3 : " << hexiter->vertex(3) << std::endl;
        std::cout << "\tneighbour 4 : " << hexiter->vertex(4) << std::endl;
        std::cout << "\tneighbour 5 : " << hexiter->vertex(5) << std::endl;
    }
    std::cout << hex_card  << " hexagon iterated through" << std::endl;
    
    return 0;
}
