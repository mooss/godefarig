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

#ifndef MOOSS_TRANSFORMATRIX_H
#define MOOSS_TRANSFORMATRIX_H

#include "myglad.h"

#include <string>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class Transformation
{
  public:
    Transformation(){}
    Transformation(const glm::mat4& matrix):
        matrix_(matrix)
    {}
    virtual ~Transformation(){}
    
    virtual void update()=0;

    glm::mat4 const& matrix() const {return matrix_;}
    const GLfloat* ptr() const {return glm::value_ptr(matrix_);}
    
  protected:
    glm::mat4 matrix_;

};

class Model : public Transformation
{
  public:
    Model();
    Model(const glm::mat4& matrix):
        Transformation(matrix)
    {}
    ~Model(){}

    void update() override;

  private:
    
};

class Projection : public Transformation
{
  public:
    Projection(GLuint, GLuint, GLfloat=45.0f, GLfloat=0.1f , GLfloat=100.0f );
    ~Projection(){}
    
    void update() override;
    
    GLfloat fov() const {return fov_;}
    GLfloat near() const {return near_;}
    GLfloat far() const {return far_;}

    GLfloat& fov() {return fov_;}
    GLfloat& near() {return near_;}
    GLfloat& far() {return far_;}

    Projection()=delete;
  private:
    GLuint displayWidth, displayHeight;
    GLfloat fov_, near_, far_;
};

#endif//MOOSS_TRANSFORMATRIX_H
