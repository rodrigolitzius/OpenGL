#include <GLFW/glfw3.h>

#include "main.h"
#include "camera.h"

GLFWgamepadstate gamepad_state;

double mouse_last_x = WINDOW_WIDTH/2.0f;
double mouse_last_y = WINDOW_HEIGHT/2.0f;

void update_gamepad() {
    glfwGetGamepadState(GLFW_JOYSTICK_1, &gamepad_state);
}

void apply_deadzone(vec2 joystick) {
    if (joystick[POS_X] > 0 && joystick[POS_X] < JOYSTICK_DEADZONE) { joystick[POS_X] = 0; }
    if (joystick[POS_X] < 0 && joystick[POS_X] > -JOYSTICK_DEADZONE) { joystick[POS_X] = 0; }
    if (joystick[POS_Y] > 0 && joystick[POS_Y] < JOYSTICK_DEADZONE) { joystick[POS_Y] = 0; }
    if (joystick[POS_Y] < 0 && joystick[POS_Y] > -JOYSTICK_DEADZONE) { joystick[POS_Y] = 0; }
}

void move_camera(GLFWwindow* window, struct camera* camera) {
    update_gamepad();

    vec3 direction_gamepad = {0.0f, 0.0f, 0.0f};
    vec3 direction_keyboard = {0.0f, 0.0f, 0.0f};
    vec3 final_direction = {0.0f, 0.0f, 0.0f};

    vec2 joystick = {
        gamepad_state.axes[GLFW_GAMEPAD_AXIS_LEFT_X],
        gamepad_state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]
    };

    apply_deadzone(joystick);

    direction_gamepad[FORWARD] = -1 * joystick[POS_Y];
    direction_gamepad[RIGHT] = joystick[POS_X];
    direction_gamepad[UP] = ((-1 * gamepad_state.buttons[GLFW_GAMEPAD_BUTTON_CROSS]) + gamepad_state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB]);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { direction_keyboard[FORWARD] += 1; }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { direction_keyboard[FORWARD] += -1; }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { direction_keyboard[RIGHT] += -1; }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { direction_keyboard[RIGHT] += 1; }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) { direction_keyboard[UP] += -1; }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) { direction_keyboard[UP] += 1; }

    double speed = 1;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) { speed = 3; }
    if (gamepad_state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_THUMB]) { speed = 3; }

    glm_vec3_add(direction_keyboard, direction_gamepad, final_direction);

    camera_move(camera, final_direction, speed);
}

void rotate_camera(GLFWwindow* window, struct camera* camera) {
    update_gamepad();

    vec3 direction_gamepad = {0.0f, 0.0f, 0.0f};

    vec2 joystick = {
        gamepad_state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X],
        gamepad_state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]
    };

    apply_deadzone(joystick);

    direction_gamepad[POS_X] = -1 * joystick[POS_X];
    direction_gamepad[POS_Y] = -1 * joystick[POS_Y];

    glm_vec3_scale(direction_gamepad, 0.07, direction_gamepad);

    double mouse_x, mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y);

    double mouse_rel_x_displacement = mouse_last_x - mouse_x;
    double mouse_rel_y_displacement = mouse_last_y - mouse_y;

    mouse_last_x = mouse_x;
    mouse_last_y = mouse_y;

    // fov / INITIAL_FOV prevents the camera from moving too quick or slow allowing for better control over the camera
    camera->yaw += (CAMERA_SENSITIVITY * (camera->fov/INITIAL_FOV) * mouse_rel_x_displacement);
    camera->pitch += (CAMERA_SENSITIVITY * (camera->fov/INITIAL_FOV) * mouse_rel_y_displacement);

    camera->yaw += direction_gamepad[POS_X] * (camera->fov/INITIAL_FOV);
    camera->pitch += direction_gamepad[POS_Y] * (camera->fov/INITIAL_FOV);
}