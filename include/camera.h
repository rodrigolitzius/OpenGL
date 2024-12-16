#ifndef TDE_CAMERA
#define TDE_CAMERA

#include <cglm/cglm.h>

struct camera {
    vec3 pos; vec3 direction; double yaw; double pitch;
};

// Returns the pointer to a camera struct
struct camera* camera_create();

// Moves the camera in the specified direction
void camera_move(struct camera* camera, vec3 direction);

// Updates the camera direction based on its yaw and pitch
void camera_update_direction(struct camera* camera);

// Calculates a normalized vection that points to the camera right and up direction relative to its direction
void camera_get_up_and_right(struct camera* camera, vec3* right, vec3* up);

#endif