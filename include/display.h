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

#ifndef MOOSS_DISPLAY_H
#define MOOSS_DISPLAY_H

#include "myglad.h"
// GLFW
#include <GLFW/glfw3.h>
//#include "eventHandling.h"

class Display
{
  public:
    Display(GLuint, GLuint, const std::string&);
    Display(const std::string&);
    ~Display();
    void create();

    GLFWwindow* window() {return m_window;}
    GLuint width() const {return m_width;}
    GLuint height() const {return m_height;}
    Display()=delete;

  private:
    GLuint m_width, m_height;
    std::string m_title;
    GLFWwindow* m_window;
    GLFWmonitor *m_monitor;

    void startContext();
};

#endif//MOOSS_DISPLAY_H
