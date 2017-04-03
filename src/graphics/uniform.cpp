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
    return std::make_unique<uniform_mat3f>(name, glm::value_ptr(resource), shader_program);
}

std::unique_ptr<uniform> uniform::create(std::string const& name, const glm::vec3& resource, GLuint shader_program)
{
    return std::make_unique<uniform_vec3f>(name, glm::value_ptr(resource), shader_program);
}

std::unique_ptr<uniform> uniform::create_static(std::string const& name, const glm::mat3& resource, GLuint shader_program)
{
    return std::make_unique<static_uniform_mat3f>(name, resource, shader_program);
}

std::unique_ptr<uniform> uniform::create_static(std::string const& name, const glm::vec3& resource, GLuint shader_program)
{
    return std::make_unique<static_uniform_vec3f>(name, resource, shader_program);
}

//////////////////////////////////
// constructors implementations //
//////////////////////////////////
uniform_mat3f::uniform_mat3f(const std::string& name, const GLfloat* resource, GLuint shader_program):
    uniform(name, shader_program),
    resource_(resource)
{
    conditional_update();
}

uniform_vec3f::uniform_vec3f(const std::string& name, const GLfloat* resource, GLuint shader_program):
    uniform(name, shader_program),
    resource_(resource)
{
    conditional_update();
}

transformation_uniform::transformation_uniform(const std::string& name, std::shared_ptr<Transformation> resource, GLuint shader_program):
    uniform(name, shader_program),
    resource_(resource)
{
    DEBUG( name_ << "\n" << resource->matrix() << "\n");
    conditional_update();
}

static_uniform_mat3f::static_uniform_mat3f(const std::string& name, const glm::mat3& resource, GLuint shader_program):
    uniform(name, shader_program),
    resource_(resource)
{
    DEBUG( name_ << "\n" << resource_ << "\n");
    conditional_update();
}

static_uniform_vec3f::static_uniform_vec3f(const std::string& name, const glm::vec3& resource, GLuint shader_program):
    uniform(name, shader_program),
    resource_(resource)
{
    DEBUG( name_ << " " << resource_);
    conditional_update();
}
