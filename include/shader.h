#ifndef MOOSS_SHADER_H
#define MOOSS_SHADER_H

#include <string>
#include "myglad.h"

namespace gfg
{
class Shader
{
  public:
    Shader(const std::string& filename);
    Shader()=delete;
    ~Shader();
    //Shader(const Shader& source){}

    void bind();
    void load();

    GLuint program() {return m_program;}
    
  private:
    void operator=(const Shader& source){}

    std::string m_filename;
    static const unsigned int NUM_SHADERS = 2;
    GLuint m_program;
    GLuint m_shaders[NUM_SHADERS];
};

}//namespace gfg

#endif// MOOSS_SHADER_H
