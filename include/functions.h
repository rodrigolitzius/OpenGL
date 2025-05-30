#ifndef TDE_FUNCTIONS
#define TDE_FUNCTIONS

#include <cglm/cglm.h>
#include <include_glad.h>

// Print an error with glfw and exit
void exit_with_error_glfw(const char* error_text);

// Print a generic error and exit
void exit_with_error_generic(const char* error_text, const char* error_description);

// Reads all contents of a file
void* file_read_all(const char* file_path);

// Sums all the values in an array
double sum_array(double* array, size_t array_length);

// Creates identity model, view, and projection matrices
struct MVP create_mvp();

// Normalize vec only if its magnitude > 1
void normalize_if_mag_gt_1(vec3 vec, vec3 dest);

// Error checked and logged malloc()
void* safe_malloc(size_t size);

#endif