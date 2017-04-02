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

#ifndef MOOSS_TRANSFORMATIONS_H
#define MOOSS_TRANSFORMATIONS_H

#include "myglad.h"

#include <string>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "meta_utils.hpp"//value_bounder
#include <array>

class Transformation
{
  public:
    Transformation(){}
    Transformation(const glm::mat4& matrix);
    Transformation(glm::mat4&& matrix);
    virtual ~Transformation(){}
    
    virtual void update()=0;

    const glm::mat4& matrix() const {return matrix_;}
    glm::mat4& matrix() { return matrix_; }
    const GLfloat* ptr() const {return glm::value_ptr(matrix_);}
    
  protected:
    glm::mat4 matrix_;
};

class Model : public Transformation
{
  public:
    Model(){}
    Model(const glm::mat4& matrix);
    Model(glm::mat4&& matrix);
    ~Model(){}

    void rotate(GLfloat angle, const glm::vec3& axes);
    void update() override {}//todo: remove update from transformation. perhaps updatable interface + multiple inheritance for proj and cam
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

    void alter_fov(double);

    Projection()=delete;
  private:
    GLuint displayWidth, displayHeight;
    GLfloat fov_, near_, far_;
    value_bounder<GLfloat> fov_bounds_;
};//Projection

class vertex_and_normal_models
{
  public:
    vertex_and_normal_models(){}
    vertex_and_normal_models(const glm::mat4& vertex_model);

    const glm::mat4& vertex_model() const { return vertex_model_; }
    glm::mat4& vertex_model() { return vertex_model_; }
    
    const glm::mat3& normal_model() const { return normal_model_; }
    glm::mat3& normal_model() { return normal_model_; }

    void rotate(GLfloat angle, const glm::vec3& axes);
    void update_locations(const GLint* locations) const;
    //it's up to the caller to ensure that locations will not provoque an out of bounds access (using required_size)
    static const std::size_t required_size = 2;

  private:
    glm::mat4 vertex_model_;
    glm::mat3 normal_model_;
};



#endif//MOOSS_TRANSFORMATIONS_H
