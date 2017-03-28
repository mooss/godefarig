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

#include "transformatrix.h"

#include <iostream>

using namespace std;

//############### model ###############
Model::Model()
{
    update();
}

void Model::update()
{
    //temporaire
    //matrix_ = glm::rotate(matrix_, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
}


//################# projection ##################

Projection::Projection(GLuint width, GLuint height, GLfloat fov, GLfloat near, GLfloat far)
    : displayWidth(width),
      displayHeight(height),
      fov_(fov),
      near_(near),
      far_(far)
{
    update();
}

void Projection::update()
{
    matrix_ = glm::perspective(glm::radians(fov_), static_cast<GLfloat>(displayWidth) / displayHeight, near_, far_);
}

