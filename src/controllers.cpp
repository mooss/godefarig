#include "controllers.h"
#include "comparison_component.h"

#include <iostream>

using namespace gfg::input;

gfg::control::camera_controller::camera_controller(
    gfg::camera& cam,
    input_manager& input,
    key forward,
    key left,
    key backward,
    key right):

    camera_(cam),
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
            cam.move(gfg::LEFT, input_.frame_delta() * speed_.value);
        });
    input_[right_].attach(pression_status::repeated,
        [&]()
        {
            cam.move(gfg::RIGHT, input_.frame_delta() * speed_.value);
        });
    input_[forward_].attach(pression_status::repeated,
        [&]()
        {
            cam.move(gfg::FORWARD, input_.frame_delta() * speed_.value);
        });
    input_[backward_].attach(pression_status::repeated,
        [&]()
        {
            cam.move(gfg::BACKWARD, input_.frame_delta() * speed_.value);
        });

    input_.attach_to_mouse_position(
        [&]()
        {
            cam.orientate(input_.mouse_delta().x * sensitivity_.value,
                          input_.mouse_delta().y * sensitivity_.value);
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
}
