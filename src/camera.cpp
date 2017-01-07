#include <iostream>
#include "containerOverload.h"
#include "camera.h"
#include <GLFW/glfw3.h>

gfg::camera::camera(glm::vec3 const& position)
    : m_position(position),
      m_front(glm::vec3(0.0f, 0.0f, -1.0f)),
      m_up(glm::vec3(0.0f, 1.0f,  0.0f))
{
    m_matrix = glm::lookAt(m_position, m_position + m_front, glm::vec3(0.0f, 1.0f, 0.0f));

}

void gfg::camera::update()
{
    m_matrix = glm::lookAt(m_position, m_position + m_front, m_up); 
}

std::unique_ptr<gfg::camera> gfg::camera::factory(const po::variables_map& vm)
{
    std::string optval = vm["spawn"].as<std::string>();
    glm::vec3 position;
    
    if(optval == "standard")
        position = {0.0, 0.0, 5.0};
    else if(optval == "eagle")
        position = {0, 5, 3};
    else
        throw parameter_exception("--spawn", {"standard", "eagle"}, optval);

    std::cout << optval << " ; " << position << std::endl;

    optval = vm["camera"].as<std::string>();
    if(optval == "south-north")
    {
        gfg::rotationSettings<GLfloat> sett{{0.0, 0.0, 0.0}, 1.2, 8.0};
        return std::make_unique<spinning_camera>
            (position, sett);
    }
    else if(optval == "fps")
        return std::make_unique<fps_camera>(position);

    throw parameter_exception("--camera", {"south-north", "fps"}, optval);
}

void gfg::fps_camera::move(movement dir, GLfloat delta)
{
    if(dir==FORWARD)
        m_position += delta * m_front;
    else if(dir==BACKWARD)
        m_position -= delta * m_front;
    else if(dir==LEFT)
        m_position -= glm::normalize(glm::cross(m_front, m_up)) * delta;
    else if(dir==RIGHT)
        m_position += glm::normalize(glm::cross(m_front, m_up)) * delta;
}

void gfg::fps_camera::orientate(double deltaX, double deltaY)
{
    m_yaw += deltaX;
    m_pitch -= deltaY;

    if(m_pitch > 89.0f)//généraliser
        m_pitch = 89.0f;
    else if(m_pitch < -89.0f)
        m_pitch = -89.0f;

    updateFront();
}

void gfg::fps_camera::updateFront()
{
    m_front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front.y = sin(glm::radians(m_pitch));
    m_front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(m_front);
}

void gfg::spinning_camera::move(movement dir, GLfloat delta)
{
    if(dir==FORWARD)
        m_rayon -= delta;
    else if(dir==BACKWARD)
        m_rayon += delta;
    else
        return;

    if(m_rayon < m_settings.min)
        m_rayon = m_settings.min;
    else if(m_rayon > m_settings.max)
        m_rayon = m_settings.max;
    refresh();
}

void gfg::spinning_camera::orientate(double deltaX, double deltaY)
{
    m_latitude+=deltaY;
    m_longitude+=deltaX;
    
    if(m_latitude > 89.0f)
        m_latitude = 89.0f;
    else if(m_latitude < -89.0f)
        m_latitude = -89.0f;

    refresh();
}

void gfg::spinning_camera::refresh()
{
    m_position.x = cos(glm::radians(m_longitude)) * cos(glm::radians(m_latitude))* m_rayon;
    m_position.y = sin(glm::radians(m_latitude)) * m_rayon;
    m_position.z = sin(glm::radians(m_longitude)) * cos(glm::radians(m_latitude))* m_rayon;
    m_front = -glm::normalize(m_position);
    m_position+= m_settings.pointOfFocus;
}
