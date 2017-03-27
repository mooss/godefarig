#version 330 core
out vec4 color;

in vec3 color_fragment;

void main()
{
    color = vec4(color_fragment, 1.0f);
} 
