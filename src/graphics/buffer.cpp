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

#include "graphics/buffer.h"



std::size_t gfg::gl::gl_enum_type_size(GLenum value)
{
    switch(value)
    {
      case GL_BYTE:
          return sizeof(GLbyte);
      case GL_UNSIGNED_BYTE:
          return sizeof(GLubyte);
      case GL_SHORT:
          return sizeof(GLshort);
      case GL_UNSIGNED_SHORT:
          return sizeof(GLushort);
      case GL_INT:
          return sizeof(GLint);
      case GL_UNSIGNED_INT:
          return sizeof(GLuint);
      case GL_HALF_FLOAT:
          return sizeof(GLhalf);
      case GL_FLOAT:
          return sizeof(GLfloat);
      case GL_DOUBLE:
          return sizeof(GLdouble);
    }
    return 0;
}



/////////////////
// buffer_hint //
/////////////////

gfg::gl::buffer_hint::buffer_hint(GLuint index, GLenum type, GLboolean normalised, GLvoid* pointer):
    index_(index),
    type_(type),
    normalised_(normalised),
    pointer_(pointer)
{}

////////////
// buffer //
////////////

gfg::gl::buffer::buffer(GLenum target):
    target_(target)
{
    glGenBuffers(1, &handle_);
}


gfg::gl::buffer::~buffer()
{
    glDeleteBuffers(1, &handle_);
}

void gfg::gl::buffer::bind() const
{
    glBindBuffer(target_, handle_);
}

///////////////////
// vertex_buffer //
///////////////////

gfg::gl::vertex_buffer::vertex_buffer(gfg::gl::buffer_hint const& hint):
    buffer(GL_ARRAY_BUFFER),
    hint_(hint)
{}

////////////////////
// element_buffer //
////////////////////

gfg::gl::element_buffer::element_buffer():
    buffer(GL_ELEMENT_ARRAY_BUFFER)
{}

////////////////////////
// stream redirection //
////////////////////////

std::ostream& operator<<(std::ostream& os, const gfg::gl::buffer_hint& hint)
{
    os << "index=" << hint.index()
       << " type=" << hint.type()
       << " normalised=" << ((hint.normalised() == GL_TRUE) ? "true" : "false")
       << " pointer=" << hint.pointer();
    return os;
}




std::ostream& operator<<(std::ostream& os, const gfg::gl::buffer& buf)
{
    os << "handle : " << buf.handle() << "\n"
       << "target : " << buf.target() << "\n";
    return os;
}
