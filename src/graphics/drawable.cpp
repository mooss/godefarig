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

#include "fractahedron.h"
#include "geometry/hexagonal_iterator.h"
#include "container_overload.h"
#include "graphics/drawable.h"
#include "graphics/shader.h"
#include <cassert>

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
    elements_count_(elNbr),
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
    glDrawElements(mode_, elements_count_, GL_UNSIGNED_INT, 0);//todo: generalise (what, how ?)
}

/////////////////////////////////
// drawable_fractal_octahedron //
/////////////////////////////////
gfg::drawable_fractal_octahedron::drawable_fractal_octahedron(gfg::fractal_octahedron& octa, unsigned int initial_draw_stage, unsigned int element_count):
    elements_drawable(element_count, GL_TRIANGLES),
    octa_(octa),
    draw_stage_(initial_draw_stage),
    positions_(gfg::gl::vertex_buffer_hint(0)),
    colors_(gfg::gl::vertex_buffer_hint(1)),
    normals_(gfg::gl::vertex_buffer_hint(2))
{}

bool gfg::drawable_fractal_octahedron::increment_draw_stage()
{
    if(draw_stage_ == octa_.rank())
        return false;
    ++draw_stage_;
    return apply_draw_stage();
}

bool gfg::drawable_fractal_octahedron::decrement_draw_stage()
{
    if(draw_stage_ == 0)
        return false;
    --draw_stage_;
    return apply_draw_stage();
}

//////////////////////////////
// drawable_octal_triangles //
//////////////////////////////
gfg::drawable_octal_triangles::drawable_octal_triangles(gfg::fractal_octahedron& octa, unsigned int initial_draw_stage):
    drawable_fractal_octahedron(octa, initial_draw_stage, elements_count_at_stage(initial_draw_stage))
{
    send_data_to_gpu();
}

bool gfg::drawable_octal_triangles::apply_draw_stage()
{
    bind_vao();
    send_indexes_to_gpu();
    unbind_vao();
    
    elements_count_ = elements_count_at_stage(draw_stage_);
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

unsigned int gfg::drawable_octal_triangles::elements_count_at_stage(unsigned int stage)
{
    return 3 * gfg::face::number_at_stage(stage);//times 3 because there is 3 vertices per triangle
}

/////////////////////////////
// drawable_octal_hexagons //
/////////////////////////////
gfg::drawable_octal_hexagons::drawable_octal_hexagons(gfg::fractal_octahedron& octa, unsigned int initial_draw_stage):
    drawable_fractal_octahedron(octa, initial_draw_stage, elements_count_at_stage(initial_draw_stage))
{
    send_data_to_gpu();
}

void gfg::drawable_octal_hexagons::send_indexes_to_gpu()
{
    auto indexes = calculate_indexes();

    ebo_.send( indexes );
}

void gfg::drawable_octal_hexagons::send_data_to_gpu()
{
    bind_vao();
    send_indexes_to_gpu();

    {//in this block we retrieve the positions and normals, then we send them to the gpu and at the end of the scope, all allocated memory is freed
        gfg::position_vector tmp_pos;
        gfg::normal_vector tmp_norm;
        std::tie(tmp_pos, tmp_norm) = calculate_positions_and_normals();

        positions_.send(tmp_pos);
        normals_.send(tmp_norm);

    }

    {
        gfg::color_vector tmp_col(6*gfg::hexagon::number_at_stage(draw_stage_));

        for(auto colorit = tmp_col.begin(); colorit != tmp_col.end(); ++colorit)
        {
            *colorit = {0.05, 0.3, 0.7};
        }
        colors_.send(tmp_col);
    }
    unbind_vao();
//    elements_count_ = 36;
}

bool gfg::drawable_octal_hexagons::apply_draw_stage()
{
    send_data_to_gpu();
    elements_count_ = elements_count_at_stage(draw_stage_);
    return true;
}

unsigned int gfg::drawable_octal_hexagons::elements_count_at_stage(unsigned int stage)
{
    return 12 * gfg::hexagon::number_at_stage(stage);
    //                                   12
    // =                3                *                 4
    // = number_of_vertices_per_triangle * number_of_triangles_per_hexagon
}

inline glm::vec3 cartesian_from_latlong(
    double latitude,
    double longitude)
{
    return glm::vec3(
        sin(latitude) * cos(longitude),
        sin(latitude) * sin(longitude),
        cos(latitude) );
}

std::tuple<
    std::vector<gfg::position>,
    std::vector<gfg::normal>
    > gfg::drawable_octal_hexagons::calculate_positions_and_normals() const
{
    float distance_between_points = 1. / (pow(2, draw_stage_));

    //we reserve space for six points per hexagon
    std::vector<gfg::position> positions;
    positions.reserve( 6 * gfg::hexagon::number_at_stage(draw_stage_));
    std::vector<gfg::normal> normals;
    normals.reserve( 6 * gfg::hexagon::number_at_stage(draw_stage_));
    
    for(gfg::hexagonal_iterator hexit(octa_, draw_stage_);
        hexit != gfg::hexagonal_iterator::end();
        ++hexit)
    {
        const auto center_elevation = hexit->elevation();
        const float external_elevation =
            sqrt( (distance_between_points * distance_between_points) +
                  (center_elevation * center_elevation) );

        const gfg::normal shared_normal(
            - glm::normalize( cartesian_from_latlong(
                                hexit->center().latitude(),
                                hexit->center().longitude() )) );
        //cartesian_from_latlong should already return a normalised vec3 (or so I think)

        for(unsigned int i = 0; i < 6; ++i)
        {
            //we calculate and emplace back the position of an external node
            positions.emplace_back(
                cartesian_from_latlong(
                    hexit->external_node(i).latitude(),
                    hexit->external_node(i).longitude() ) * external_elevation );
            normals.emplace_back( shared_normal );
            //all external nodes of an hexagon share the same normal
        }
    }
    
    return std::make_tuple(positions, normals);
}

std::vector< std::array<unsigned int, 3> > gfg::drawable_octal_hexagons::calculate_indexes() const
{
    const unsigned int hexagon_number = gfg::hexagon::number_at_stage(draw_stage_);
    //there are 4 triangles per hexagons
    std::vector< std::array<unsigned int, 3> > result(4 * hexagon_number);

    for(unsigned int index = 0; index < hexagon_number; ++index)
    {//here we decompose each hexagon into four triangles
        const unsigned int triangle_index = 4 * index;
        const unsigned int node_index = 6 * index;

        //first triangle
        result[triangle_index][0] = node_index;
        result[triangle_index][1] = node_index+1;
        result[triangle_index][2] = node_index+2;

        //second triangle
        result[triangle_index+1][0] = node_index;
        result[triangle_index+1][1] = node_index+2;
        result[triangle_index+1][2] = node_index+3;

        //third triangle
        result[triangle_index+2][0] = node_index;
        result[triangle_index+2][1] = node_index+3;
        result[triangle_index+2][2] = node_index+4;
        
        //fourth triangle
        result[triangle_index+3][0] = node_index;
        result[triangle_index+3][1] = node_index+4;
        result[triangle_index+3][2] = node_index+5;
    }
    return result;
}

/////////////////////////
// cube implementation //
/////////////////////////
gfg::cube::cube(GLfloat size):
    elements_drawable(14, GL_TRIANGLE_STRIP),
    positions_(gfg::gl::vertex_buffer_hint(0))
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
