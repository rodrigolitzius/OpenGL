#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal_vec;
layout (location = 2) in vec2 text_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 frag_normal_vec;
out vec3 frag_frag_pos;
out vec2 frag_text_coords;

void main() {
    gl_Position = projection * view * model * vec4(position.x, position.y, position.z, 1.0);

    frag_normal_vec = normalize(mat3(transpose(inverse(model))) * normal_vec);
    frag_frag_pos = vec3(model * vec4(position, 1.0));
    frag_text_coords = text_coords;
}