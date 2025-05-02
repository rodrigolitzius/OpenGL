#include <stdio.h>
#include <stdlib.h>

#include "functions.h"
#include "main.h"

#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

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

void* file_read_all(const char* file_path) {
    FILE* file = fopen(file_path, "r");
    // Finding the size of the file in bytes
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocating enough memory to hold the data in the file
    void* contents = safe_malloc(file_size+1);

    // Reading all the data
    fread(contents, 1, file_size, file);

    // Adding a null terminator to the end of the string
    *((char*)(contents+file_size)) = '\0';

    // Closing file
    fclose(file);

    return contents;
}

double sum_array(double* array, size_t array_length) {
    double sum = 0;

    for (size_t i=0; i < array_length; i++) {
        sum += array[i];
    };

    return sum;
}

struct MVP create_mvp() {
    struct MVP mvp;

    glm_mat4_identity(mvp.model);
    glm_mat4_identity(mvp.view);
    glm_mat4_identity(mvp.projection);

    return mvp;
}

void normalize_if_mag_gt_1(vec3 vec, vec3 dest) {
    if (glm_vec3_norm(vec) > 1.0f) {
        glm_normalize_to(vec, dest);
    } else {
        glm_vec3_copy(vec, dest);
    }
}

void* safe_malloc(size_t size) {
    void* address = malloc(size);

    if (!address) {
        exit_with_error_generic("Malloc failed", "");
    }

    return address;
}