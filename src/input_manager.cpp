#include <algorithm>
#include "input_manager.h"

using namespace gfg::input;

observable_pressure_device::observable_pressure_device():
    status_(pression_status::inactive)
{}

void observable_pressure_device::press()
{
    status_ = pression_status::pressed;
    notify();
    status_ = pression_status::repeated;
}

void observable_pressure_device::release()
{
    status_ = pression_status::released;
    notify();
    status_ = pression_status::inactive;
}

void observable_pressure_device::repeat()
{
    if(status_ != pression_status::repeated)
        status_ = pression_status::repeated;
    notify();
}

void observable_pressure_device::notify()
{
    signals_[static_cast<std::size_t>(status_)]();
}


mouse_coordinates& mouse_coordinates::operator+=(const mouse_coordinates& rhs)
{
    x += rhs.x;
    y += rhs.y;
    return *this;
}

mouse_coordinates& mouse_coordinates::operator-=(const mouse_coordinates& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

mouse_coordinates mouse_coordinates::operator+(mouse_coordinates rhs) const
{
    rhs += *this;
    return rhs;
}

mouse_coordinates mouse_coordinates::operator-(mouse_coordinates rhs) const
{
    rhs -= *this;
    return rhs;
}

void mouse_coordinates::reset()
{
    x = 0;
    y = 0;
}

keyboard::keyboard():
    keys_(static_cast<unsigned int>(key::size))
{}

mouse::mouse():
    buttons_(static_cast<unsigned int>(button::size))
{}

// void mouse::set_position(int x, int y)
// {
//     positions_.get().x = x;
//     positions_.get().y = y;
// }

void mouse::set_position(int x, int y)
{
    position_.x = x;
    position_.y = y;
    update_delta();
    notify();
}


void mouse::update_delta()
{
    delta_ = position_ - last_position_;
    last_position_ = position_;
}

void mouse::reset_delta()
{
    delta_.reset();
    last_position_ = position_;
}

void input_manager::update_deltas()
{
    time_handler_.update();
    mouse_.update_delta();
}

void input_manager::reset_deltas()
{
    time_handler_.reset();
    mouse_.reset_delta();
}

void input_manager::press(key id)
{
    if(std::find(pressed_keys_.begin(), pressed_keys_.end(), id)
       == pressed_keys_.end() )
    {
        pressed_keys_.push_back(id);
        keyboard_[id].press();//applying first pression
    }
}

void input_manager::release(key id)
{
    auto it = std::find(pressed_keys_.begin(), pressed_keys_.end(), id);
    
    if( it != pressed_keys_.end() )
    {
        pressed_keys_.erase(it);
        keyboard_[id].release();//releasing the key
    }
}

void input_manager::press(button id)
{
    if(std::find(pressed_buttons_.begin(), pressed_buttons_.end(), id)
       == pressed_buttons_.end() )
    {
        pressed_buttons_.push_back(id);
        mouse_[id].press();//applying first pression
    }
}

void input_manager::release(button id)
{
    auto it = std::find(pressed_buttons_.begin(), pressed_buttons_.end(), id);
    
    if( it != pressed_buttons_.end() )
    {
        pressed_buttons_.erase(it);
        mouse_[id].release();
    }
}


void input_manager::check()
{
    for(auto& el : pressed_buttons_)
        mouse_[el].repeat();
    for(auto& el : pressed_keys_)
        keyboard_[el].repeat();
}
