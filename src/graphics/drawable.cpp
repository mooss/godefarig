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

#include "graphics/drawable.h"
#include <cassert>
#include "fractahedron.h"
#include "container_overload.h"
#include "graphics/shader.h"
//#include "graphics/buffer.h"


/////////////////////
// simple_drawable //
/////////////////////

gfg::simple_drawable::simple_drawable()
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


gfg::elements_drawable::elements_drawable(GLsizei elNbr, GLenum mode):
    elements_(elNbr),
    mode_(mode)
{}


void gfg::elements_drawable::draw() const
{
    bind_vao();
    draw_without_binding();
    unbind_vao();
}

void gfg::elements_drawable::draw_without_binding() const
{
    glDrawElements(mode_, elements_, GL_UNSIGNED_INT, 0);//generalise later
}

/////////////////////////////////
// drawable_fractal_octahedron //
/////////////////////////////////
gfg::drawable_fractal_octahedron::drawable_fractal_octahedron(gfg::fractal_octahedron& octa, unsigned int initial_draw_stage):
    elements_drawable(3*gfg::face::numberAtStage(initial_draw_stage), GL_TRIANGLES),
    octa_(octa),
    draw_stage_(initial_draw_stage),
    positions_(gfg::gl::buffer_hint(0)),
    colors_(gfg::gl::buffer_hint(1)),
    normals_(gfg::gl::buffer_hint(2))
{}

//////////////////////////////
// drawable_octal_triangles //
//////////////////////////////
gfg::drawable_octal_triangles::drawable_octal_triangles(gfg::fractal_octahedron& octa, unsigned int initial_draw_stage):
    drawable_fractal_octahedron(octa, initial_draw_stage)
{
    send_data_to_gpu();
}

bool gfg::drawable_octal_triangles::increment_draw_stage()
{
    if(draw_stage_ == octa_.rank())
        return false;
    ++draw_stage_;
    return apply_draw_stage();
}

bool gfg::drawable_octal_triangles::decrement_draw_stage()
{
    if(draw_stage_ == 0)
        return false;
    --draw_stage_;
    return apply_draw_stage();
}

bool gfg::drawable_octal_triangles::apply_draw_stage()
{
    
    bind_vao();
    send_indexes_to_gpu();
    unbind_vao();
    
    elements_ = 3 * gfg::face::numberAtStage(draw_stage_);
    return true;
}

void gfg::drawable_octal_triangles::send_indexes_to_gpu()
{
    auto indexes = octa_.get_faces_index(draw_stage_);
    ebo_.send( indexes );
}

void gfg::drawable_octal_triangles::send_data_to_gpu()
{
    bind_vao();
    
    send_indexes_to_gpu();

    {
        gfg::position_vector tmp_pos;
        gfg::normal_vector tmp_norm;
        std::tie(tmp_pos, tmp_norm) = octa_.mesh().positions_and_normals();
        positions_.send(tmp_pos);
        normals_.send(tmp_norm);
    }

    {
        auto couleurs = octa_.mesh().colors();
        //send_data_to_vertex_buffer(couleurs, 1, GL_STATIC_DRAW);
        colors_.send(couleurs);
    }

    unbind_vao(); //unbind vertex array, not element buffer object

}

/////////////////////////////
// drawable_octal_hexagons //
/////////////////////////////
// gfg::drawable_octal_hexagons::drawable_octal_hexagons(gfg::fractal_octahedron& octa, unsigned int initial_draw_stage):
//     drawable_fractal_octahedron(octa, initial_draw_stage)
// {
//     send_data_to_gpu();
// }

// void gfg::drawable_octal_hexagons::send_data_to_gpu()
// {
//     bind_vao();
//     send_indexes_to_gpu();
// }

////////////////////////////
// implementation du cube //
////////////////////////////

gfg::cube::cube(GLfloat size):
    elements_drawable(14, GL_TRIANGLE_STRIP),
    positions_(gfg::gl::buffer_hint(0))
{
    std::array<GLfloat, 24> vertices{
         size/2,  size/2,  size/2,
        -size/2,  size/2,  size/2,
         size/2,  size/2, -size/2,
        -size/2,  size/2, -size/2,
         size/2, -size/2,  size/2,
        -size/2, -size/2,  size/2,
        -size/2, -size/2, -size/2,
        size/2, -size/2, -size/2 };
    std::array<GLuint, 14> elements{
        3, 2, 6, 7, 4, 2, 0,
        3, 1, 6, 5, 4, 1, 0 };


    bind_vao();
    ebo_.send(elements);
    positions_.send(vertices, 3);
    unbind_vao();

}
