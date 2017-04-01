#include "graphics/shading_unit.h"
#include "container_overload.h"

using namespace gfg::graphics;
////////////////////////////
// drawer implementations //
////////////////////////////

drawer::drawer(std::shared_ptr<drawable> target):
    target_(target)
{}

drawer_single::drawer_single( std::shared_ptr<drawable> target, std::shared_ptr<Model> model):
    drawer(target),
    model_{model}
{}

void drawer_single::draw( GLint location ) const
{
    glUniformMatrix4fv(location, 1, GL_FALSE, model_->ptr());
    target_->draw();
}

drawer_multi::drawer_multi( std::shared_ptr<drawable> target):
    drawer(target)
{}

void drawer_multi::draw( GLint location ) const
{
    for(auto& el : models_)
    {
        glUniformMatrix4fv(location, 1, GL_FALSE, el->ptr());
        target_->draw();
    }
}

void drawer_multi::add( std::shared_ptr<Model> model)
{
    models_.push_back(model);
}

/////////////////////////////////
// shading_unit implementation //
/////////////////////////////////

shading_unit::shading_unit(std::string shaderfile, std::shared_ptr<camera> view, std::shared_ptr<Projection> projection):
    shader_(shaderfile),
    view_("view", view, shader_.bind_and_return_program()),
    projection_("projection", projection, shader_.program()),
    model_location_( shader_.uniform_location("model"))
{
    DEBUG( "view of shading_unit :\n" << view->matrix() << "\n");
}

void shading_unit::draw() const
{
    shader_.bind();
    view_.update();
    for(auto& el : drawers_)
    {
        el->draw( model_location_ );
    }
}
