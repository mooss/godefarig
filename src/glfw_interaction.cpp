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

#include <iostream>
#include <array>
#include "glfw_interaction.h"
#include "input_manager.h"

using namespace std;

gfg::input::input_manager* inputs = nullptr;
std::array<gfg::input::key, GLFW_KEY_LAST+1> key_translation;
std::array<gfg::input::button, GLFW_MOUSE_BUTTON_LAST> button_translation;

//grep -x "GLFW_KEY_[A-Z]" glfwkeynames  | sed 's/GLFW_KEY_\(.\)/key_translation[GLFW_KEY_\1] = gfg::input::key::\L\1;/'

void gfg::input::init_glfw_interaction(GLFWwindow* win, gfg::input::input_manager* manager)
{
    inputs = manager;
    init_cursor_pos(win);
    init_key_translation();
    init_button_translation();
    
    glfwSetKeyCallback(win, gfg::input::key_callback);
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//capturing the cursor and hiding it
    glfwSetCursorPosCallback(win, gfg::input::mouse_callback);
    glfwSetMouseButtonCallback(win, gfg::input::mouse_button_callback);
    glfwSetScrollCallback(win, gfg::input::scroll_callback);
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

void gfg::input::init_button_translation()
{
    for(auto& el : button_translation)
        el = gfg::input::button::unknown;

    button_translation[GLFW_MOUSE_BUTTON_MIDDLE] = gfg::input::button::middle;
    button_translation[GLFW_MOUSE_BUTTON_LEFT] = gfg::input::button::left;
    button_translation[GLFW_MOUSE_BUTTON_RIGHT] = gfg::input::button::right;
    button_translation[GLFW_MOUSE_BUTTON_1] = gfg::input::button::special_1;
    button_translation[GLFW_MOUSE_BUTTON_2] = gfg::input::button::special_2;
    button_translation[GLFW_MOUSE_BUTTON_3] = gfg::input::button::special_3;
    button_translation[GLFW_MOUSE_BUTTON_4] = gfg::input::button::special_4;
    button_translation[GLFW_MOUSE_BUTTON_5] = gfg::input::button::special_5;
    button_translation[GLFW_MOUSE_BUTTON_6] = gfg::input::button::special_6;
    button_translation[GLFW_MOUSE_BUTTON_7] = gfg::input::button::special_7;
    button_translation[GLFW_MOUSE_BUTTON_8] = gfg::input::button::special_8;

}

void gfg::input::init_cursor_pos(GLFWwindow *win)
{
    double x, y;
    glfwGetCursorPos(win, &x, &y);
    inputs->set_mouse_position(x, y);
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

void gfg::input::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if(button >= 0)
    {
        if(action == GLFW_PRESS)
            inputs->press(button_translation[button]);
        else if(action == GLFW_RELEASE)
            inputs->release(button_translation[button]);
    }
}

void gfg::input::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    inputs->set_mouse_scroll_delta( flat_coordinates<double>(xoffset, yoffset));
}
