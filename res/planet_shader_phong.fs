#version 330 core
out vec4 color;

in vec3 position_fragment;
in vec3 color_fragment;
in vec3 normal_fragment;

uniform mat3 normal_model;
uniform mat4 model;
uniform vec3 light_color;
uniform vec3 light_position;
uniform vec3 camera_position;
uniform float min_ambient;

void main()
{
    vec3 normal = normalize(normal_model * normal_fragment);
    vec3 frag_pos = vec3(model * vec4(position_fragment, 1));
    vec3 surface_to_light = normalize(light_position - frag_pos);
    vec3 surface_to_camera = normalize(-camera_position - frag_pos);
    //brightness = cos(angle_of_incidence)
    float brightness = dot(normal, surface_to_light);
    float specular_exponent = 512;
    float specular_coeff = 0.0;
    if(brightness > 0)
        specular_coeff = pow( max(0.0, dot(surface_to_camera, reflect(-surface_to_light, normal))),
                              specular_exponent);
    vec3 specular_component = specular_coeff/4 * light_color * color_fragment;
    brightness = max(min_ambient, brightness);
    color = vec4( specular_component + (color_fragment * light_color *  brightness), 1.0f);
}
