#version 330 core

out vec4 FragColor;

uniform vec3 view_position;

in vec3 frag_normal_vec;
in vec3 frag_frag_pos;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular; 
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;
uniform Material material;

void main() {
    vec3 light_direction = normalize(light.position - frag_frag_pos);
    vec3 view_direction = normalize(frag_frag_pos - view_position);

    vec3 reflect_direction = reflect(light_direction, frag_normal_vec);

    float diffuse_scalar = max(dot(frag_normal_vec, light_direction), 0.0);
    vec3 diffuse_color = diffuse_scalar * material.diffuse * light.diffuse;

    float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
    vec3 specular = light.specular * spec * material.specular;

    vec3 ambient_color = light.ambient * material.ambient;

    vec3 final_light = (ambient_color + diffuse_color + specular);

    FragColor = vec4(final_light, 1.0);
}