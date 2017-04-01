#include "graphics/uniform.h"
#include "container_overload.h"
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
    uniform_clonable<transformation_uniform>(name, shader_program),
    resource_(resource)
{
    DEBUG( name_ << "\n" << resource->matrix() << "\n");
    update();
}

uniformMat3f::uniformMat3f(const std::string& name, const glm::mat3& resource, GLuint shader_program):
    uniform_clonable<uniformMat3f>(name, shader_program),
    resource_(resource)
{
    DEBUG( name_ << "\n" << resource_ << "\n");
    update();
}

uniformVec3f::uniformVec3f(const std::string& name, const glm::vec3& resource, GLuint shader_program):
    uniform_clonable<uniformVec3f>(name, shader_program),
    resource_(resource)
{
    DEBUG( name_ << " " << resource_);
    update();
}
