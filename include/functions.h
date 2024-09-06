#ifndef TDE_FUNCTIONS
#define TDE_FUNCTIONS

#include <glad.h>

// Print an error with glfw and exit
void exit_with_error_glfw(const char* error_text);
// Print a generic error and exit
void exit_with_error_generic(const char* error_text, const char* error_description);
// Reads all contents of a file
void* file_read_all(const char* file_path);
// Transforms coordinates from 0 -> window_size to -1 -> 1
void to_range(double max_x, double max_y, double* x, double* y);
// Compile a GLSL shader
GLuint compile_shader(const GLchar* source, GLenum type);

#endif