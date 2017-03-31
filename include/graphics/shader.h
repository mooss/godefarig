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

#ifndef MOOSS_SHADER_H
#define MOOSS_SHADER_H

#include <string>
#include "myglad.h"

namespace gfg
{
class Shader
{
  public:
    Shader(const std::string& filename);
    Shader()=delete;
    ~Shader();
    //Shader(const Shader& source){}

    void bind();
    void load();

    GLuint program() {return program_;}
    
  private:
    void operator=(const Shader& source){}

    std::string filename_;
    static const unsigned int NUM_SHADERS = 2;
    GLuint program_;
    GLuint shaders_[NUM_SHADERS];
};

}//namespace gfg

#endif// MOOSS_SHADER_H
