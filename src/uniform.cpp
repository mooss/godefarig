#include "uniform.h"
#include "shader.h"

gfg::TransformationUniforms(gfg::Shader& shader, const GLfloat* model_ptr, const GLfloat* view_ptr, const GLfloat* projection_ptr):
    m_model(shader.program(), "model", model_ptr),
    m_view(shader.program(), "view", view_ptr),
    m_projection(shader.program(), "projection", projection_ptr)
{}
