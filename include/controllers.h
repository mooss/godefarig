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

#ifndef MOOSS_CONTROLLERS_H
#define MOOSS_CONTROLLERS_H

#include "input_manager.h"
#include "camera.h"
#include "glToggle.h"
#include "drawable.h"
#include "meta_utils.hpp"

namespace gfg{
namespace control{

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

//class fov_controller

}//namespace control
}//namespace gfg

#endif /* MOOSS_CONTROLLERS_H */
