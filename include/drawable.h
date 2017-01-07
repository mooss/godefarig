#ifndef MOOSS_DRAWABLE_HPP
#define MOOSS_DRAWABLE_HPP

#include "uniform.h"
//#include <initializer_list>
#include "camera.h"
#include <array>
#include <iostream>
#include "gfg_utils.h"

namespace gfg
{

class drawable
{
  public:
    virtual void draw()=0;
};

class Shader;
/**@class TransformationUniforms
 * represents the three transformations matrices requiered
 * by almost any vertex shader
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
    UniformMat4f& model() { return m_model; }
    /**@brief view getter
     */
    UniformMat4f& view() { return m_view; }
    /**@brief projection uniform
     */
    UniformMat4f& projection() { return m_projection; }
    
  private:
    TransformationUniforms(Shader& shader, const GLfloat* model_ptr, const GLfloat* view_ptr, const GLfloat* projection_ptr);
    
    UniformMat4f m_model;///< the model Uniform
    UniformMat4f m_view;///< the view Uniform
    UniformMat4f m_projection;///< the projection Uniform
};//class TransformationUniforms

class fractal_octahedron;

template<std::size_t vboSize>
class gl_drawable : public gfg::drawable
{
  public:
    gl_drawable(Model& mod):
        m_model(mod)
    {
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(vboSize, m_VBO.data());
    }

    virtual ~gl_drawable()
    {
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(vboSize, m_VBO.data());
    }

    virtual void draw()=0;
    
    Model& model(){return m_model;}

  protected:
    std::array<GLuint, vboSize> m_VBO;
    GLuint m_VAO;
    Model m_model;

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
        send_data_to_buffer(vect, GL_ARRAY_BUFFER, m_VBO[index], draw_mode);
    }//todo: a generic interface for color, position and such in order to no longer write glm specific instructions
        
};

template<size_t vboSize, GLenum mode>
class EBO_drawable : public gl_drawable<vboSize>
{
  public:
    EBO_drawable()=delete;//not sure if useful
    EBO_drawable(GLsizei elNbr, Model& mod):
        gl_drawable<vboSize>(mod),
        m_elements(elNbr)
    {
        glGenBuffers(1, &m_EBO);
    }
        
    ~EBO_drawable()
    {
        glDeleteBuffers(1, &m_EBO);
    }

    virtual void draw() override
    {
        glBindVertexArray(this->m_VAO);//something something two phase name loockup
        drawWithoutBinding();
        glBindVertexArray(0);
    }

    void drawWithoutBinding()//make virtual pure in superclass ? are VAOs multiple in case of several gl_drawable ?
    {
        glDrawElements(mode, m_elements, GL_UNSIGNED_INT, 0);//generalise later
    }

    
  protected:
    GLuint m_EBO;
    GLsizei m_elements;
    
    void send_data_to_element_buffer(const std::vector< std::array<gfg::index, 3> >& vect, GLenum draw_mode)
    {
        gl_drawable<vboSize>::send_data_to_buffer(vect, GL_ELEMENT_ARRAY_BUFFER, m_EBO, draw_mode);
    }

};

class drawable_octal : public EBO_drawable<2, GL_TRIANGLES>//todo: use GL_TRIANGLE_STRIP
{
  public:
    drawable_octal()=delete;
    // drawable_octal(unsigned int stage);
    drawable_octal(gfg::fractal_octahedron&, Model&&=Model());//prevoir coeff ctor. why the && ?
    
    ~drawable_octal(){}

    
    
  protected:
    gfg::fractal_octahedron& m_octa;
    void sendDataToGpu();
};

class Cube : public EBO_drawable<1, GL_TRIANGLE_STRIP>
{
  public:
    Cube(float size, Model&& mod=Model());
};

}

#endif//MOOSS_DRAWABLE_HPP
