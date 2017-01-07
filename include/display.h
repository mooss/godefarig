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
