#version 330 core
  
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 inColor;

out vec3 ourColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightColor;

void main()
{
    float ambientStrength = 1.0f;
	
    gl_Position = projection * view * model * vec4(position, 1.0f);
    ourColor = inColor * lightColor * ambientStrength;
}
