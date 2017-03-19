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


#ifndef MOOSS_GL_BUFFER_H
#define MOOSS_GL_BUFFER_H

#include "myglad.h"
#include <vector>

namespace gfg{
namespace gl{

class buffer_data
{
    
};//todo: write this

class buffer
{
  public:
    buffer()=delete;
    buffer(GLenum target);
    buffer(GLenum target, GLsizei size);
    virtual ~buffer();
    
    void bind();
    void bind(unsigned int indice);

    void send_data(buffer_data const& data);//todo: write this too

  private:
    GLenum target_;
    GLsizei size_;
    std::vector<GLuint> handles_;
};//class buffer

class vbo
{
  public:
    vbo();

};//class vbo



}//namespace gl
}//namespace gfg

#endif//MOOSS_GL_BUFFER_H
