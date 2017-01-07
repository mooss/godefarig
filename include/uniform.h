#ifndef MOOSS_UNIFORM_H
#define MOOSS_UNIFORM_H


#include <string>
#include <functional>
#include "myglad.h"

template<typename unitype>
class Uniform
{
  public:
    Uniform(GLuint shader, std::string const& name, const unitype* ressource):
        m_ptr(ressource),
        m_name(name)
    {
        m_location = glGetUniformLocation(shader, m_name.c_str());
    }
    
    ~Uniform(){}

    GLuint location() const {return m_location;}
    const unitype* ptr() const {return m_ptr;}
    void reloadShader(GLuint newShader)
    {
        m_location = glGetUniformLocation(newShader, m_name.c_str());
        update();
    }
    
    virtual void update()=0;

    void setPtr(const unitype* newPtr)
    {
        m_ptr = newPtr;
    }

    Uniform()=delete;
  protected:
    
    GLint m_location;
    const unitype* m_ptr;
    std::string m_name;
};

class UniformMat4f : public Uniform<GLfloat>
{
  public:
    UniformMat4f(GLuint shader, std::string const& name, const GLfloat* ressource):
        Uniform(shader, name, ressource)
    {
        update();
    }

    void update() override//exception if no pointer is bound
    {
        glUniformMatrix4fv(m_location, 1, GL_FALSE, m_ptr);
    }

    UniformMat4f()=delete;
};

class UniformVec3f : public Uniform<GLfloat>
{
  public:
    UniformVec3f(GLuint shader, std::string const& name, const GLfloat* ressource):
        Uniform(shader, name, ressource)
    {
        update();
    }

    void update() override
    {
        glUniform3fv(m_location, 1, m_ptr);
    }

    UniformVec3f()=delete;
};


#endif
