#include <iostream>
#include <array>
#include "glfw_interaction.h"
#include "input_manager.h"

using namespace std;

gfg::input::input_manager* inputs = nullptr;
std::array<gfg::input::key, GLFW_KEY_LAST+1> key_translation;

//grep -x "GLFW_KEY_[A-Z]" glfwkeynames  | sed 's/GLFW_KEY_\(.\)/key_translation[GLFW_KEY_\1] = gfg::input::key::\L\1;/'

void gfg::input::init_glfw_interaction(GLFWwindow* win, gfg::input::input_manager* manager)
{
    inputs = manager;
    init_cursor_pos(win);
    glfwSetKeyCallback(win, gfg::input::key_callback);
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//capturing the cursor and hiding it
    glfwSetCursorPosCallback(win, gfg::input::mouse_callback);
    init_key_translation();
}

void gfg::input::init_key_translation()
{
    for(auto& el : key_translation)
        el = gfg::input::key::unknown;

    key_translation[GLFW_KEY_A] = gfg::input::key::a;
    key_translation[GLFW_KEY_B] = gfg::input::key::b;
    key_translation[GLFW_KEY_C] = gfg::input::key::c;
    key_translation[GLFW_KEY_D] = gfg::input::key::d;
    key_translation[GLFW_KEY_E] = gfg::input::key::e;
    key_translation[GLFW_KEY_F] = gfg::input::key::f;
    key_translation[GLFW_KEY_G] = gfg::input::key::g;
    key_translation[GLFW_KEY_H] = gfg::input::key::h;
    key_translation[GLFW_KEY_I] = gfg::input::key::i;
    key_translation[GLFW_KEY_J] = gfg::input::key::j;
    key_translation[GLFW_KEY_K] = gfg::input::key::k;
    key_translation[GLFW_KEY_L] = gfg::input::key::l;
    key_translation[GLFW_KEY_M] = gfg::input::key::m;
    key_translation[GLFW_KEY_N] = gfg::input::key::n;
    key_translation[GLFW_KEY_O] = gfg::input::key::o;
    key_translation[GLFW_KEY_P] = gfg::input::key::p;
    key_translation[GLFW_KEY_Q] = gfg::input::key::q;
    key_translation[GLFW_KEY_R] = gfg::input::key::r;
    key_translation[GLFW_KEY_S] = gfg::input::key::s;
    key_translation[GLFW_KEY_T] = gfg::input::key::t;
    key_translation[GLFW_KEY_U] = gfg::input::key::u;
    key_translation[GLFW_KEY_V] = gfg::input::key::v;
    key_translation[GLFW_KEY_W] = gfg::input::key::w;
    key_translation[GLFW_KEY_X] = gfg::input::key::x;
    key_translation[GLFW_KEY_Y] = gfg::input::key::y;
    key_translation[GLFW_KEY_Z] = gfg::input::key::z;

    key_translation[GLFW_KEY_SPACE] = gfg::input::key::space;
    key_translation[GLFW_KEY_ENTER] = gfg::input::key::enter;
    key_translation[GLFW_KEY_LEFT_CONTROL] = gfg::input::key::left_ctrl;
    key_translation[GLFW_KEY_RIGHT_CONTROL] = gfg::input::key::right_ctrl;
    key_translation[GLFW_KEY_LEFT_SHIFT] = gfg::input::key::left_shift;
    key_translation[GLFW_KEY_RIGHT_SHIFT] = gfg::input::key::right_shift;
}

void gfg::input::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if(key >= 0)
    {
        if( action == GLFW_PRESS )
            inputs->press(key_translation[key]);
        else if(action == GLFW_RELEASE)
            inputs->release(key_translation[key]);
    }
}

void gfg::input::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    inputs->set_mouse_position(xpos, ypos);
}

void gfg::input::init_cursor_pos(GLFWwindow *win)
{
    double x, y;
    glfwGetCursorPos(win, &x, &y);
    inputs->set_mouse_position(x, y);
}
