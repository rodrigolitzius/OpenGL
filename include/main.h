#ifndef TDE_MAIN
#define TDE_MAIN

#include <glad.h>
#include <stdbool.h>

///////// GLFW /////////
#define WINDOW_WIDTH 700
#define WINDOW_HEIGHT 700

///////// OpenGL /////////
#define OPGL_MAJOR_VER 3
#define OPGL_MINOR_VER 3

///////// CAMERA /////////
#define MOV_SPEED 0.13f
#define INITIAL_FOV 90
#define CAMERA_SENSITIVITY 0.005f

///////// CROSS EYE 3D /////////
#define D3_EYE_DISTANCE 0.1f // D3 = 3D

///////// MATH /////////
#define PI 3.14159265358979

///////// Definitions /////////
/* vec3's can be used as positions or directions
   These macros exist to make clear what each vector is representing */
#define POS_X 0
#define POS_Y 1
#define POS_Z 2

#define FORWARD 0 // Negative forward = backwards
#define RIGHT 1   // Negative right = left
#define UP 2      // Negative up = down

#define WORLD_UP (vec3){0.0f, 1.0f, 0.0f}

struct VaoData {
    GLuint vao; unsigned int vertex_count; char name[128]; bool has_texture;
};

#endif