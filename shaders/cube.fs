#version 330 core

out vec4 FragColor;

uniform vec3 view_position;

in vec3 frag_normal_vec;
in vec3 frag_frag_pos;
in vec2 frag_text_coords;

struct Material {
    sampler2D diffuse;
    sampler2D specular; 
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
    vec3 diffuse_color = light.diffuse * diffuse_scalar * texture(material.diffuse, frag_text_coords).rgb;

    float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, frag_text_coords).rgb;

    vec3 ambient_color = light.ambient * texture(material.diffuse, frag_text_coords).rgb;

    vec3 final_light = (ambient_color + diffuse_color + specular);

    FragColor = vec4(final_light, 1.0);
}