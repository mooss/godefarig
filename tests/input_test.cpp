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

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE input
#include <boost/test/unit_test.hpp>
#include "input_manager.h"
#include <iostream>

using namespace gfg::input;

input_manager default_constructed;

class observer_tester
{
  public:
    observer_tester():
        updated_(false)
    {}

    void operator()()
    {
        updated_ = true;
    }

    bool was_updated()
    {
        return updated_;
    }

    void reset()
    {
        updated_ = false;
    }
    
  private:
    bool updated_;
};

/**@brief checks if an input_manager is constructed with appropriates pressure options
 */
BOOST_AUTO_TEST_CASE(default_construction_pressure)
{
    bool inactive = true;
    for(unsigned int i = 0; i < static_cast<unsigned int>(key::size); ++i)
    {
        auto current = static_cast<key>(i);
        if(! default_constructed[current].is_inactive())
            inactive = false;

    }
    BOOST_CHECK(inactive);
}

/**@brief checks if pression changes are correctly recorded
 */
BOOST_AUTO_TEST_CASE(pressure_change)
{
    
    for(unsigned int i = 0; i < static_cast<unsigned int>(key::size); ++i)
    {
        auto current = static_cast<key>(i);
        default_constructed[current].press();
    }
    
    bool repeated = true;
    for(unsigned int i = 0; i < static_cast<unsigned int>(key::size); ++i)
    {
        auto current = static_cast<key>(i);
        if(!default_constructed[current].is_repeated())
        {
            repeated = false;
        }

    }
    BOOST_CHECK(repeated);
}

/**@brief checks if observers are notified of changes on their observed subject
 */
BOOST_AUTO_TEST_CASE(observer_update)
{
    observer_tester key_y;
    observer_tester key_z;

    default_constructed[key::y].attach(pression_status::pressed, std::ref(key_y));
    default_constructed[key::z].attach(pression_status::pressed, std::ref(key_z));
    default_constructed[key::y].press();

    BOOST_CHECK(key_y.was_updated());
    BOOST_CHECK(! key_z.was_updated());
}

/**@brief checks if the detachment of observers from their subject works as intended
 */
// BdOOST_AUTO_TEST_CASE(observer_detach)
// {
//     observer_tester key_y;
//     observer_tester key_z;

//     default_constructed.get_keyboard()[key::y].attach(key_y);
//     default_constructed.get_keyboard()[key::z].attach(key_z);
    
//     default_constructed.get_keyboard()[key::y].detach(key_y);
//     default_constructed.get_keyboard()[key::z].detach(key_y);//does nothing on purpose

//     default_constructed.get_keyboard()[key::y].press();
//     default_constructed.get_keyboard()[key::z].press();
    
//     BOOST_CHECK(! key_y.was_updated());
//     BOOST_CHECK(key_z.was_updated());
// }

/**@brief checks that a key pressed twice in a row is considered repeated
 */
BOOST_AUTO_TEST_CASE(pression_repetition)
{
    input_manager input;
    input[key::i].press();
    input[key::i].press();
    BOOST_CHECK( input[key::i].is_repeated());
}

//put this in pressure_device test
// /**@brief checks if a combination of option can be toggled
//  */
// BOOST_AUTO_TEShhT_CASE(double_toggle)
// {
//     observer_tester key_y;
//     observer_tester key_z;

//     input_manager input;

//     input[key::y].attach(key_y);
//     input[key::z].attach(key_z);
    
//     input.get_keyboard()[key::y].detach(key_y);
//     input.get_keyboard()[key::z].detach(key_y);//does nothing

//     input.get_keyboard()[key::y].toggle(pression_option::just);
//     input.get_keyboard()[key::z].toggle(pression_option::just);
    
//     BOOST_CHECK(! key_y.was_updated());
//     BOOST_CHECK(key_z.was_updated());
// }


