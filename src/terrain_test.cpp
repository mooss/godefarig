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
    unsigned int stage = 8;
    gfg::fractal_octahedron octahedron(stage);

    std::cout << "created an octahedron of rank " << octahedron.rank()
              << std::endl;
    // for(gfg::hexagonal_iterator hexit(octahedron); hexit != gfg::hexagonal_iterator::end(); ++hexit)
    // {
    //     std::cout << "\t" << hexit.index() << "\n";
    // }

    for(unsigned int current_stage = 0; current_stage <= stage; ++current_stage)
    {
        unsigned int hex_card = 0;
        for(gfg::hexagonal_iterator hexiter(octahedron, current_stage);
            hexiter != gfg::hexagonal_iterator::end();
            ++hexiter)
        {
            ++hex_card;
        }
        std::cout << "stage " << current_stage << " :\n\t"
                  << hex_card  << "/" << gfg::hexagon::number_at_stage(current_stage) << " hex ; "
                  << gfg::point::number_at_stage(current_stage) << " points ; "
                  << gfg::face::number_at_stage(current_stage) << " faces ; "
                  << gfg::edge::number_at_stage(current_stage) << " edges" << std::endl;

    }
//    for(std::size_t i = 0; i < 21; ++i)
    
    return 0;
}
