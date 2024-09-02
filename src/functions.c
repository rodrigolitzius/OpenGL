#include <stdio.h>
#include <stdlib.h>

#include <GLFW/glfw3.h>

#include "functions.h"

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
    void* contents = malloc(file_size);

    // Reading all the data
    fread(contents, 1, file_size, file);

    // Closing file
    fclose(file);

    return contents;
}