#version 330 core
  
layout (location = 0) in vec3 position_vertex;
layout (location = 1) in vec3 color_vertex;

out vec3 color_fragment;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightColor;

void main()
{
    float ambient_strength = 1.0f;
	
    gl_Position = projection * view * model * vec4(position_vertex, 1.0f);
    color_fragment = color_vertex * lightColor * ambient_strength;
}
