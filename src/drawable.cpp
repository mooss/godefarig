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

#include "drawable.h"
#include <cassert>
#include "fractahedron.h"
#include "containerOverload.h"
#include "shader.h"
//#include "graphics/buffer.h"


gfg::TransformationUniforms::TransformationUniforms(Shader& shader,
                       const GLfloat* model_ptr,
                       const GLfloat* view_ptr,
                       const GLfloat* projection_ptr ):
    model_(shader.program(), "model", model_ptr),
    view_(shader.program(), "view", view_ptr),
    projection_(shader.program(), "projection", projection_ptr)
{}

gfg::TransformationUniforms gfg::TransformationUniforms::create(
    Shader& shader,
    const GLfloat* model_ptr,
    const GLfloat* view_ptr,
    const GLfloat* projection_ptr )
{
    shader.bind();
    return gfg::TransformationUniforms(shader, model_ptr, view_ptr, projection_ptr);
}

/////////////////////
// simple_drawable //
/////////////////////

gfg::simple_drawable::simple_drawable(Model& mod):
    model_(mod)
{
    glGenVertexArrays(1, &vao_);
}

gfg::simple_drawable::~simple_drawable()
{
    glDeleteVertexArrays(1, &vao_);
}

void gfg::simple_drawable::bind_vao() const
{
    glBindVertexArray(vao_);
}

void gfg::simple_drawable::unbind_vao() const
{
    glBindVertexArray(0);
}


//////////////////////
// element_drawable //
//////////////////////


gfg::elements_drawable::elements_drawable(GLsizei elNbr, Model& mod, GLenum mode):
    simple_drawable(mod),
    elements_(elNbr),
    mode_(mode)
{}


void gfg::elements_drawable::draw()
{
    bind_vao();
    draw_without_binding();
    unbind_vao();
}

void gfg::elements_drawable::draw_without_binding()
{
    glDrawElements(mode_, elements_, GL_UNSIGNED_INT, 0);//generalise later
}

////////////////////
// drawable_octal //
////////////////////

gfg::drawable_octal::drawable_octal(gfg::fractal_octahedron& octa, unsigned int initial_draw_stage, Model&& mod):
    elements_drawable(3*gfg::face::numberAtStage(initial_draw_stage), mod, GL_TRIANGLES),
    octa_(octa),
    draw_stage_(initial_draw_stage),
    positions_(gfg::gl::buffer_hint(0)),
    colors_(gfg::gl::buffer_hint(1))
{
    send_data_to_gpu();
}

bool gfg::drawable_octal::increment_draw_stage()
{
    if(draw_stage_ == octa_.rank())
        return false;
    ++draw_stage_;
    return apply_draw_stage();
}

bool gfg::drawable_octal::decrement_draw_stage()
{
    if(draw_stage_ == 0)
        return false;
    --draw_stage_;
    return apply_draw_stage();
}

bool gfg::drawable_octal::apply_draw_stage()
{
    
    bind_vao();
    send_indexes_to_gpu();
    unbind_vao();
    
    elements_ = 3 * gfg::face::numberAtStage(draw_stage_);
    return true;
}

void gfg::drawable_octal::send_indexes_to_gpu()
{
    auto indexes = octa_.get_faces_index(draw_stage_);
    ebo_.send( indexes );
}

void gfg::drawable_octal::send_data_to_gpu()
{
    bind_vao();
    
    send_indexes_to_gpu();

    {
        auto positions = octa_.mesh().positions();
        //send_data_to_vertex_buffer(positions, 0, GL_STATIC_DRAW);
        positions_.send(positions);

        // std::cout << "buffer : \n" << position_buffer << "\n"
        //           << "correct_size=" << sizeof(positions.front()) * positions.size() << "\n"
        //           << "correct_stride=" << 3*sizeof(GLfloat) << std::endl;
    }

    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
    //                       3 * sizeof(GLfloat), (GLvoid*)0 );
    // glEnableVertexAttribArray(0);

    //sending the colors
    {
        auto couleurs = octa_.mesh().colors();
        //send_data_to_vertex_buffer(couleurs, 1, GL_STATIC_DRAW);
        colors_.send(couleurs);
    }
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
    //                       3 * sizeof(GLfloat), (GLvoid*)0 );
    // glEnableVertexAttribArray(1);

    unbind_vao(); //unbind vertex array, not element buffer object

}

////////////////////////////
// implementation du cube //
////////////////////////////

gfg::Cube::Cube(float size, Model&& mod):
    EBO_drawable(14, mod)
{
    static GLfloat vertices[] =
    { 
        size/2,	size/2,	size/2,
        -size/2,	size/2,	size/2,
        size/2,	size/2,	-size/2,
        -size/2,	size/2,	-size/2,
        size/2,	-size/2,	size/2,
        -size/2,	-size/2,	size/2,
        -size/2,	-size/2,	-size/2,
        size/2,	-size/2,	-size/2
    };

    static GLuint elements[] = {
        3, 2, 6, 7, 4, 2, 0,
        3, 1, 6, 5, 4, 1, 0
    };
//from www.paridebroggi.com/2015/06/optimized-cube-opengl-triangle-strip.html
    glBindVertexArray(VAO_);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_[0]);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 14 * sizeof(GLuint), elements, GL_STATIC_DRAW);
    glBindVertexArray(0);
}
