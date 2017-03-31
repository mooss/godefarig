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

#ifndef MOOSS_UNIFORM_H
#define MOOSS_UNIFORM_H


#include <string>
#include <functional>
#include "myglad.h"
#include "make_unique_cpp11.h"

template<typename unitype>
class Uniform
{
  public:
    Uniform(GLuint shader, std::string const& name, const unitype* ressource):
        ptr_(ressource),
        name_(name)
    {
        location_ = glGetUniformLocation(shader, name_.c_str());
    }
    
    ~Uniform(){}

    GLuint location() const {return location_;}
    const unitype* ptr() const {return ptr_;}
    void reloadShader(GLuint newShader)
    {
        location_ = glGetUniformLocation(newShader, name_.c_str());
        update();
    }
    
    virtual void update()=0;
    virtual std::unique_ptr<Uniform> clone() const=0;

    void setPtr(const unitype* newPtr)
    {
        ptr_ = newPtr;
    }

    Uniform()=delete;
  protected:
    
    GLint location_;
    const unitype* ptr_;
    std::string name_;
};

template<typename derived, typename unitype>
class clonable_Uniform : public Uniform<unitype>
{
  public:
    clonable_Uniform(GLuint shader, std::string const& name, const unitype* ressource):
        Uniform<unitype>(shader, name, ressource)
    {}

    std::unique_ptr<Uniform<unitype>> clone() const override
    {
        return std::make_unique<derived>( static_cast<const derived&>(*this));
    }

};

class UniformMat4f : public clonable_Uniform<UniformMat4f, GLfloat>
{
  public:
    UniformMat4f(GLuint shader, std::string const& name, const GLfloat* ressource):
        clonable_Uniform(shader, name, ressource)
    {
        update();
    }

    void update() override//exception if no pointer is bound
    {
        glUniformMatrix4fv(location_, 1, GL_FALSE, ptr_);
    }

    UniformMat4f()=delete;
};

class UniformMat3f : public clonable_Uniform<UniformMat3f, GLfloat>
{
  public:
    UniformMat3f(GLuint shader, std::string const& name, const GLfloat* ressource):
        clonable_Uniform(shader, name, ressource)
    {
        update();
    }

    void update() override//exception if no pointer is bound
    {
        glUniformMatrix3fv(location_, 1, GL_FALSE, ptr_);
    }

    UniformMat3f()=delete;
};

class UniformVec3f : public clonable_Uniform<UniformVec3f, GLfloat>
{
  public:
    UniformVec3f(GLuint shader, std::string const& name, const GLfloat* ressource):
        clonable_Uniform(shader, name, ressource)
    {
        update();
    }

    void update() override
    {
        glUniform3fv(location_, 1, ptr_);
    }

    UniformVec3f()=delete;
};


#endif
