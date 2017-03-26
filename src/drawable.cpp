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

gfg::drawable_octal::drawable_octal(gfg::fractal_octahedron& octa, Model&& mod):
    EBO_drawable(3*octa.face_cardinal(), mod),
    octa_(octa),
    draw_stage_(octa_.rank())
{
    sendDataToGpu();
}

bool gfg::drawable_octal::increment_draw_stage()
{
    ++draw_stage_;
    return apply_draw_stage();
}

bool gfg::drawable_octal::decrement_draw_stage()
{
    --draw_stage_;
    return apply_draw_stage();
}

bool gfg::drawable_octal::apply_draw_stage()
{
    if(draw_stage_ < 1)
    {
        draw_stage_ = 1;
        return false;
    }
    if(draw_stage_ > octa_.rank())
    {
        draw_stage_ = octa_.rank();
        return false;
    }
    
    elements_ = 3 * gfg::face::numberAtStage(draw_stage_);
    return true;
}

void gfg::drawable_octal::sendDataToGpu()
{
    glBindVertexArray(VAO_);
    
    {
        auto indexes = octa_.get_faces_index();
        send_data_to_element_buffer(indexes, GL_STATIC_DRAW);
    }

    {
        auto positions = octa_.mesh().positions();
        send_data_to_vertex_buffer(positions, 0, GL_STATIC_DRAW);
    }
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          3 * sizeof(GLfloat), (GLvoid*)0 );
    glEnableVertexAttribArray(0);

    //sending the colors
    {
        auto couleurs = octa_.mesh().colors();
        send_data_to_vertex_buffer(couleurs, 1, GL_STATIC_DRAW);
    }
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          3 * sizeof(GLfloat), (GLvoid*)0 );
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); //unbind vertex array, not element buffer object

}

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
