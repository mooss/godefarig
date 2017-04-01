#ifndef MOOSS_SHADER_UNIT
#define MOOSS_SHADER_UNIT

#include "graphics/drawable.h"
#include "graphics/transformatrix.h"
#include "graphics/uniform.h"

namespace gfg{
namespace graphics{

class drawer
{
  public:
    drawer(std::shared_ptr<drawable> target);
    drawer()=delete;
    virtual ~drawer(){}

    virtual void draw(GLint location) const=0;
  protected:
    std::shared_ptr<drawable> target_;
};

class drawer_single : public drawer
{
  public:
    drawer_single(std::shared_ptr<drawable> target, std::shared_ptr<Model> model);
    virtual void draw(GLint location) const;

  private:
    std::shared_ptr<Model> model_;
};

class drawer_multi : public drawer
{
  public:
    drawer_multi(std::shared_ptr<drawable> target);
    virtual void draw(GLint location) const;
    void add(std::shared_ptr<Model> model);

  private:
    std::vector< std::shared_ptr<Model> > models_;
};

class shading_unit
{
  public:
    shading_unit(std::string shaderfile, std::shared_ptr<camera> view, std::shared_ptr<Projection> projection);

    void add_drawer( std::shared_ptr<drawer> el) { drawers_.push_back(el); }
    //todo: think about emplacing back drawers

    void add_uniform( std::unique_ptr<uniform> && unif)
    {
        optional_uniforms_.push_back( std::move(unif) );
        optional_uniforms_.back()->reload_shader(shader_.program());
    }

    template<typename... Args>
    void add_uniform(Args&&... args)
    {
        optional_uniforms_.push_back( uniform::create(std::forward<Args>(args)...) );
        optional_uniforms_.back()->reload_shader(shader_.program());
    }

    Shader& shader() { return shader_; }
    transformation_uniform& view() { return view_; }
    transformation_uniform& projection() { return projection_; }
    void draw() const;
  private:
    Shader shader_;
    transformation_uniform view_;
    transformation_uniform projection_;
    GLint model_location_;
    std::vector< std::shared_ptr<drawer> > drawers_;

    std::vector<std::unique_ptr<uniform>> optional_uniforms_;
};

}//namespace graphics
}//namespace gfg
#endif//MOOSS_SHADER_UNIT
