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

/** \brief abstract class destined to be the base for the model, view and projection matrices
 */
class Transformation
{
  public:
    Transformation(){}
    Transformation(const glm::mat4& matrix);
    Transformation(glm::mat4&& matrix);
    virtual ~Transformation(){}
    
    virtual void update()=0;

    /** \brief matrix getter (read-only version)
     *  \return the matrix
     */
    const glm::mat4& matrix() const {return matrix_;}
    
    /** \brief matrix getter (read-write version)
     *  \return the matrix
     */
    glm::mat4& matrix() { return matrix_; }

    /** \brief pointer getter
     *  \return the matrix as a pointer to float
     */
    const GLfloat* ptr() const {return glm::value_ptr(matrix_);}
    
  protected:
    glm::mat4 matrix_;/// transformation matrix
};

/** \brief model matrix
 */
class Model : public Transformation
{
  public:
    Model(){}
    Model(const glm::mat4& matrix);
    Model(glm::mat4&& matrix);
    ~Model(){}

    /** \brief rotate along an axe
     */
    void rotate(GLfloat angle, const glm::vec3& axes);
    void update() override {}//todo: remove update from transformation. simplify this shenanigan and get rid of the inheritance
};

/** \brief projection matrix
 */
class Projection : public Transformation
{
  public:
    /** \param width width of the window
     *  \param height height of the window
     *  \param fov value of the field of view
     *  \param near near value
     *  \param far far value
     */
    Projection(GLuint width, GLuint height, GLfloat fov=45.0f, GLfloat near=0.1f, GLfloat far=100.0f );
    ~Projection(){}
    
    void update() override;

    /** \brief field of view getter
     *  \return the field of view
     */
    GLfloat fov() const {return fov_;}

    /** \brief near value getter
     */
    GLfloat near() const {return near_;}

    /** \brief far value getter
     */
    GLfloat far() const {return far_;}

    /** \brief alter the fov
     *
     *  the fov is altered in the limit of the bounds set by fov_bounds_
     *
     * \param step value to add to the current fov
     */
    void alter_fov(double step);

    Projection()=delete;
  private:
    GLuint displayWidth;///width of the display
    GLuint displayHeight;///height of the display
    GLfloat fov_;///field of view
    GLfloat near_;///near value
    GLfloat far_;///far value
    value_bounder<GLfloat> fov_bounds_;///bounds of the fov (default : between 20 and 180)
};//Projection

/** \brief represents the vertex model and the normal model of a mesh
 */
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
