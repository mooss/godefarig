#ifndef MOOSS_SHADER_UNIT
#define MOOSS_SHADER_UNIT

#include "graphics/drawable.h"
#include "graphics/transformations.h"
#include "graphics/uniform.h"

namespace gfg{
namespace graphics{

/** \brief abstract class forming the base of classes providing a way to draw one or several objects
 */
class drawer
{
  public:
    drawer(std::shared_ptr<drawable> target):
        target_(target)
    {}
    drawer()=delete;
    virtual ~drawer(){}

    /** \brief draw the target
     *  \param locations location of the uniforms
     */
    virtual void draw( const GLint* locations) const=0;
  protected:
    std::shared_ptr<drawable> target_;
};

/** \brief can draw a target at a single location
 *  \tparam T type of the model
 */
template<typename T>
class drawer_single : public drawer
{
  public:
    drawer_single(std::shared_ptr<drawable> target, std::shared_ptr< T > model):
        drawer(target),
        models_{model}
    {}

    virtual void draw(const GLint* locations) const override
    {
        models_->update_locations(locations);
        target_->draw();
    }

  private:
    std::shared_ptr< T > models_;
};

/** \brief can draw a target at multiple locations
 *  \tparam T type of the model
*/
template<typename T>
class drawer_multi : public drawer
{
  public:
    drawer_multi(std::shared_ptr<drawable> target):
        drawer(target)
    {}

    virtual void draw(const GLint* locations) const override
    {
        for(auto& el : models_)
        {
            el->update_locations(locations);
            target_->draw();
        }
    }

    void add(std::shared_ptr<T> model)
    {
        models_.push_back(model);
    }


  private:
    std::vector< std::shared_ptr<T> > models_;
};

template<typename T>
class shading_unit
{
  public:
    shading_unit(std::string shaderfile, std::shared_ptr<camera> view, std::shared_ptr<Projection> projection, const std::array<std::string, T::required_size>& uniform_names):
        shader_(shaderfile),
        view_("view", view, shader_.bind_and_return_program()),
        projection_("projection", projection, shader_.program())
    {
        DEBUG( "view of shading_unit :\n" << view->matrix() << "\n");
        for(unsigned int i = 0; i < T::required_size; ++i)
            model_locations_[i] = shader_.uniform_location( uniform_names[i] );//todo: find a standard algorithm capable of mapping
    }

    void add_drawer( std::shared_ptr< drawer_single<T> > el) { drawers_.push_back(el); }
    void add_drawer( std::shared_ptr< drawer_multi<T> > el) { drawers_.push_back(el); }
    //todo: think about storing drawer_single and drawer_multi separately
    //todo: think about emplacing back drawers

    void add_uniform( std::unique_ptr<uniform> && unif)
    {
        shader_.bind();
        uniforms_.push_back( std::move(unif) );
        uniforms_.back()->reload_shader(shader_.program());
    }

    template<typename... Args>
    void add_uniform(Args&&... args)
    {
        shader_.bind();
        uniforms_.push_back( uniform::create(std::forward<Args>(args)...) );
        uniforms_.back()->reload_shader(shader_.program());
    }

    template<typename... Args>
    void add_static_uniform(Args&&... args)
    {
        uniforms_.push_back( uniform::create_static(std::forward<Args>(args)...) );
        uniforms_.back()->reload_shader(shader_.program());
    }

    bool update_uniform(std::string const& name)
    {
        for(auto& el: uniforms_)
            if(el->name() == name)
            {
                shader_.bind();
                el->update();
                return true;
            }
        std::cerr << "error: trying to update unknown uniform '" << name << "'" << std::endl;
        return false;
    }


    Shader& shader() { return shader_; }
    transformation_uniform& view() { return view_; }
    transformation_uniform& projection() { return projection_; }
    
    void draw() const
    {
        shader_.bind();
        view_.update();
        for(auto& el : drawers_)
        {
            el->draw( model_locations_.data() );
        }
    }
    
  private:
    Shader shader_;
    transformation_uniform view_;
    transformation_uniform projection_;
    std::array<GLint, T::required_size>  model_locations_;
    std::vector< std::shared_ptr<drawer> > drawers_;
    std::vector<std::unique_ptr<uniform>> uniforms_;
};

}//namespace graphics
}//namespace gfg
#endif//MOOSS_SHADER_UNIT
