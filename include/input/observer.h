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

#ifndef MOOSS_OBSERVER_H
#define MOOSS_OBSERVER_H

#include <vector>
#include <functional>
#include <boost/signals2.hpp>

namespace gfg
{

// class observer
// {
//   public:
//     /**@brief method used by the subject to inform an observer that a change occured
//      */
//     virtual void update()=0;//todo: think about crtp.
//     virtual ~observer(){}
// };

// class subject
// {
//   public:
//     virtual ~subject(){}
//     /**@brief adds an observer
//      *@param obs the observer to add
//      */
//     virtual void attach(std::function<void()> obs)=0;

//     /**@brief notifies the observers of a change in the subject
//      */
//     virtual void notify()=0;
// };

class signal_subject
{
  public:

    template<typename Fun>
    boost::signals2::connection attach( Fun&& obs)
    {
        return observers_.connect(std::forward<Fun>(obs));
    }

    void notify() { observers_(); }

  private:
    boost::signals2::signal<void()> observers_;
};

}//namespace gfg

#endif//MOOSS_OBSERVER_H
