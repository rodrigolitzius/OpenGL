#version 330 core

out vec4 FragColor;

uniform vec3 light_color;
uniform vec3 light_pos;
uniform vec3 object_color;
uniform vec3 view_position;

in vec3 frag_normal_vec;
in vec3 frag_frag_pos;

float ambient_light_strength = 0.3;
float specular_strength = 0.9;

void main() {
    vec3 light_direction = normalize(light_pos - frag_frag_pos);
    vec3 view_direction = normalize(frag_frag_pos - view_position);

    vec3 reflect_direction = reflect(light_direction, frag_normal_vec);

    float diffuse_scalar = max(dot(frag_normal_vec, light_direction), 0.0);
    vec3 diffuse_color = diffuse_scalar * object_color;

    float spec = pow(max(dot(view_direction, reflect_direction), 0.0), 32);
    vec3 specular = specular_strength * spec * light_color;

    vec3 ambient_color = ambient_light_strength * light_color;

    vec3 final_light = (ambient_color + diffuse_color + specular) * object_color;

    FragColor = vec4(final_light, 1.0);
}