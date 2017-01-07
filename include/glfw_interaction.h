#ifndef MOOSS_GLFW_INTERACTION_H
#define MOOSS_GLFW_INTERACTION_H

#include <GLFW/glfw3.h>

namespace gfg{
namespace input{

class input_manager;

void init_glfw_interaction(GLFWwindow* win, input_manager* manager);
void init_key_translation();
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void init_cursor_pos(GLFWwindow*);

}//namespace input
}//namespace gfg
#endif//MOOSS_GLFW_INTERACTION_H
