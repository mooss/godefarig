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

#include "glToggle.h"

using namespace gfg::input;

std::shared_ptr<toggler> toggler::create(std::function<void(GLenum)> fun, std::vector<GLenum> vec)
{
    return std::make_shared<switcher<GLenum> >(fun, vec);
}
std::shared_ptr<toggler> toggler::create(GLenum cap, bool def)//something something forwarding ?
{
    return std::make_shared<glEnabler>(cap, def);
}

void glSettings::changePrimitiveSize(bool incrementation) const
{
    GLint modes[2];
    glGetIntegerv(GL_POLYGON_MODE, modes);
    if(modes[0] == GL_POINT)//front face mode, 
    {
        GLfloat current, granularity, range[2];
        glGetFloatv(GL_POINT_SIZE, &current);
        glGetFloatv(GL_POINT_SIZE_GRANULARITY, &granularity);
        glGetFloatv(GL_POINT_SIZE_RANGE, range);
        if(incrementation)
        {
            current+=granularity;
            if(current >= range[1])
                current = range[1];
        }
        else
        {
            current-=granularity;
            if(current <= range[0])
                current = range[0];
        }
        glPointSize(current);
    }
    else if(modes[0] == GL_LINE)//ALIASED_LINE
    {
        GLfloat current, granularity=0.5, range[2];
        glGetFloatv(GL_LINE_WIDTH, &current);
        //glGetFloatv(GL_LINE_GRANULARITY, &granularity);
        
        glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, range);
        if(incrementation)
        {
            current+=granularity;
            if(current >= range[1])
                current = range[1];
        }
        else
        {
            current-=granularity;
            if(current <= range[0])
                current = range[0];
        }
        glLineWidth(current);
    }

}
