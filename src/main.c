#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <unistd.h>

#include "functions.h"
#include "camera.h"
#include "dynamic_array.h"
#include "texture.h"
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
    glfwSwapInterval(0);

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

void render_full(struct MVP mvp_original, GLuint shader_program1, GLuint shader_program2) {
    mat4 model, view, projection;

    glm_mat4_copy(mvp_original.model, model);
    glm_mat4_copy(mvp_original.view, view);
    glm_mat4_copy(mvp_original.projection, projection);

    struct VaoData cube_vao = *(struct VaoData*)(vaos->data[0]);
    struct VaoData light_cube_vao = *(struct VaoData*)(vaos->data[1]);

    vec3 light_color = {1.0f, 1.0f, 1.0f};
    vec3 light_position = {5.0f, -2.0f, 4.0f};

    glm_vec3_rotate(light_position, glfwGetTime()/1.2f, (vec3){0.0f, 1.0f, 0.0f});

    ///////// Cube /////////
    glm_scale(model, (vec3){3.0f, 3.0f, 3.0f});

    vec3 color_change = {
        /* (sin(glfwGetTime() * 2.0f) + 1) / 2, */ 1,
        /* (sin(glfwGetTime() * 0.7f) + 1) / 2, */ 1,
        /* (sin(glfwGetTime() * 1.3f) + 1) / 2, */ 1,
    };

    glm_vec3_mulv(light_color, color_change, light_color);

    vec3 light_diffuse;
    glm_vec3_scale(light_color, 0.8f, light_diffuse);

    vec3 light_ambient;
    glm_vec3_scale(light_color, 0.3f, light_ambient);

    vec3 light_specular;
    glm_vec3_scale(light_color, 1.0f, light_specular);

    glBindVertexArray(cube_vao.vao);

    glUseProgram(shader_program1);

    glUniform3fv(glGetUniformLocation(shader_program1, "light.position"), 1, light_position);

    glUniform3fv(glGetUniformLocation(shader_program1, "light.ambient"), 1, light_ambient);
    glUniform3fv(glGetUniformLocation(shader_program1, "light.diffuse"), 1, light_diffuse);
    glUniform3fv(glGetUniformLocation(shader_program1, "light.specular"), 1, light_specular);

    glUniform1f(glGetUniformLocation(shader_program1, "material.shininess"), 32);

    glUniform3fv(glGetUniformLocation(shader_program1, "view_position"), 1, camera->pos);

    glUniformMatrix4fv(glGetUniformLocation(shader_program1, "model"), 1, GL_FALSE, *model);
    glUniformMatrix4fv(glGetUniformLocation(shader_program1, "view"), 1, GL_FALSE, *view);
    glUniformMatrix4fv(glGetUniformLocation(shader_program1, "projection"), 1, GL_FALSE, *projection);

    glDrawArrays(GL_TRIANGLES, 0, cube_vao.vertex_count);

    glm_mat4_copy(mvp_original.model, model);

    ///////// Light source /////////
    glBindVertexArray(light_cube_vao.vao);
    glUseProgram(shader_program2);

    glm_translate(model, light_position);
    glm_scale(model, (vec3){0.5f, 0.5f, 0.5f});

    glUniform3fv(glGetUniformLocation(shader_program2, "light_color"), 1, light_color);

    glUniformMatrix4fv(glGetUniformLocation(shader_program2, "model"), 1, GL_FALSE, *model);
    glUniformMatrix4fv(glGetUniformLocation(shader_program2, "view"), 1, GL_FALSE, *view);
    glUniformMatrix4fv(glGetUniformLocation(shader_program2, "projection"), 1, GL_FALSE, *projection);

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

    struct TextureWrap default_wrap = {
        GL_REPEAT, GL_REPEAT, 
        GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR
    };

    GLuint diffuse_map = load_texture("./textures/container2.png", GL_RGBA, default_wrap, false);
    GLuint specular_map = load_texture("./textures/container2_specular.png", GL_RGBA, default_wrap, false);

    glUseProgram(cube_shader_program);
    glUniform1i(glGetUniformLocation(cube_shader_program, "material.diffuse"), 0);
    glUniform1i(glGetUniformLocation(cube_shader_program, "material.specular"), 1);

    camera = camera_create();
    camera_shift(camera, (vec3){0.0f, 0.0f, 3.0f});

    double time_start, time_end;
    double fps_values[FPS_CAP];
    uint64_t fps_i = 0;

    int fps_values_length = sizeof(fps_values) / sizeof(double);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        time_start = glfwGetTime();

        // Input
        handle_input(window);

        // Declaring transformation matrices
        struct MVP mvp = create_mvp();

        if (enable_3d) {
            glm_perspective(glm_rad(fov), (window_width/2.0f)/(double)window_height, 0.01, 100, mvp.projection);
        } else {
            glm_perspective(glm_rad(fov), (double)window_width/(double)window_height, 0.01, 100, mvp.projection);
        }

        // View matrix is defined by the camera
        camera_update_direction(camera);
        camera_get_view_matrix(camera, &mvp.view);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuse_map);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specular_map);

        // Finally drawing the vertices
        if (enable_3d) {
            // Basically this moves the camera left and right and recalculates its view matrix
            // to get a different image on each side of the screen, resulting in the 3D effect
            
            // Left half of the window
            glViewport(0, 0, window_width/2, window_height);
            camera_shift(camera, (vec3){0.0f, D3_EYE_DISTANCE, 0.0f});
            camera_get_view_matrix(camera, &mvp.view);
            render_full(mvp, cube_shader_program, light_cube_shader_program);
            camera_shift(camera, (vec3){0.0f, -D3_EYE_DISTANCE, 0.0f});

            // Right half of the window
            glViewport(window_width/2, 0, window_width/2, window_height);
            camera_shift(camera, (vec3){0.0f, -D3_EYE_DISTANCE, 0.0f});
            camera_get_view_matrix(camera, &mvp.view);
            render_full(mvp, cube_shader_program, light_cube_shader_program);
            camera_shift(camera, (vec3){0.0f, D3_EYE_DISTANCE, 0.0f});
        } else {
            // Full window
            glViewport(0, 0, window_width, window_height);
            render_full(mvp, cube_shader_program, light_cube_shader_program);
        }

        // Updating window
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Clearing the screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // FPS calculation
        time_end = glfwGetTime();

        double time_elapsed = time_end - time_start;

        double sleep_time = (1.0f / FPS_CAP) - time_elapsed;
        sleep_time = glm_max(sleep_time, 0);

        usleep(sleep_time * 1000000.0f);

        time_elapsed += sleep_time;
        double fps = 1 / time_elapsed;

        fps_values[fps_i] = fps;
        fps_i += 1;
        
        if (fps_i >= fps_values_length) {
            double avg_fps = sum_array(fps_values, fps_values_length) / fps_values_length;
            printf("%f\n", avg_fps);

            fps_i = 0;
        }

    }

    dynarray_free(vaos);

    glfwTerminate();

    return EXIT_SUCCESS;
}