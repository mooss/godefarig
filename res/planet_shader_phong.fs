#version 330 core
out vec4 color;

in vec3 position_fragment;
in vec3 color_fragment;
in vec3 normal_fragment;

uniform mat3 normal_model;
uniform mat4 model;
uniform vec3 light_color;
uniform vec3 light_position;

void main()
{
    vec3 normal = normalize(normal_model * normal_fragment);
    vec3 frag_pos = vec3(model * vec4(position_fragment, 1));
    vec3 surface_to_light = light_position - frag_pos;

    //brightness = cos(angle_of_incidence)
    float brightness = dot(normal, surface_to_light) / ( length(normal) * length(surface_to_light) );
    brightness = clamp(brightness, 0, 1);
    
    color = vec4(color_fragment * light_color * brightness, 1.0f);
} 
