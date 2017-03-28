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

#ifndef MOOSS_CAMERA_H
#define MOOSS_CAMERA_H

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "transformatrix.h"
#include <memory>
#include "make_unique_cpp11.h"
#include "definitions.h"

namespace gfg
{

template<typename distanceType>
struct rotationSettings
{
    glm::vec3 pointOfFocus;
    distanceType min, max;
};

enum movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
};

class camera : public Transformation
{
  public:
    static std::unique_ptr<camera> factory(const po::variables_map& vm);
    
    camera(glm::vec3 const& position);
    camera()=delete;
    
    camera(const camera&)=delete;
    camera& operator=(camera const&)=delete;
    
    virtual ~camera(){}

    void update() override;
    virtual void move(gfg::movement, GLfloat)=0;
    virtual void orientate(double deltaX, double deltaY)=0;
//    void spinAroundCenter(double deltaX, double deltaY);

    // static std::unique_ptr<camera> fps(glm::vec3 const& position=glm::vec3(0.0f, 0.0f, 6.0f));
    // static std::unique_ptr<camera> rotating(glm::vec3 const& position, const rotationSettings<GLfloat>& sett);
    
  protected:
    glm::vec3 position_;
    glm::vec3 front_;
    glm::vec3 up_;

};

class fps_camera : public camera
{
  public:
    fps_camera(const glm::vec3& position):
        camera(position),
        pitch_(0.0f),
        yaw_(-90.0f)
    {}
    fps_camera()=delete;
    fps_camera(const fps_camera&)=delete;
    fps_camera& operator=(fps_camera const&)=delete;

    void move(gfg::movement, GLfloat) override;
    void orientate(double, double) override;

  private:
    void updateFront();//private ?
    GLfloat pitch_;
    GLfloat yaw_;

};

class spinning_camera : public camera//ajouter point of focus
{
  public:
    spinning_camera(const glm::vec3& position, const rotationSettings<GLfloat>& sett);

    spinning_camera(const spinning_camera&)=delete;
    spinning_camera& operator=(spinning_camera const&)=delete;

    void move(gfg::movement, GLfloat) override;
    void orientate(double, double) override;

  private:
    GLfloat latitude_;
    GLfloat longitude_;
    GLfloat rayon_;
    rotationSettings<GLfloat> settings_;
    
    void refresh();
};


}//namespace gfg

#endif//MOOSS_CAMERA_H
