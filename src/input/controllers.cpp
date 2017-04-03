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

#include "input/controllers.h"
#include "input/comparison_component.h"
#include "graphics/shader.h"

#include <iostream>

using namespace gfg::input;

gfg::control::camera_controller::camera_controller(
    graphics::shading_unit<vertex_and_normal_models>& planet,
    vertex_and_normal_models& planet_models,
    camera& camera,
    input_manager& input,
    key forward,
    key left,
    key backward,
    key right):

    input_(input),
    left_(left),
    right_(right),
    forward_(forward),
    backward_(backward),
    speed_(4, 0.6, 0.6, 10),
    sensitivity_(-0.2, 0, -0.2, -0.2)
{
    input_[left_].attach(pression_status::repeated,
        [&]()
        {
            camera.move(gfg::LEFT, input_.frame_delta() * speed_.value);
            planet.update_uniform("camera_position");
        });
    input_[right_].attach(pression_status::repeated,
        [&]()
        {
            camera.move(gfg::RIGHT, input_.frame_delta() * speed_.value);
            planet.update_uniform("camera_position");
        });
    input_[forward_].attach(pression_status::repeated,
        [&]()
        {
            camera.move(gfg::FORWARD, input_.frame_delta() * speed_.value);
            planet.update_uniform("camera_position");
        });
    input_[backward_].attach(pression_status::repeated,
        [&]()
        {
            camera.move(gfg::BACKWARD, input_.frame_delta() * speed_.value);
            planet.update_uniform("camera_position");
        });

    input_.attach_to_mouse_position(
        [&]()
        {
            
            if(input_[key::left_ctrl].is_inactive())
            {
                camera.orientate(input_.mouse_delta().x * sensitivity_.value,
                                 input_.mouse_delta().y * sensitivity_.value);
                planet.update_uniform("camera_position");
            }
            else
            {
                planet_models.rotate( static_cast<float>(input_.mouse_delta().x) * sensitivity_.value, glm::vec3(0, 1, 0));
                planet_models.rotate( -static_cast<float>(input_.mouse_delta().y) * sensitivity_.value, glm::vec3(0, 0, 1));
                planet.update_uniform("camera_position");
            }
        }
        );

    auto leaf = std::make_shared<key_comparison>(key::left_ctrl, pression_status::repeated);
    comparison_composite incr;
    incr.add(leaf);
    incr.add(std::make_shared<key_comparison>(key::n, pression_status::pressed));
    
    input_[key::n].attach(pression_status::pressed,
        [&, incr]()
        {
            if(incr.conditions_are_met(input_))
                ++speed_;
        });

    comparison_composite decr;
    decr.add(leaf);
    decr.add(std::make_shared<key_comparison>(key::b, pression_status::pressed));
    
    input_[key::b].attach(pression_status::pressed,
        [&, decr]()
        {
            if(decr.conditions_are_met(input_))
                --speed_;
        });
}

gfg::control::gl_options_controller::gl_options_controller(
        glSettings& sett,
        input_manager& input,
        key polygon_mode,
        key depth_test,
        key culled_face,
        key face_culling,
        key augment_primitive_size,
        key reduce_primitive_size):
    settings_(sett),
    input_(input)
{
    input_[polygon_mode].attach(
        pression_status::pressed,
        [&]()
        {
            settings_.toggle("polygonMode");
        });
    
    input_[depth_test].attach(
        pression_status::pressed,
        [&]()
        {
            settings_.toggle("depthTest");
        });
    
    input_[culled_face].attach(
        pression_status::pressed,
        [&]()
        {
            settings_.toggle("faceCulled");
        });
    
    input_[face_culling].attach(
        pression_status::pressed,
        [&]()
        {
            settings_.toggle("enableCullFace");
        });
    
    input_[augment_primitive_size].attach(
        pression_status::repeated,
        [&]()
        {
            settings_.changePrimitiveSize(true);
        });
    
    input_[reduce_primitive_size].attach(
        pression_status::repeated,
        [&]()
        {
            settings_.changePrimitiveSize(false);
        });
}//gl_options_controller cstr

gfg::control::draw_stage_controller::draw_stage_controller(
        gfg::drawable_octal& octal,
        input_manager& input,
        key increment,
        key decrement):
    octal_(octal),
    input_(input)
{
    input_[increment].attach(
        pression_status::pressed,
        [&]()
        {
            octal_.increment_draw_stage();
        });

    input_[decrement].attach(
        pression_status::pressed,
        [&]()
        {
            octal_.decrement_draw_stage();
        });
}

gfg::control::fov_controller::fov_controller(
        input_manager& input,
        Projection& proj,
        transformation_uniform& projection_uniform,
        gfg::Shader& shader):
    input_(input),
    projection_(proj),
    projection_uniform_(projection_uniform),
    shader_(shader)
{
    input_.attach_to_mouse_scroll(
        [&]()
        {
            projection_.alter_fov( input_.vertical_mouse_scroll()*5);
            projection_.update();
            shader_.bind();
            projection_uniform_.update();
            std::cout << "fov value : " << projection_.fov() << std::endl;
        });
}
