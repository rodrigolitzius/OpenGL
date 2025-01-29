#ifndef TDE_SHADER
#define TDE_SHADER

#include <glad.h>
#include <cglm/cglm.h>

// Light types
#define LT_POINT       0
#define LT_DIRECTIONAL 1
#define LT_SPOT        2

struct Shader {
    char* path; GLuint type;
};

struct Light {
    int type;

    vec3 vec;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    vec3 direction;
    float cutoff;
};

GLuint compile_shader(const GLchar* source, GLenum type);
GLuint link_shaders(struct Shader* shaders_data, int shader_data_count);

#endif