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
#include "gfg_utils.h"

template<typename unitype>
class Uniform
{
  public:
    Uniform(GLuint shader, std::string const& name, const unitype* resource):
        ptr_(resource),
        name_(name)
    {
        location_ = glGetUniformLocation(shader, name_.c_str());
    }
    
    ~Uniform(){}

    GLuint location() const {return location_;}
    const unitype* ptr() const {return ptr_;}
    void reload_shader(GLuint newShader)
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
    clonable_Uniform(GLuint shader, std::string const& name, const unitype* resource):
        Uniform<unitype>(shader, name, resource)
    {}

    std::unique_ptr<Uniform<unitype>> clone() const override
    {
        return std::make_unique<derived>( static_cast<const derived&>(*this));
    }

};

class UniformMat4f : public clonable_Uniform<UniformMat4f, GLfloat>
{
  public:
    UniformMat4f(GLuint shader, std::string const& name, const GLfloat* resource):
        clonable_Uniform(shader, name, resource)
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
    UniformMat3f(GLuint shader, std::string const& name, const GLfloat* resource):
        clonable_Uniform(shader, name, resource)
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
    UniformVec3f(GLuint shader, std::string const& name, const GLfloat* resource):
        clonable_Uniform(shader, name, resource)
    {
        update();
    }

    void update() override
    {
        glUniform3fv(location_, 1, ptr_);
    }

    UniformVec3f()=delete;
};

/////////////
// uniform //
/////////////

#include "graphics/shader.h"
#include "transformatrix.h"
#define DEBUG_MODE
#include "debug.h"

class uniform
{
  public:
    uniform(){}
    virtual std::unique_ptr<uniform> clone() const=0;
    virtual void update() const=0;
    virtual void reload_shader(GLuint)=0;
    virtual ~uniform(){}
    
    static std::unique_ptr<uniform> create(std::string const& name, std::shared_ptr<Transformation> resource, GLuint shader_program=0);
    static std::unique_ptr<uniform> create(std::string const& name, const glm::mat3& resource, GLuint shader_program=0);
    static std::unique_ptr<uniform> create(std::string const& name, const glm::vec3& resource, GLuint shader_program=0);
};

template<typename derived>
class uniform_clonable : public uniform
{
  public:
    uniform_clonable()=delete;
    uniform_clonable(std::string const& name, GLuint shader_program):
        name_(name)
    {
        location_ = glGetUniformLocation(shader_program, name_.c_str());
        DEBUG( "uniform " << name_ << " initialised to " << location_ << "by" << shader_program);
    }
    
    std::unique_ptr<uniform> clone() const override
    {
        return std::make_unique<derived>( static_cast<const derived&>(*this));
    }
    
    void reload_shader(GLuint shader_program) override
    {
        location_ = glGetUniformLocation(shader_program, name_.c_str());
        this->update();
    }
    
  protected:
    std::string name_;
    GLint location_;
};

class transformation_uniform : public uniform_clonable<transformation_uniform>
{
  public:
    transformation_uniform(const std::string& name, std::shared_ptr<Transformation> resource, GLuint shader_program=0);
    void update() const override//exception if no pointer is bound
    {
//        DEBUG( "updating " << name_ );
        glUniformMatrix4fv(location_, 1, GL_FALSE, resource_->ptr());
    }
  private:
    std::shared_ptr<Transformation> resource_;
};

class uniformMat3f : public uniform_clonable<uniformMat3f>
{
  public:
    uniformMat3f(std::string const& name, const glm::mat3& resource, GLuint shader_program=0);
    void update() const override//exception if no pointer is bound
    {
        glUniformMatrix3fv(location_, 1, GL_FALSE, glm::value_ptr(resource_));
    }
  private:
    glm::mat3 resource_;
};

class uniformVec3f : public uniform_clonable<uniformVec3f>
{
  public:
    uniformVec3f(std::string const& name, const glm::vec3& resource, GLuint shader_program=0);

    void update() const override
    {
        glUniform3fv(location_, 1, glm::value_ptr(resource_));
    }
  private:
    glm::vec3 resource_;
};

#endif
