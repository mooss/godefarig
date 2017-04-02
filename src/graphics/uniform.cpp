#include "graphics/uniform.h"
#include "container_overload.h"
////////////////////////////
// uniform implementation //
////////////////////////////

uniform::uniform(std::string const& name, GLuint shader_program):
    name_(name)
{
    if(shader_program)
        location_ = glGetUniformLocation(shader_program, name_.c_str());
    else
        location_ = -1;
    DEBUG( "uniform " << name_ << " initialised to " << location_ << "by" << shader_program);
}


void uniform::reload_shader(GLuint shader_program)
{
    location_ = glGetUniformLocation(shader_program, name_.c_str());
    update();
}
/////////////////////
// factory methods //
/////////////////////
    
std::unique_ptr<uniform> uniform::create(std::string const& name, std::shared_ptr<Transformation> resource, GLuint shader_program)
{
    return std::make_unique<transformation_uniform>(name, resource, shader_program);
}

std::unique_ptr<uniform> uniform::create(std::string const& name, const glm::mat3& resource, GLuint shader_program)
{
    return std::make_unique<uniformMat3f>(name, resource, shader_program);
}

std::unique_ptr<uniform> uniform::create(std::string const& name, const glm::vec3& resource, GLuint shader_program)
{
    return std::make_unique<uniformVec3f>(name, resource, shader_program);
}

/////////////////////////////////
// constructors implementation //
/////////////////////////////////

transformation_uniform::transformation_uniform(const std::string& name, std::shared_ptr<Transformation> resource, GLuint shader_program):
    uniform(name, shader_program),
    resource_(resource)
{
    DEBUG( name_ << "\n" << resource->matrix() << "\n");
    update();
}

uniformMat3f::uniformMat3f(const std::string& name, const glm::mat3& resource, GLuint shader_program):
    uniform(name, shader_program),
    resource_(resource)
{
    DEBUG( name_ << "\n" << resource_ << "\n");
    update();
}

uniformVec3f::uniformVec3f(const std::string& name, const glm::vec3& resource, GLuint shader_program):
    uniform(name, shader_program),
    resource_(resource)
{
    DEBUG( name_ << " " << resource_);
    update();
}
