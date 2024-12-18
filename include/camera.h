#ifndef TDE_CAMERA
#define TDE_CAMERA

#include <cglm/cglm.h>

struct camera {
    vec3 pos; vec3 direction; double yaw; double pitch;
};

// Returns the pointer to a camera struct
struct camera* camera_create();

// Moves the camera relative to it's direction.
// This is meant for player control
// {x, 0.0, 0.0} means forwards/backwards
// {0.0, x, 0.0} means left/right
// {0.0, 0.0, x} means up/down
// - Up and down move the camera relative to the scene up, not it's up vector
// - Values of X are either -1, 0 or 1. Any oher number does not affect the result 
//   since the movement is normalized at the end. The speed is defined by MOV_SPEED
void camera_move(struct camera* camera, vec3 direction);

// Same as move_camera, but does not normalize the vectors, so it can be used to
// move the camera a specified amount
void camera_shift(struct camera* camera, vec3 direction);

// Updates the camera direction based on its yaw and pitch
void camera_update_direction(struct camera* camera);

// Calculates a normalized vector that points to the camera right and up direction relative to its direction
void camera_get_up_and_right(struct camera* camera, vec3* right, vec3* up);

// Returns the view_matrix for the camera
void camera_get_view_matrix(struct camera* camera, mat4* matrix);

#endif