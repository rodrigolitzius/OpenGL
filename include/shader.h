#ifndef TDE_SHADER
#define TDE_SHADER

#include <glad.h>
#include <cglm/cglm.h>

// Light types
enum LIGHT_TYPES {
    // Point light
    LIGHTT_POINT = 0,
    // Directional lighs
    LIGHTT_DIRECTIONAL = 1,
    // Spotlight
    LIGHTT_SPOT = 2,
};

// represents a single shader (i.e. not a shader program containing multiple shaders)
struct Shader {
    char* path; GLuint type;
};

// Common variables for different types of lighting
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

// Compile a single shader
GLuint compile_shader(const GLchar* source, GLenum type);

// Link multiple shaders into a shader program
GLuint link_shaders(struct Shader* shaders_data, int shader_data_count);

#endif