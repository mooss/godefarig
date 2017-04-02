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

#include "graphics/transformations.h"
#include <iostream>

using namespace std;

////////////////////
// Transformation //
////////////////////

Transformation::Transformation(const glm::mat4& matrix):
    matrix_(matrix)
{}

Transformation::Transformation(glm::mat4&& matrix):
    matrix_(std::move(matrix))
{}

///////////
// Model //
///////////
Model::Model(const glm::mat4& matrix):
    Transformation(matrix)
{}

Model::Model(glm::mat4&& matrix):
    Transformation(std::move(matrix))
{}

void Model::rotate(GLfloat angle, const glm::vec3& axes)
{
    matrix_ = glm::rotate(matrix_, glm::radians(angle), axes);
}


////////////////
// projection //
////////////////

Projection::Projection(GLuint width, GLuint height, GLfloat fov, GLfloat near, GLfloat far)
    : displayWidth(width),
      displayHeight(height),
      fov_(fov),
      near_(near),
      far_(far),
      fov_bounds_(20, 180)
{
    update();
}

void Projection::alter_fov(double step)
{
    fov_ += step;
    fov_bounds_.enforce(fov_);
}

void Projection::update()
{
    matrix_ = glm::perspective(glm::radians(fov_), static_cast<GLfloat>(displayWidth) / displayHeight, near_, far_);
}

//////////////////////////////
// vertex_and_normal_models //
//////////////////////////////

vertex_and_normal_models::vertex_and_normal_models(const glm::mat4& vertex_model):
    vertex_model_(vertex_model),
    normal_model_( glm::transpose(glm::inverse(glm::mat3(vertex_model_))))
{}

void vertex_and_normal_models::rotate(GLfloat angle, const glm::vec3& axes)
{
    vertex_model_ = glm::rotate(vertex_model_, glm::radians(angle), axes);
    normal_model_ = glm::transpose(glm::inverse(glm::mat3(vertex_model_)));
}

void vertex_and_normal_models::update_locations(const GLint* locations) const
{
    glUniformMatrix4fv(locations[0], 1, GL_FALSE, glm::value_ptr(vertex_model_));
    glUniformMatrix3fv(locations[1], 1, GL_FALSE, glm::value_ptr(normal_model_));
}
