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

///////////////////////
// utility functions //
///////////////////////

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

std::string gfg::gl::to_str(GLenum value)
{
    switch(value)
    {
        ///////////
        // types //
        ///////////
      case GL_BYTE:
          return "GL_BYTE";
      case GL_UNSIGNED_BYTE:
          return "GL_UNSIGNED_BYTE";
      case GL_SHORT:
          return "GL_SHORT";
      case GL_UNSIGNED_SHORT:
          return "GL_UNSIGNED_SHORT";
      case GL_INT:
          return "GL_INT";
      case GL_UNSIGNED_INT:
          return "GL_UNSIGNED_INT";
      case GL_HALF_FLOAT:
          return "GL_HALF_FLOAT";
      case GL_FLOAT:
          return "GL_FLOAT";
      case GL_DOUBLE:
          return "GL_DOUBLE";

          /////////////
          // targets //
          /////////////
      case GL_ARRAY_BUFFER:
          return "GL_ARRAY_BUFFER";
      case GL_ELEMENT_ARRAY_BUFFER:
          return "GL_ELEMENT_ARRAY_BUFFER";

          //////////////
          // booleans //
          //////////////

      case GL_TRUE:
          return "GL_TRUE";
      case GL_FALSE:
          return "GL_FALSE";
    }
    return "unknown<" + std::to_string(value) + ">";
    
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
       << " type=" << gfg::gl::to_str(hint.type())
       << " type_size=" << hint.type_size()
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

std::ostream& operator<<(std::ostream& os, const gfg::gl::vertex_buffer& buf)
{
    os << reinterpret_cast<const gfg::gl::buffer&>(buf)
       << "hint : " << buf.hint();
    return os;
}
