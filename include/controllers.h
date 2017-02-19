#ifndef MOOSS_CONTROLLERS_H
#define MOOSS_CONTROLLERS_H

#include "input_manager.h"
#include "camera.h"
#include "glToggle.h"
#include "drawable.h"

namespace gfg{
namespace control{

template<typename T>
struct stepped_value
{
    stepped_value(T val, T ste, T m, T M):
        value(val),
        step(ste),
        min(m),
        max(M)
    {}

    stepped_value<T> operator++()
    {
        if( value + step < max)
            value += step;
        return *this;
    }

    stepped_value<T> operator--()
    {
        if( value - step > min)
            value -= step;
        return *this;
    }
    
    T value;
    T step;
    T min;
    T max;
};

class camera_controller
{
  public:
    camera_controller(
    camera& cam,
    gfg::input::input_manager& input,
    gfg::input::key left,
    gfg::input::key right,
    gfg::input::key forward,
    gfg::input::key backward);
  private:
    camera& camera_;
    gfg::input::input_manager& input_;
    gfg::input::key left_;
    gfg::input::key right_;
    gfg::input::key forward_;
    gfg::input::key backward_;
    stepped_value<double> speed_;
    stepped_value<double> sensitivity_;
    
};

class gl_options_controller
{
  public:
    gl_options_controller(
        gfg::input::glSettings& sett,
        gfg::input::input_manager& input,
        gfg::input::key polygon_mode,
        gfg::input::key depth_test,
        gfg::input::key culled_face,
        gfg::input::key face_culling,
        gfg::input::key augment_primitive_size,
        gfg::input::key reduce_primitive_size);

  private:
    gfg::input::glSettings& settings_;
    gfg::input::input_manager& input_;
};

class draw_stage_controller
{
  public:
    draw_stage_controller(
        gfg::drawable_octal& octal,
        gfg::input::input_manager& input,
        gfg::input::key increment,
        gfg::input::key decrement
        );
  private:
    gfg::drawable_octal& octal_;
    gfg::input::input_manager& input_;
};

}//namespace control
}//namespace gfg

#endif /* MOOSS_CONTROLLERS_H */
