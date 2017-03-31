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

#ifndef MOOSS_INPUT_MANAGER
#define MOOSS_INPUT_MANAGER

#include "input/observer.h"
#include "input/input_enums.h"
#include "gfg_utils.h"
#include "meta_utils.hpp"//flat_coordinates
#include <vector>
#include <array>
#include <tuple>

namespace gfg{
namespace input{

using mouse_coordinates = flat_coordinates<int>;
using scroll_coordinates = flat_coordinates<double>;

enum class pression_status : unsigned char
{
    pressed,
    repeated,
    released,
    inactive,
    SIZE//todo: use static assert or something like that to ensure that it is only used in the proper context
};

class observable_pressure_device
{
  public:
    using underlying_type = typename std::underlying_type<pression_status>::type;
    using observer_signal = typename boost::signals2::signal<void()>;

    observable_pressure_device();
    void press();
    void release();
    void repeat();

    
    //https://thehermeticvault.com/software-development/making-boost-signals2-more-oop-friendly
    template<typename Fun>
    boost::signals2::connection attach(pression_status condition, Fun&& obs)
    {
        return signals_[static_cast<underlying_type>(condition)]
            .connect(std::forward<Fun>(obs));
    }//todo: learn more about universal references and "perfect" forwarding and what they imply in this context

    bool is_pressed() const
    {
        return status_ == pression_status::pressed;
    }

    bool is_repeated() const
    {
        return status_ == pression_status::repeated;
    }

    bool is_inactive() const
    {
        return status_ == pression_status::inactive;
    }

    bool is_released() const
    {
        return status_ == pression_status::released;
    }

    pression_status status() const
    {
        return status_;
    }

    void notify();

  private:
    pression_status status_;
    std::array<
        observer_signal,
        static_cast<std::size_t>(pression_status::SIZE)
    > signals_;
};

class keyboard
{
  public:
    keyboard();
    observable_pressure_device& operator[](key id)
    {
        return keys_[static_cast<unsigned int>(id)];
    }
    
    const observable_pressure_device& operator[](key id) const
    {
        return keys_[static_cast<unsigned int>(id)];
    }

  private:
    std::vector<observable_pressure_device> keys_;
};//class keyboard

class mouse_scroll : public signal_subject
{
  public:
    mouse_scroll(){}
    void set_delta(const scroll_coordinates& delta);
    void set_vertical_delta(double vdelta);
    void set_horizontal_delta(double hdelta);

    const scroll_coordinates& delta() const { return delta_; }
    double vertical_delta() const { return delta_.y; }
    double horizontal_delta() const { return delta_.x; }

  private:
    scroll_coordinates delta_;
};

class mouse : public signal_subject
{
  public:
    mouse();
    int x() const { return position_.x; }
    int y() const { return position_.y; }
    
    observable_pressure_device& operator[](button id)
    {
        return buttons_[static_cast<unsigned int>(id)];
    }
    
    const observable_pressure_device& operator[](button id) const
    {
        return buttons_[static_cast<unsigned int>(id)];
    }

    const mouse_coordinates& position() const { return position_; }
    const mouse_coordinates& delta() const { return delta_; }
    mouse_scroll& scroll() { return scroll_; }
    const mouse_scroll& scroll() const { return scroll_; }
    const scroll_coordinates& scroll_delta() const { return scroll_.delta(); }
    double vertical_scroll() const { return scroll_.vertical_delta(); }
    double horizontal_scroll() const { return scroll_.horizontal_delta(); }

    void set_position(int x, int y);
    void set_scroll_delta(const scroll_coordinates& delta) { scroll_.set_delta(delta); }
    void set_vertical_scroll(double vdelta) { scroll_.set_vertical_delta(vdelta); }
    void set_horizontal_scroll(double hdelta) { scroll_.set_horizontal_delta(hdelta); }

    void update_delta();
    void reset_delta();
    
  private:
    std::vector<observable_pressure_device> buttons_;
    mouse_coordinates position_;
    mouse_coordinates last_position_;
    mouse_coordinates delta_;
    mouse_scroll scroll_;

};//class mouse

class comparison_component;

class input_manager
{
  public:
    keyboard& get_keyboard() { return keyboard_; }
    const keyboard& get_keyboard() const { return keyboard_; }

    mouse& get_mouse() { return mouse_; }
    const mouse& get_mouse() const { return mouse_; }

    /**@brief mouse button getter
     *@param id the desired button
     */
    observable_pressure_device& operator[](button id)
    {
        return mouse_[id];
    }
    
    /**@brief const mouse button getter
     *@param id the desired button
     */
    const observable_pressure_device& operator[](button id) const
    {
        return mouse_[id];
    }
    
    /**@brief keyboard key getter
     *@param id the desired key
     */
    observable_pressure_device& operator[](key id)
    {
        return keyboard_[id];
    }
    
    /**@brief const keyboard key getter
     *@param id the desired key
     */
    const observable_pressure_device& operator[](key id) const
    {
        return keyboard_[id];
    }

    void set_mouse_position(int x, int y) { mouse_.set_position(x, y); }
    void set_mouse_scroll_delta(const scroll_coordinates& delta) { mouse_.set_scroll_delta(delta); }
    void set_vertical_mouse_scroll(double vdelta) { mouse_.set_vertical_scroll(vdelta); }
    void set_horizontal_mouse_scroll(double hdelta) { mouse_.set_horizontal_scroll(hdelta); }

    void press(key id);
    void release(key id);
    void press(button id);
    void release(button id);
    void check();
    
    void update_deltas();
    void reset_deltas();

    const mouse_coordinates& position() const { return mouse_.position(); }
    double frame_delta() const { return time_handler_.delta(); }
    const mouse_coordinates& mouse_delta() const { return mouse_.delta(); }
    const scroll_coordinates& mouse_scroll_delta() const { return mouse_.scroll_delta(); }
    double vertical_mouse_scroll() const { return mouse_.vertical_scroll(); }
    double horizontal_mouse_scroll() const { return mouse_.horizontal_scroll(); }

    template<typename fun>
    boost::signals2::connection attach_to_mouse_position(fun&& obs)
    {
        return mouse_.attach( obs );//no forward because it does not seem to work with lambdas
    }

    template<typename fun>
    boost::signals2::connection attach_to_mouse_scroll(fun&& obs)
    {
        return mouse_.scroll().attach( obs );
    }

  private:
    keyboard keyboard_;
    mouse mouse_;
    std::vector<key> pressed_keys_;
    std::vector<button> pressed_buttons_;
    delta_time time_handler_;
};//class input_manager

}//namespace input
}//namespace gfg

#endif//MOOSS_INPUT_MANAGER
