#ifndef TDE_CAMERA
#define TDE_CAMERA

#include <cglm/cglm.h>

struct camera {
    vec3 pos; vec3 direction; double yaw; double pitch; double fov;
};

// Returns the pointer to a camera struct
struct camera* camera_create();

// Frees the camera struct
void camera_destroy(struct camera* camera);

// Moves the camera relative to it's direction.
// This is meant for player control since speed is normalized
// {x, 0.0, 0.0} means forwards/backwards
// {0.0, x, 0.0} means left/right
// {0.0, 0.0, x} means up/down
// - Up and down move the camera relative to the scene up, not it's up vector
// - Values of X are either -1, 0 or 1. Any oher number does not affect the result 
// - The speed is defined by MOV_SPEED * speed
void camera_move(struct camera* camera, vec3 direction, double speed);

// Same as move_camera, but does not normalize the vectors, so it can be used to
// move the camera a specific amount. The direction AND speed is defined by vec
void camera_shift(struct camera* camera, vec3 vec);

// Updates the camera direction based on its yaw and pitch
void camera_update_direction(struct camera* camera);

// Calculates a normalized vector that points to the camera right and up direction relative to its direction
void camera_get_up_and_right(struct camera* camera, vec3* right, vec3* up);

// Returns the view_matrix for the camera
void camera_get_view_matrix(struct camera* camera, mat4* matrix);

#endif