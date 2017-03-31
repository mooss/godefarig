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

/*todo:
make better shaderHelper functions
throw exception if check fails
*/
#include <fstream>
#include <iostream>
#include "graphics/shader.h"

using namespace std;

static bool checkShaderError(GLuint shader, GLuint flag, const string& errorMessage);
static bool checkProgramError(GLuint shader, GLuint flag, const string& errorMessage);
static string loadShader(const string& filename);//rename
static GLuint createShader(const string& shaderText, GLenum shaderType);

gfg::Shader::Shader(const string& filename) : filename_(filename)
{
    load();
}

gfg::Shader::~Shader()
{
    for(unsigned int i = 0; i < NUM_SHADERS; ++i)
    {
        glDetachShader(program_, shaders_[i]);
        glDeleteShader(shaders_[i]);//delete after loading (?)
    }
    glDeleteProgram(program_);
}

void gfg::Shader::bind()
{
    glUseProgram(program_);
}

void gfg::Shader::load()
{
    program_ = glCreateProgram();
    shaders_[0] = createShader(loadShader(filename_ + ".vs"), GL_VERTEX_SHADER);
    shaders_[1] = createShader(loadShader(filename_ + ".fs"), GL_FRAGMENT_SHADER);

    for(unsigned int i = 0; i < NUM_SHADERS; ++i)
        glAttachShader(program_, shaders_[i]);

    glBindAttribLocation(program_, 0, "position");
    
    glLinkProgram(program_);
    checkProgramError(program_, GL_LINK_STATUS, "shader program failed to link");

    glValidateProgram(program_);
    checkProgramError(program_, GL_VALIDATE_STATUS, "shader program validation failed");
}

bool checkShaderError(GLuint shader, GLuint flag, const string& errorMessage)
{
    GLint success = 0;

    glGetShaderiv(shader, flag, &success);

    if(success == GL_FALSE)
    {
        GLchar error[1024] = {0};
        glGetShaderInfoLog(shader, sizeof(error), NULL, error);
        cerr << errorMessage << " : '" << error << "'" << endl;
    }

    return success;
}

bool checkProgramError(GLuint shader, GLuint flag, const string& errorMessage)
{
    GLint success = 0;

    glGetProgramiv(shader, flag, &success);

    if(success == GL_FALSE)
    {
        GLchar error[1024] = {0};
        glGetProgramInfoLog(shader, sizeof(error), NULL, error);
        cerr << errorMessage << ": '" << error << "'" << endl;
    }

    return success;
}

string loadShader(const string& filename)//vérifier si les méthodes utilisées sont "à jour"
{
    ifstream file;
    file.open(filename.c_str());

    string output, line;

    if(file.is_open())
    {
        while(file.good())
        {
            getline(file, line);
            output.append(line+"\n");
        }
    }
    else
    {
        cerr << "unable to load shader : " << filename << endl;
    }

    return output;
}


GLuint createShader(const string& shaderText, GLenum shaderType)
{
    int sourceLines = 1;
    GLuint shader = glCreateShader(shaderType);

    if(shader == 0)
        cerr << "shader creation failed" << endl;

    const GLchar* shaderSource[sourceLines];
    GLint shaderSourceLength[sourceLines];
    
    shaderSource[0] = shaderText.c_str();
    shaderSourceLength[0] = shaderText.length();

    glShaderSource(shader, sourceLines, shaderSource, shaderSourceLength);
    glCompileShader(shader);

    checkShaderError(shader, GL_COMPILE_STATUS, "shader compilation failed");
    
    return shader;
}
