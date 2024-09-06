#include <stdio.h>
#include <stdlib.h>

#include "functions.h"

#include <GLFW/glfw3.h>

void exit_with_error_glfw(const char* error_text) {
    const char* error_description = NULL;
    glfwGetError(&error_description);

    fprintf(stderr, "\n[E] ERROR: %s\n%s\n\n", error_text, (char*) error_description);
    exit(EXIT_FAILURE);
}

void exit_with_error_generic(const char* error_text, const char* error_description) {
    fprintf(stderr, "\n[E] ERROR: %s\n%s\n\n", error_text, error_description);
    exit(EXIT_FAILURE);
}

// Reads all contents of a file
void* file_read_all(const char* file_path) {
    FILE* file = fopen(file_path, "r");
    // Finding the size of the file in bytes
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocating enough memory to hold the data in the file
    void* contents = malloc(file_size+1);

    // Reading all the data
    fread(contents, 1, file_size, file);

    // Adding a null terminator to the end of the string
    *((char*)(contents+file_size)) = '\0';

    // Closing file
    fclose(file);

    printf("%s\n", (char*) contents);

    return contents;
}

void to_range(double max_x, double max_y, double* x, double* y) {
    *x = (*x - (max_x/2.0f)) / (max_x/2.0f);
    *y = -(*y - (max_y/2.0f)) / (max_y/2.0f);
}

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