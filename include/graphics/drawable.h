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

#ifndef MOOSS_DRAWABLE_HPP
#define MOOSS_DRAWABLE_HPP

#include "graphics/uniform.h"
//#include <initializer_list>
#include "camera.h"
#include <array>
#include <iostream>
#include "gfg_utils.h"
#include "graphics/buffer.h"

namespace gfg
{

class drawable
{
  public:
    virtual ~drawable(){};
    virtual void draw() const=0;
};

class Shader;

class fractal_octahedron;

class simple_drawable : public gfg::drawable
{
  public:
    simple_drawable();

    virtual ~simple_drawable();
    virtual void draw() const=0;

    void bind_vao() const;
    void unbind_vao() const;
    
  private:
    GLuint vao_;
};//class simple_drawable

class elements_drawable : public simple_drawable
{
  public:
    elements_drawable()=delete;
    elements_drawable(GLsizei elNbr, GLenum mode);
        
    virtual void draw() const override;

    void draw_without_binding() const;
    
  protected:
    gfg::gl::element_buffer ebo_;
    GLsizei elements_;
    GLenum mode_;
};//class element_drawable

class drawable_octal : public elements_drawable//todo: use GL_TRIANGLE_STRIP
{
  public:
    drawable_octal()=delete;
    // drawable_octal(unsigned int stage);
    drawable_octal(gfg::fractal_octahedron&, unsigned int initial_draw_stage);
    
    ~drawable_octal(){}
    bool increment_draw_stage();
    bool decrement_draw_stage();
    
    
  protected:
    gfg::fractal_octahedron& octa_;
    unsigned int draw_stage_;

    gfg::gl::vertex_buffer positions_;
    gfg::gl::vertex_buffer colors_;
    gfg::gl::vertex_buffer normals_;

    void send_indexes_to_gpu();
    void send_data_to_gpu();
    bool apply_draw_stage();
};

class cube : public elements_drawable
{
  public:
    cube(GLfloat size);
  private:
    gfg::gl::vertex_buffer positions_;
};//class cube

}//namespace gfg

#endif//MOOSS_DRAWABLE_HPP
