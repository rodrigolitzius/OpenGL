#ifndef TDE_SHADER
#define TDE_SHADER

#include <glad.h>

struct Shader {
    char* path; GLuint type;
};

GLuint compile_shader(const GLchar* source, GLenum type);
GLuint link_shaders(struct Shader* shaders_data, int shader_data_count);

#endif