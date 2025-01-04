#ifndef TDE_FUNCTIONS
#define TDE_FUNCTIONS

#include <glad.h>

// Print an error with glfw and exit
void exit_with_error_glfw(const char* error_text);

// Print a generic error and exit
void exit_with_error_generic(const char* error_text, const char* error_description);

// Reads all contents of a file
void* file_read_all(const char* file_path);

struct MVP create_mvp();

#endif