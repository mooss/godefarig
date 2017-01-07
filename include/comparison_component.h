#ifndef MOOSS_COMPARISON_COMPONENT_H
#define MOOSS_COMPARISON_COMPONENT_H

#include "input_manager.h"
#include <vector>
#include <memory>

namespace gfg{
namespace input{

class comparison_component
{
  public:
    virtual bool conditions_are_met( const input_manager& comp_ref) const=0;

};

class comparison_leaf : public comparison_component
{
  public:
    comparison_leaf(pression_status status);
    
  protected:
    pression_status status_;
};

class button_comparison : public comparison_leaf
{
  public:
    button_comparison(button id, pression_status status);
    bool conditions_are_met( const input_manager& comp_ref) const override;

  private:
    button id_;
};

class key_comparison : public comparison_leaf
{
  public:
    key_comparison(key id, pression_status status);
    bool conditions_are_met( const input_manager& comp_ref) const override;
  private:
    key id_;
};

class comparison_composite : public comparison_component
{
  public:
    comparison_composite();
    bool conditions_are_met( const input_manager& comp_ref) const override;
    
    void add( std::shared_ptr<comparison_component> el);
    void remove( std::shared_ptr<comparison_component> el);
  private:
    std::vector< std::shared_ptr<comparison_component> > components_;
};

}//namespace input
}//namespace gfg

#endif /* MOOSS_COMPARISON_COMPONENT_H */
