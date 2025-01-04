#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include "functions.h"
#include "camera.h"
#include "dynamic_array.h"
#include "vertex_spec.h"
#include "shader.h"

#include <main.h>

bool draw_wireframe = false;
bool enable_3d = false;

struct DynamicArray* vaos;

int window_width;
int window_height;

double mouse_last_x = WINDOW_WIDTH/2.0f;
double mouse_last_y = WINDOW_HEIGHT/2.0f;

struct camera* camera;
double fov = INITIAL_FOV;

// Runs when the size of the window is changed
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    window_width = width;
    window_height = height;
}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset) {
    fov += y_offset * fov/INITIAL_FOV * 2;

    if (fov > 180) {
        fov = 180;
    } else if (fov < 0) {
        fov = 0;
    }
}

void mouse_callback(GLFWwindow* window, double x_pos, double y_pos) {
    double mouse_rel_x_displacement = mouse_last_x - x_pos;
    double mouse_rel_y_displacement = mouse_last_y - y_pos;

    mouse_last_x = x_pos;
    mouse_last_y = y_pos;

    // fov/INITIAL_FOV decreases the camera sensitivity as you zoom
    camera->yaw += (CAMERA_SENSITIVITY * (fov/INITIAL_FOV) * mouse_rel_x_displacement);
    camera->pitch += (CAMERA_SENSITIVITY * (fov/INITIAL_FOV) * mouse_rel_y_displacement);
}

// Runs at every key press
void key_event_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Switch wireframe mode
    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        draw_wireframe = !draw_wireframe;

        if (draw_wireframe == true) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }

    if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
        enable_3d = !enable_3d;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
}

void handle_input(GLFWwindow* window) {
    vec3 direction = {0.0f, 0.0f, 0.0f};

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        direction[FORWARD] += 1;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        direction[FORWARD] += -1;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        direction[RIGHT] += -1;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        direction[RIGHT] += 1;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        direction[UP] += -1;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        direction[UP] += 1;
    }

    camera_move(camera, direction);
}

// Stuff to do only once at the start of the program
void initialize(GLFWwindow** window) {
    int return_value = 0;

    // Initiallizing GLFW
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPGL_MAJOR_VER);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPGL_MINOR_VER);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    *window = glfwCreateWindow(
        WINDOW_WIDTH, WINDOW_HEIGHT, 
        "My 3D Engine", 
        NULL, NULL
    );

    if (*window == NULL) {
        exit_with_error_glfw("Could not initialize GLFW.");
    }

    glfwMakeContextCurrent(*window);

    // Set OpenGL viewport when the window is resized
    glfwSetFramebufferSizeCallback(*window, framebuffer_size_callback);
    glfwSetKeyCallback(*window, key_event_callback);
    glfwSetScrollCallback(*window, scroll_callback);
    glfwSetCursorPosCallback(*window, mouse_callback);
    glfwSetMouseButtonCallback(*window, mouse_button_callback);

    glfwGetFramebufferSize(*window, &window_width, &window_height);

    // Intiializing GLAD
    return_value = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    if (!return_value) {
        exit_with_error_generic("Failed to load GLAD.", "");
    }

    // OpenGL stuff
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glEnable(GL_DEPTH_TEST);

    vaos = dynarray_create(sizeof(struct VaoData));
}

void render_full(mat4 model, mat4 view, mat4 projection, GLuint shader_program1, GLuint shader_program2) {
    struct VaoData cube_vao = *(struct VaoData*)(vaos->data[0]);
    struct VaoData light_cube_vao = *(struct VaoData*)(vaos->data[1]);

    vec4 light_color = {1.0f, 1.0f, 1.0f, 1.0f};
    vec4 object_color = {1.0f, 0.5f, 0.31f};

    ///////// Cube /////////
    glBindVertexArray(cube_vao.vao);
    glUseProgram(shader_program1);

    glUniform4fv(glad_glGetUniformLocation(shader_program1, "light_color"), 1, light_color);
    glUniform4fv(glad_glGetUniformLocation(shader_program1, "object_color"), 1, object_color);

    glUniformMatrix4fv(glad_glGetUniformLocation(shader_program1, "model"), 1, GL_FALSE, *model);
    glUniformMatrix4fv(glad_glGetUniformLocation(shader_program1, "view"), 1, GL_FALSE, *view);
    glUniformMatrix4fv(glad_glGetUniformLocation(shader_program1, "projection"), 1, GL_FALSE, *projection);

    glDrawArrays(GL_TRIANGLES, 0, cube_vao.vertex_count);

    ///////// Light source /////////
    glBindVertexArray(light_cube_vao.vao);
    glUseProgram(shader_program2);

    glm_translate(model, (vec3){10.0f, -2.0f, 0.0f});

    glUniform4fv(glad_glGetUniformLocation(shader_program2, "light_color"), 1, light_color);

    glUniformMatrix4fv(glad_glGetUniformLocation(shader_program2, "model"), 1, GL_FALSE, *model);
    glUniformMatrix4fv(glad_glGetUniformLocation(shader_program2, "view"), 1, GL_FALSE, *view);
    glUniformMatrix4fv(glad_glGetUniformLocation(shader_program2, "projection"), 1, GL_FALSE, *projection);

    glm_translate(model, (vec3){-2.0f, 2.0f, 0.0f});

    glDrawArrays(GL_TRIANGLES, 0, light_cube_vao.vertex_count);
}

int main() {
    // Initializing
    GLFWwindow* window;

    initialize(&window);

    // test();

    struct Shader cube_shader_data[] = {
        (struct Shader){"./shaders/cube.vs", GL_VERTEX_SHADER},
        (struct Shader){"./shaders/cube.fs", GL_FRAGMENT_SHADER},
    };

    struct Shader light_cube_shader_data[] = {
        (struct Shader){"./shaders/light_cube.vs", GL_VERTEX_SHADER},
        (struct Shader){"./shaders/light_cube.fs", GL_FRAGMENT_SHADER},
    };

    int cube_shader_data_count = sizeof(cube_shader_data)/sizeof(struct Shader);
    int light_cube_shader_data_count = sizeof(light_cube_shader_data)/sizeof(struct Shader);

    // Build, compile and link the shaders
    GLuint cube_shader_program = link_shaders(cube_shader_data, cube_shader_data_count);
    GLuint light_cube_shader_program = link_shaders(light_cube_shader_data, light_cube_shader_data_count);

    // Creating all the VAOs
    vertex_spec(vaos);

    camera = camera_create();
    camera_shift(camera, (vec3){0.0f, 0.0f, 3.0f});

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Declaring transformation matrices
        mat4 model_mat, view_mat, projection_mat;
        glm_mat4_identity(model_mat);
        glm_mat4_identity(view_mat);
        glm_mat4_identity(projection_mat);

        if (enable_3d) {
            glm_perspective(glm_rad(fov), (window_width/2.0f)/(double)window_height, 0.01, 100, projection_mat);
        } else {
            glm_perspective(glm_rad(fov), (double)window_width/(double)window_height, 0.01, 100, projection_mat);
        }

        // View matrix is defined by the camera
        camera_update_direction(camera);
        camera_get_view_matrix(camera, &view_mat);

        // Input
        handle_input(window);

        // Finally drawing the vertices
        if (enable_3d) {
            // Basically this moves the camera left and right and recalculates its projection matrix
            // to get a different image on each side of the screen, resulting in the 3D effect
            
            // Left half of the window
            glViewport(0, 0, window_width/2, window_height);
            camera_shift(camera, (vec3){0.0f, D3_EYE_DISTANCE, 0.0f});
            camera_get_view_matrix(camera, &view_mat);
            render_full(model_mat, view_mat, projection_mat, cube_shader_program, light_cube_shader_program);
            camera_shift(camera, (vec3){0.0f, -D3_EYE_DISTANCE, 0.0f});

            // Right half of the window
            glViewport(window_width/2, 0, window_width/2, window_height);
            camera_shift(camera, (vec3){0.0f, -D3_EYE_DISTANCE, 0.0f});
            camera_get_view_matrix(camera, &view_mat);
            render_full(model_mat, view_mat, projection_mat, cube_shader_program, light_cube_shader_program);
            camera_shift(camera, (vec3){0.0f, D3_EYE_DISTANCE, 0.0f});
        } else {
            // Full window
            glViewport(0, 0, window_width, window_height);
            render_full(model_mat, view_mat, projection_mat, cube_shader_program, light_cube_shader_program);
        }

        // Updating window
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Clearing the screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    dynarray_free(vaos);

    glfwTerminate();

    return EXIT_SUCCESS;
}