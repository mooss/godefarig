#include "transformatrix.h"

#include <iostream>

using namespace std;

//############### model ###############
Model::Model()
{
    update();
}

void Model::update()
{
    //temporaire
    //m_matrix = glm::rotate(m_matrix, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
}


//################# projection ##################

Projection::Projection(GLuint width, GLuint height, GLfloat fov, GLfloat near, GLfloat far)
    : displayWidth(width),
      displayHeight(height),
      m_fov(fov),
      m_near(near),
      m_far(far)
{
    update();
}

void Projection::update()
{
    m_matrix = glm::perspective(glm::radians(m_fov), static_cast<GLfloat>(displayWidth) / displayHeight, m_near, m_far);
}

