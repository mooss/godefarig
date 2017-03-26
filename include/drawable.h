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

#ifndef MOOSS_DRAWABLE_HPP
#define MOOSS_DRAWABLE_HPP

#include "graphics/buffer.h"
#include "uniform.h"
//#include <initializer_list>
#include "camera.h"
#include <array>
#include <iostream>


namespace gfg
{

class drawable
{
  public:
    virtual ~drawable(){};
    virtual void draw()=0;
};

class Shader;
/**@class TransformationUniforms
 * represents the three transformations matrices usefull for vertex shaders
 */
class TransformationUniforms
{
  public:
    /**@brief static factory
     * @param shader the Shader to which the uniforms will be send
     * @param model_ptr a pointer to the model matrix
     * @param view_ptr a pointer to the view matrix
     * @param projection_ptr a pointer to the projection matrix
     * ensures that the shader is bound before instanciating the uniforms
     */
    static TransformationUniforms create(
        Shader& shader,
        const GLfloat* model_ptr,
        const GLfloat* view_ptr,
        const GLfloat* projection_ptr );//todo: think about shared pointers for this task

    /**@brief model getter
     */
    UniformMat4f& model() { return model_; }
    /**@brief view getter
     */
    UniformMat4f& view() { return view_; }
    /**@brief projection uniform
     */
    UniformMat4f& projection() { return projection_; }
    
  private:
    TransformationUniforms(Shader& shader, const GLfloat* model_ptr, const GLfloat* view_ptr, const GLfloat* projection_ptr);
    
    UniformMat4f model_;///< the model Uniform
    UniformMat4f view_;///< the view Uniform
    UniformMat4f projection_;///< the projection Uniform
};//class TransformationUniforms

class fractal_octahedron;

template<std::size_t vboSize>
class gl_drawable : public gfg::drawable
{
  public:
    gl_drawable(Model& mod):
        model_(mod)
    {
        glGenVertexArrays(1, &VAO_);
        glGenBuffers(vboSize, VBO_.data());
    }

    virtual ~gl_drawable()
    {
        glDeleteVertexArrays(1, &VAO_);
        glDeleteBuffers(vboSize, VBO_.data());
    }

    virtual void draw()=0;
    
    Model& model(){return model_;}

  protected:
    std::array<GLuint, vboSize> VBO_;
    GLuint VAO_;
    Model model_;

    void send_data_to_buffer(
        const std::vector<glm::vec3>& vect,
        GLenum buffer_type,
        GLuint buffer_id,
        GLenum draw_mode )
    {
        glBindBuffer(buffer_type, buffer_id);
        glBufferData(
            buffer_type,
            sizeof(vect.front()) * vect.size(),
            glm::value_ptr(vect.front()),
            draw_mode );
    }

    void send_data_to_buffer(
        const std::vector< std::array<gfg::index, 3> > & vect,
        GLenum buffer_type,
        GLuint buffer_id,
        GLenum draw_mode )
    {
        glBindBuffer(buffer_type, buffer_id);
        glBufferData(
            buffer_type,
            sizeof(vect.front()) * vect.size(),
            vect.front().data(),
            draw_mode );
    }//todo: use traits to provide a generic method to acces to the pointer of such a structure

    
    template<class container>
    void send_data_to_vertex_buffer(const container& vect, std::size_t index, GLenum draw_mode)
    {
        send_data_to_buffer(vect, GL_ARRAY_BUFFER, VBO_[index], draw_mode);
    }//todo: a generic interface for color, position in order to no longer write glm specific instructions
        
};//class gl_drawable

template<size_t vboSize, GLenum mode>
class EBO_drawable : public gl_drawable<vboSize>
{
  public:
    EBO_drawable()=delete;//not sure if useful
    EBO_drawable(GLsizei elNbr, Model& mod):
        gl_drawable<vboSize>(mod),
        elements_(elNbr)
    {
        glGenBuffers(1, &EBO_);
    }
        
    ~EBO_drawable()
    {
        glDeleteBuffers(1, &EBO_);
    }

    virtual void draw() override
    {
        glBindVertexArray(this->VAO_);//something something two phase name loockup
        draw_without_binding();
        glBindVertexArray(0);
    }

    void draw_without_binding()//make virtual pure in superclass ? are VAOs multiple in case of several gl_drawable ?
    {
        glDrawElements(mode, elements_, GL_UNSIGNED_INT, 0);//generalise later
    }

    
  protected:
    GLuint EBO_;
    GLsizei elements_;
    
    void send_data_to_element_buffer(const std::vector< std::array<gfg::index, 3> >& vect, GLenum draw_mode)
    {
        gl_drawable<vboSize>::send_data_to_buffer(vect, GL_ELEMENT_ARRAY_BUFFER, EBO_, draw_mode);
    }

};

class drawable_octal : public EBO_drawable<2, GL_TRIANGLES>//todo: use GL_TRIANGLE_STRIP
{
  public:
    drawable_octal()=delete;
    // drawable_octal(unsigned int stage);
    drawable_octal(gfg::fractal_octahedron&, Model&&=Model());//prevoir coeff ctor. why the && ?
    
    ~drawable_octal(){}
    bool increment_draw_stage();
    bool decrement_draw_stage();
    
    
  protected:
    gfg::fractal_octahedron& octa_;
    unsigned int draw_stage_;
    void sendDataToGpu();
    bool apply_draw_stage();
};

class Cube : public EBO_drawable<1, GL_TRIANGLE_STRIP>
{
  public:
    Cube(float size, Model&& mod=Model());
};

}//namespace gfg

#endif//MOOSS_DRAWABLE_HPP
