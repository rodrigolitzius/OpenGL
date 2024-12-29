#include <stdlib.h>

#include <glad.h>

#include "shader.h"
#include "functions.h"

GLuint compile_shader(const GLchar* source, GLenum type) {
    // Creating the shader object
    GLuint shader;
    shader = glCreateShader(type);

    // COmpiling from the source code
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    // Checking for erros in the compilation
    GLint shader_compiled = GL_TRUE;
    char compiling_log[8192];

    glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_compiled);

    if (shader_compiled == GL_FALSE) {
        glGetShaderInfoLog(shader, 8192, NULL, compiling_log);
        exit_with_error_generic("Failed to compile shader", compiling_log);
    }

    return shader;
}

GLuint link_shaders(struct Shader* shaders_data, int shader_data_count) {
    GLuint shader_program = glCreateProgram();

    for (int i=0; i<shader_data_count; i++) {
        const char* shader_src = file_read_all(shaders_data[i].path);
        GLuint shader = compile_shader(shader_src, shaders_data[i].type);

        glAttachShader(shader_program, shader);
        glDeleteShader(shader);

        free((void*)shader_src);
    }

    glLinkProgram(shader_program);

    // Checking for errors in the linking process
    int linked_successfully = GL_TRUE;
    char linking_log[8192];
    glGetProgramiv(shader_program, GL_LINK_STATUS, &linked_successfully);

    if (linked_successfully == GL_FALSE) {
        glGetProgramInfoLog(shader_program, 8192, NULL, linking_log);
        exit_with_error_generic("Failed to link shaders.", linking_log);
    }

    return shader_program;
}