#include <iostream>
#include "display.h"
#include <stdexcept>

using namespace std;


Display::Display(GLuint w, GLuint h, const string &title)
    : m_width(w),
      m_height(h),
      m_title(title),
      m_window(nullptr),
      m_monitor(nullptr)
{
    // Init GLFW
    glfwInit();
}

Display::Display(const string& title):
    m_title(title),
    m_window(nullptr)
{
    // Init GLFW
    glfwInit();
    m_monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* videoMode(glfwGetVideoMode(m_monitor));
    m_width = videoMode->width;
    m_height = videoMode->height;
}

Display::~Display()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Display::create()
{
    startContext();
    // Create a GLFWwindow object that we can use for GLFW's functions
    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), m_monitor, nullptr);    
    if (m_window == nullptr)
    {
        glfwTerminate();
        //throw exception
        throw runtime_error("glfw failed to create window");
    }
    glfwMakeContextCurrent(m_window);
    
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        throw runtime_error("glad failed to initialize OpenGL context");
    }

    // Define the viewport dimensions
    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);  
    glViewport(0, 0, width, height);
}

void Display::startContext()
{
    cout << "Starting GLFW context, OpenGL 3.3" << endl;
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
}
