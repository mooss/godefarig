#include "drawable.h"
#include <cassert>
#include "fractahedron.h"
#include "containerOverload.h"
#include "shader.h"

gfg::TransformationUniforms::TransformationUniforms(Shader& shader,
                       const GLfloat* model_ptr,
                       const GLfloat* view_ptr,
                       const GLfloat* projection_ptr ):
    m_model(shader.program(), "model", model_ptr),
    m_view(shader.program(), "view", view_ptr),
    m_projection(shader.program(), "projection", projection_ptr)
{}

gfg::TransformationUniforms gfg::TransformationUniforms::create(
    Shader& shader,
    const GLfloat* model_ptr,
    const GLfloat* view_ptr,
    const GLfloat* projection_ptr )
{
    shader.bind();
    return gfg::TransformationUniforms(shader, model_ptr, view_ptr, projection_ptr);
}

gfg::drawable_octal::drawable_octal(gfg::fractal_octahedron& octa, Model&& mod):
    EBO_drawable(3*octa.face_cardinal(), mod),
    m_octa(octa),
    m_draw_stage(m_octa.rank())
{
    sendDataToGpu();
}

bool gfg::drawable_octal::increment_draw_stage()
{
    ++m_draw_stage;
    return apply_draw_stage();
}

bool gfg::drawable_octal::decrement_draw_stage()
{
    --m_draw_stage;
    return apply_draw_stage();
}

bool gfg::drawable_octal::apply_draw_stage()
{
    if(m_draw_stage < 1)
    {
        m_draw_stage = 1;
        return false;
    }
    if(m_draw_stage > m_octa.rank())
    {
        m_draw_stage = m_octa.rank();
        return false;
    }
    
    m_elements = 3 * gfg::face::numberAtStage(m_draw_stage);
    return true;
}

void gfg::drawable_octal::sendDataToGpu()
{
    glBindVertexArray(m_VAO);
    
    {
        auto indexes = m_octa.getFacesIndex();
        send_data_to_element_buffer(indexes, GL_STATIC_DRAW);
    }

    {
        auto positions = m_octa.mesh().positions();
        send_data_to_vertex_buffer(positions, 0, GL_STATIC_DRAW);
    }
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          3 * sizeof(GLfloat), (GLvoid*)0 );
    glEnableVertexAttribArray(0);

    //sending the colors
    {
        auto couleurs = m_octa.mesh().colors();
        send_data_to_vertex_buffer(couleurs, 1, GL_STATIC_DRAW);
    }
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          3 * sizeof(GLfloat), (GLvoid*)0 );
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); //unbind vertex array, not element buffer object

}

gfg::Cube::Cube(float size, Model&& mod):
    EBO_drawable(14, mod)
{
    static GLfloat vertices[] =
    { 
        size/2,	size/2,	size/2,
        -size/2,	size/2,	size/2,
        size/2,	size/2,	-size/2,
        -size/2,	size/2,	-size/2,
        size/2,	-size/2,	size/2,
        -size/2,	-size/2,	size/2,
        -size/2,	-size/2,	-size/2,
        size/2,	-size/2,	-size/2
    };

    static GLuint elements[] = {
        3, 2, 6, 7, 4, 2, 0,
        3, 1, 6, 5, 4, 1, 0
    };
//from www.paridebroggi.com/2015/06/optimized-cube-opengl-triangle-strip.html
    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 14 * sizeof(GLuint), elements, GL_STATIC_DRAW);
    glBindVertexArray(0);
}
