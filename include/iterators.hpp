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

#ifndef MOOSS_ITERATORS_HPP
#define MOOSS_ITERATORS_HPP

#include "boost/iterator/iterator_facade.hpp"
#include "node.h"
#include <vector>

namespace gfg
{

class end_iterator_placeholder
{};

template<class Scalar, class Container, class Node_Type>
class clean_node_iterator : public boost::iterator_facade<
    clean_node_iterator<Scalar, Container, Node_Type>,//CRTP argument
    Node_Type,//value over which the iteration is done
    boost::forward_traversal_tag,//"type" of the iterator
    Scalar&,//type for the dereference operation
    std::ptrdiff_t >//type for the difference operation
{
  public:
    explicit clean_node_iterator(unsigned int stage):
        node_(0, 0, 6),
        target_(nullptr)
    {}
    
    clean_node_iterator() : clean_node_iterator(0){}
    clean_node_iterator(unsigned int stage, Container* target):
        node_(stage, 0, 0),
        target_(target)
    {}
        
  private:
    friend class boost::iterator_core_access;
    void increment() { node_.next(); }
    /**@note never use end-constructed iterator on the left hand side of a comparison. It is assumed that the lhs iterator was **constructed** as a valid iterator
     */
    bool equal(const clean_node_iterator& that) const
    {
        return (!that.target_ && node_.is_beyond_north())//end test
            || (node_ == that.node_);
    }

    Scalar& dereference() const
    {
        return target_->operator[](node_.index());
    }
    Node_Type node_;
    Container* target_;
};//class clean_node_iterator

}//namespace gfg

#endif//MOOSS_ITERATORS_HPP
