#ifndef MOOSS_INPUT_MANAGER
#define MOOSS_INPUT_MANAGER

#include "observer.h"
#include "input_enums.h"
#include "gfg_utils.h"
#include <vector>
#include <array>
#include <tuple>

namespace gfg{
namespace input{

struct mouse_coordinates
{
    mouse_coordinates(int x_coord, int y_coord):
        x(x_coord),
        y(y_coord)
    {}

    mouse_coordinates() {}
    int x;
    int y;

    mouse_coordinates& operator+=(const mouse_coordinates& rhs);
    mouse_coordinates& operator-=(const mouse_coordinates& rhs);

    mouse_coordinates operator+(mouse_coordinates rhs) const;
    mouse_coordinates operator-(mouse_coordinates rhs) const;

    void reset();
};//struct mouse_coordinates
//todo: find a generic way to deal with n-dimentional scalar coordinates in order to ensure future interoperability

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

    const mouse_coordinates& position() const
    {
        return position_;
    }

    const mouse_coordinates& delta() const
    {
        return delta_;
    }

    void set_position(int x, int y);

    void update_delta();
    void reset_delta();
    
  private:
    std::vector<observable_pressure_device> buttons_;
    mouse_coordinates position_;
    mouse_coordinates last_position_;
    mouse_coordinates delta_;

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

    const mouse_coordinates& position() const
    {
        return mouse_.position();
    }

    void set_mouse_position(int x, int y)
    {
        mouse_.set_position(x, y);
    }

    void press(key id);
    void release(key id);
    void press(button id);
    void release(button id);
    void check();
    
    void update_deltas();
    void reset_deltas();

    double frame_delta()
    {
        return time_handler_.delta();
    }

    const mouse_coordinates& mouse_delta()
    {
        return mouse_.delta();
    }

    template<typename Fun>
    boost::signals2::connection attach_to_mouse_position(Fun&& obs)
    {
        return mouse_.attach(obs);
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
