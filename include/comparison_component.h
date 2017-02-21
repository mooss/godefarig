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
