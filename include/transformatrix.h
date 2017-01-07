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
        m_matrix(matrix)
    {}
    virtual ~Transformation(){}
    
    virtual void update()=0;

    glm::mat4 const& matrix() const {return m_matrix;}
    const GLfloat* ptr() const {return glm::value_ptr(m_matrix);}
    
  protected:
    glm::mat4 m_matrix;

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
    
    GLfloat fov() const {return m_fov;}
    GLfloat near() const {return m_near;}
    GLfloat far() const {return m_far;}

    GLfloat& fov() {return m_fov;}
    GLfloat& near() {return m_near;}
    GLfloat& far() {return m_far;}

    Projection()=delete;
  private:
    GLuint displayWidth, displayHeight;
    GLfloat m_fov, m_near, m_far;
};

#endif//MOOSS_TRANSFORMATRIX_H
