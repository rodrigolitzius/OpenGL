#include <stdlib.h>
#include <string.h>

#include <cglm/cglm.h>

#include "main.h"
#include "camera.h"

#include <GLFW/glfw3.h>

struct camera* camera_create() {
    struct camera* camera = malloc(sizeof(struct camera));

    // Initializing everything to 0
    camera->pos[POS_X] = 0;
    camera->pos[POS_Y] = 0;
    camera->pos[POS_Z] = 0;

    camera->direction[FORWARD] = 0;
    camera->direction[RIGHT] = 0;
    camera->direction[UP] = 0;

    return camera;
}

void camera_move(struct camera* camera, vec3 direction, double speed) {
    vec3 final_movement = {0.0f, 0.0f, 0.0f};

    // Forward and backwards movement
    final_movement[POS_X] += direction[FORWARD] * cos(camera->yaw) * cos(camera->pitch);
    final_movement[POS_Y] += direction[FORWARD] * -sin(camera->pitch);
    final_movement[POS_Z] += direction[FORWARD] * sin(camera->yaw) * cos(camera->pitch);

    // Left and right movement
    final_movement[POS_X] += direction[RIGHT] * cos(PI/2 - camera->yaw);
    final_movement[POS_Z] -= direction[RIGHT] * sin(PI/2 - camera->yaw);

    // Up and down movement
    final_movement[POS_Y] += direction[UP] * 1;

    // Normalize the vector so that speeds don't add up
    // e.g. when you point up and go forward and up at the same time
    glm_normalize(final_movement);
    glm_vec3_scale(final_movement, MOV_SPEED * speed, final_movement);
    glm_vec3_add(camera->pos, final_movement, camera->pos);
}

void camera_shift(struct camera* camera, vec3 vec) {
    vec3 final_movement = {0.0f, 0.0f, 0.0f};

    // Forward and backwards movement
    final_movement[POS_X] += vec[FORWARD] * cos(camera->yaw) * cos(camera->pitch);
    final_movement[POS_Y] += vec[FORWARD] * -sin(camera->pitch);
    final_movement[POS_Z] += vec[FORWARD] * sin(camera->yaw) * cos(camera->pitch);

    // Left and right movement
    final_movement[POS_X] += vec[RIGHT] * cos(PI/2 - camera->yaw);
    final_movement[POS_Z] -= vec[RIGHT] * sin(PI/2 - camera->yaw);

    // Up and down movement
    final_movement[POS_Y] += vec[UP] * 1;

    glm_vec3_add(camera->pos, final_movement, camera->pos);
}

void camera_get_view_matrix(struct camera* camera, mat4* matrix) {
    vec3 up;
    camera_get_up_and_right(camera, NULL, &up);

    glm_translate(*matrix, camera->pos);

    vec3 look_at;
    glm_vec3_add(camera->pos, camera->direction, look_at);
    glm_lookat(camera->pos, look_at, up, *matrix);
}

void camera_update_direction(struct camera* camera) {
    if (camera->pitch > (PI/2.0f) - 0.0005) {
        camera->pitch = (PI/2.0f) - 0.0005;
    } else if (camera->pitch < (-PI/2.0f) + 0.0005) {
        camera->pitch = (-PI/2.0f) + 0.0005;
    }

    camera->direction[FORWARD] = cos(camera->yaw) * cos(camera->pitch);
    camera->direction[RIGHT] = -sin(camera->pitch);
    camera->direction[UP] = sin(camera->yaw) * cos(camera->pitch);

    glm_normalize(camera->direction);
}

void camera_get_up_and_right(struct camera* camera, vec3* right, vec3* up) {
    // The cross product between 2 vector results in a vector orthogonal to both
    vec3 camera_up, camera_right;

    glm_cross(WORLD_UP, camera->direction, camera_right);
    glm_normalize(camera_right);

    glm_cross(camera_right, camera->direction, camera_up);
    glm_normalize(camera_up);

    // if right or up are null it means the caller does not need them
    if (right) {
        memcpy(right, camera_right, sizeof(camera_right));
    }

    if (up) {
        memcpy(up, camera_up, sizeof(camera_up));
    }
}