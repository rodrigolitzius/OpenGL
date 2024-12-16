#ifndef TDE_MAIN
#define TDE_MAIN

#include <glad.h>
#include <stdbool.h>

#define WINDOW_WIDTH 700
#define WINDOW_HEIGHT 700

#define OPGL_MAJOR_VER 3
#define OPGL_MINOR_VER 3

#define WORLD_UP (vec3){0.0f, 1.0f, 0.0f}
#define MOV_SPEED 0.13
#define CAMERA_SENSITIVITY 0.005

/////////////////////

// vec3's can be used as positions or directions
// These macros exist to make clear what each vector is representing
// They're indices of any vec3
#define POS_X 0
#define POS_Y 1
#define POS_Z 2

#define FORWARD 0 // Negative forward = backwards
#define RIGHT 1   // Negative right = left
#define UP 2      // Negative up = down

/////////////////////

#define PI 3.14159265358979

struct VaoData {
    GLuint vao; unsigned int vertex_count; char name[128]; bool has_texture;
};

#endif