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

#include <iostream>
#include "containerOverload.h"
#include "camera.h"
#include <GLFW/glfw3.h>

gfg::camera::camera(glm::vec3 const& position)
    : position_(position),
      front_(glm::vec3(0.0f, 0.0f, -1.0f)),
      up_(glm::vec3(0.0f, 1.0f,  0.0f))
{
    matrix_ = glm::lookAt(position_, position_ + front_, glm::vec3(0.0f, 1.0f, 0.0f));

}

void gfg::camera::update()
{
    matrix_ = glm::lookAt(position_, position_ + front_, up_); 
}

std::unique_ptr<gfg::camera> gfg::camera::factory(const po::variables_map& vm)
{
    std::string optval = vm["spawn"].as<std::string>();
    glm::vec3 position;
    
    if(optval == "standard")
        position = {0.0, 0.0, 5.0};
    else if(optval == "eagle")
        position = {0, 5, 3};
    else
        throw parameter_exception("--spawn", {"standard", "eagle"}, optval);

    std::cout << optval << " ; " << position << std::endl;

    optval = vm["camera"].as<std::string>();
    if(optval == "south-north")
    {
        gfg::rotationSettings<GLfloat> sett{{0.0, 0.0, 0.0}, 1.2, 8.0};
        return std::make_unique<spinning_camera>
            (position, sett);
    }
    else if(optval == "fps")
        return std::make_unique<fps_camera>(position);

    throw parameter_exception("--camera", {"south-north", "fps"}, optval);
}

void gfg::fps_camera::move(movement dir, GLfloat delta)
{
    if(dir==FORWARD)
        position_ += delta * front_;
    else if(dir==BACKWARD)
        position_ -= delta * front_;
    else if(dir==LEFT)
        position_ -= glm::normalize(glm::cross(front_, up_)) * delta;
    else if(dir==RIGHT)
        position_ += glm::normalize(glm::cross(front_, up_)) * delta;
}

void gfg::fps_camera::orientate(double deltaX, double deltaY)
{
    yaw_ += deltaX;
    pitch_ -= deltaY;

    if(pitch_ > 89.0f)//généraliser
        pitch_ = 89.0f;
    else if(pitch_ < -89.0f)
        pitch_ = -89.0f;

    updateFront();
}

void gfg::fps_camera::updateFront()
{
    front_.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front_.y = sin(glm::radians(pitch_));
    front_.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front_ = glm::normalize(front_);
}

/////////////////////
// spinning_camera //
/////////////////////


gfg::spinning_camera::spinning_camera(const glm::vec3& position, const gfg::rotationSettings<GLfloat>& sett):
    camera(position),
    latitude_(0.0),
    longitude_(0.0),
    rayon_(glm::length(position)),
    settings_(sett)
{
    refresh();
}

void gfg::spinning_camera::move(movement dir, GLfloat delta)
{
    if(dir==FORWARD)
        rayon_ -= delta;
    else if(dir==BACKWARD)
        rayon_ += delta;
    else
        return;

    if(rayon_ < settings_.min)
        rayon_ = settings_.min;
    else if(rayon_ > settings_.max)
        rayon_ = settings_.max;
    refresh();
}

void gfg::spinning_camera::orientate(double deltaX, double deltaY)
{
    latitude_+=deltaY;
    longitude_+=deltaX;
    
    if(latitude_ > 89.0f)
        latitude_ = 89.0f;
    else if(latitude_ < -89.0f)
        latitude_ = -89.0f;

    refresh();
}

void gfg::spinning_camera::refresh()
{
    position_.x = cos(glm::radians(longitude_)) * cos(glm::radians(latitude_))* rayon_;
    position_.y = sin(glm::radians(latitude_)) * rayon_;
    position_.z = sin(glm::radians(longitude_)) * cos(glm::radians(latitude_))* rayon_;
    front_ = -glm::normalize(position_);
    position_+= settings_.pointOfFocus;
}
