// This is required for nanosleep()
#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <include_glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include "functions.h"
#include "camera.h"
#include "dynamic_array.h"
#include "texture.h"
#include "vertex_spec.h"
#include "input.h"
#include "shader.h"
#include "model.h"

#include <main.h>

bool draw_wireframe = false;
bool enable_3d = false;

struct DynamicArray* vaos;

int window_width;
int window_height;

struct camera* camera;
// Used in lighting calculations when the ligthing directly depends on the position of the camera.
// such as a spotlight
vec3 camera_original_pos;
// Runs when the size of the window is changed
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    window_width = width;
    window_height = height;
}

// Runs when the scroll wheels is, well, scrolled
void scroll_callback(GLFWwindow* window, double x_offset, double y_offset) {
    // fov / INITIAL_FOV prevents the FOV changing too quick or slow, allowing for better control over the FOV
    camera->fov += y_offset * camera->fov / INITIAL_FOV * 2;
    camera->fov = glm_clamp(camera->fov, 1, 179);
}

// Runs at every movement of the mouse
void mouse_callback(GLFWwindow* window, double x_pos, double y_pos) {
    // Nothing to do (there was once)
}

// Runs at every key press
void key_event_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Toggle wireframe mode
    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        draw_wireframe = !draw_wireframe;

        if (draw_wireframe == true) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }

    // Toggle 3D view mode
    if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
        enable_3d = !enable_3d;
    }
}

// Runs everytime a mouse button is pressed
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    // Grab/release cursor
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
}

void handle_input(GLFWwindow* window) {
    move_camera(window, camera);
    rotate_camera(window, camera);
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

// Render the scene to the screen
void render_full(struct MVP mvp_original, struct Model d3_model, GLuint shader_program1, GLuint shader_program2, GLuint shader_program3) {
    mat4 model, view, projection;

    // Original copies of the MVP are necessary to rollback changes to it
    glm_mat4_copy(mvp_original.model, model);
    glm_mat4_copy(mvp_original.view, view);
    glm_mat4_copy(mvp_original.projection, projection);

    struct VaoData cube_vao = *(struct VaoData*)(vaos->data[0]);
    struct VaoData light_cube_vao = *(struct VaoData*)(vaos->data[1]);
    struct VaoData axis_vao = *(struct VaoData*)(vaos->data[2]);

    vec3 light_color = {1.0f, 1.0f, 1.0f};

    // Different lighting types
    int light_type = LIGHTT_POINT;
    vec3 light_vec = {0.0f, -2.0f, 2.0f};

    // int light_type = LIGHTT_DIRECTIONAL;
    // vec3 light_vec = {-0.2f, -1.0f, -0.3f};

    // int light_type = LIGHTT_SPOT;
    // vec3 light_vec;
    // glm_vec3_copy(camera_original_pos, light_vec);

    // glm_vec3_rotate(light_vec, glfwGetTime()/1.2f, (vec3){0.0f, 1.0f, 0.0f});

    // Change the color of the lighting here
    vec3 color_change = {1.0f, 1.0f, 1.0f};

    // For funky lighting effects
    // vec3 color_change = {
    //     (sin(glfwGetTime() * 2.0f) + 1) / 2,
    //     (sin(glfwGetTime() * 0.7f) + 1) / 2,
    //     (sin(glfwGetTime() * 1.3f) + 1) / 2,
    // };

    glm_vec3_mulv(light_color, color_change, light_color);

    vec3 light_diffuse;
    glm_vec3_scale(light_color, 0.8f, light_diffuse);

    vec3 light_ambient;
    glm_vec3_scale(light_color, 0.2f, light_ambient);

    vec3 light_specular;
    glm_vec3_scale(light_color, 1.0f, light_specular);

    glBindVertexArray(cube_vao.vao);

    glUseProgram(shader_program1);

    // Passing light variables to the shader
    glUniform1i(glGetUniformLocation(shader_program1, "light.type"), light_type);
    glUniform3fv(glGetUniformLocation(shader_program1, "light.vec"), 1, light_vec);

    glUniform3fv(glGetUniformLocation(shader_program1, "light.ambient"), 1, light_ambient);
    glUniform3fv(glGetUniformLocation(shader_program1, "light.diffuse"), 1, light_diffuse);
    glUniform3fv(glGetUniformLocation(shader_program1, "light.specular"), 1, light_specular);

    glUniform1f(glGetUniformLocation(shader_program1, "light.constant"), 1.0f);
    glUniform1f(glGetUniformLocation(shader_program1, "light.linear"), 0.027f);
    glUniform1f(glGetUniformLocation(shader_program1, "light.quadratic"), 0.0028f);

    glUniform3fv(glGetUniformLocation(shader_program1, "light.direction"), 1, camera->direction);
    glUniform1f(glGetUniformLocation(shader_program1, "light.cutoff"), cos(glm_rad(12.5)));
    glUniform1f(glGetUniformLocation(shader_program1, "light.outer_cutoff"), cos(glm_rad(17.5)));

    glUniform1f(glGetUniformLocation(shader_program1, "material.shininess"), 32);

    if (light_type == LIGHTT_SPOT) {
        glUniform3fv(glGetUniformLocation(shader_program1, "view_position"), 1, camera_original_pos);
    } else {
        glUniform3fv(glGetUniformLocation(shader_program1, "view_position"), 1, camera->pos);
    }

    glm_rotate(model, glfwGetTime()/3, (vec3){0.0, 1.0, 0.0});
    glm_scale(model, (vec3){1.0f, -1.0f, 1.0f});

    glUniformMatrix4fv(glGetUniformLocation(shader_program1, "model"), 1, GL_FALSE, *model);
    glUniformMatrix4fv(glGetUniformLocation(shader_program1, "view"), 1, GL_FALSE, *view);
    glUniformMatrix4fv(glGetUniformLocation(shader_program1, "projection"), 1, GL_FALSE, *projection);

    draw_model(d3_model, shader_program1);

    glm_mat4_copy(mvp_original.model, model);

    ///////// Light source /////////
    glBindVertexArray(light_cube_vao.vao);
    glUseProgram(shader_program2);

    glm_translate(model, light_vec);
    glm_scale(model, (vec3){0.3f, 0.3f, 0.3f});

    glUniform3fv(glGetUniformLocation(shader_program2, "light_color"), 1, light_color);

    glUniformMatrix4fv(glGetUniformLocation(shader_program2, "model"), 1, GL_FALSE, *model);
    glUniformMatrix4fv(glGetUniformLocation(shader_program2, "view"), 1, GL_FALSE, *view);
    glUniformMatrix4fv(glGetUniformLocation(shader_program2, "projection"), 1, GL_FALSE, *projection);

    if (light_type == LIGHTT_POINT) {
        glDrawArrays(GL_TRIANGLES, 0, light_cube_vao.vertex_count);
    }

    glm_mat4_copy(mvp_original.model, model);

    ///////// Axis /////////
    glBindVertexArray(axis_vao.vao);
    glUseProgram(shader_program3);

    glUniformMatrix4fv(glGetUniformLocation(shader_program3, "model"), 1, GL_FALSE, *model);
    glUniformMatrix4fv(glGetUniformLocation(shader_program3, "view"), 1, GL_FALSE, *view);
    glUniformMatrix4fv(glGetUniformLocation(shader_program3, "projection"), 1, GL_FALSE, *projection);

    glDrawArrays(GL_LINES, 0, axis_vao.vertex_count);
}

int main() {
    // Initializing
    GLFWwindow* window;

    initialize(&window);

    // Build, compile and link the shaders
    struct Shader cube_shader_data[] = {
        (struct Shader){"./shaders/cube.vs", GL_VERTEX_SHADER},
        (struct Shader){"./shaders/cube.fs", GL_FRAGMENT_SHADER},
    };

    struct Shader light_cube_shader_data[] = {
        (struct Shader){"./shaders/light_cube.vs", GL_VERTEX_SHADER},
        (struct Shader){"./shaders/light_cube.fs", GL_FRAGMENT_SHADER},
    };

    struct Shader axis_shader_data[] = {
        (struct Shader){"./shaders/axis.vs", GL_VERTEX_SHADER},
        (struct Shader){"./shaders/axis.fs", GL_FRAGMENT_SHADER},
    };

    int cube_shader_data_count = sizeof(cube_shader_data)/sizeof(struct Shader);
    int light_cube_shader_data_count = sizeof(light_cube_shader_data)/sizeof(struct Shader);
    int axis_shader_data_count = sizeof(axis_shader_data)/sizeof(struct Shader);

    GLuint cube_shader_program = link_shaders(cube_shader_data, cube_shader_data_count);
    GLuint light_cube_shader_program = link_shaders(light_cube_shader_data, light_cube_shader_data_count);
    GLuint axis_shader_program = link_shaders(axis_shader_data, axis_shader_data_count);

    // Creating all the VAOs
    vertex_spec(vaos);

    struct Model model = load_model("./models/backpack/backpack.obj");

    struct TextureWrap default_wrap = {
        GL_REPEAT, GL_REPEAT, 
        GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR
    };
    
    // Loading textures
    struct Texture diffuse_map = {
        load_texture("./textures/container2.png", GL_RGBA, default_wrap, false),
        TEXTT_DIFFUSE
    };
    
    struct Texture specular_map = {
        load_texture("./textures/full_spec_map.png", GL_RGBA, default_wrap, false),
        TEXTT_SPECULAR
    };

    glUseProgram(cube_shader_program);
    glUniform1i(glGetUniformLocation(cube_shader_program, "material.diffuse"), 0);
    glUniform1i(glGetUniformLocation(cube_shader_program, "material.specular"), 1);
    
    camera = camera_create();
    glm_vec3_add(camera->pos, (vec3){0.0f, -3.0, 3.0}, camera->pos);

    // related to FPS/deltatime calculation
    double time_start, time_end;
    double fps_values[FPS_CAP];
    uint64_t frames_elapsed = 0;

    int fps_values_length = sizeof(fps_values) / sizeof(double);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        time_start = glfwGetTime();

        // Input
        handle_input(window);

        // Declaring transformation matrices
        struct MVP mvp = create_mvp();

        // View matrix is defined by the camera
        camera_update_direction(camera);
        camera_get_view_matrix(camera, &mvp.view);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuse_map.id);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specular_map.id);

        glm_vec3_copy(camera->pos, camera_original_pos);

        if (enable_3d) {
            // Basically this moves the camera left and right and recalculates its view matrix
            // to get a different image on each side of the screen, resulting in the 3D effect

            // The size of the viewport changes when 3D mode is enabled, so the proj matrix should account for that
            glm_perspective(glm_rad(camera->fov), (window_width/2.0f)/(double)window_height, 0.01, 100, mvp.projection);

            // Left half of the window
            glViewport(0, 0, window_width/2, window_height);
            camera_shift(camera, (vec3){0.0f, D3_EYE_DISTANCE, 0.0f});
            camera_get_view_matrix(camera, &mvp.view);
            render_full(mvp, model, cube_shader_program, light_cube_shader_program, axis_shader_program);
            camera_shift(camera, (vec3){0.0f, -D3_EYE_DISTANCE, 0.0f});

            // Right half of the window
            glViewport(window_width/2, 0, window_width/2, window_height);
            camera_shift(camera, (vec3){0.0f, -D3_EYE_DISTANCE, 0.0f});
            camera_get_view_matrix(camera, &mvp.view);
            render_full(mvp, model, cube_shader_program, light_cube_shader_program, axis_shader_program);
            camera_shift(camera, (vec3){0.0f, D3_EYE_DISTANCE, 0.0f});
        } else {
            // Full window
            glm_perspective(glm_rad(camera->fov), (double)window_width/(double)window_height, 0.01, 100, mvp.projection);

            glViewport(0, 0, window_width, window_height);
            render_full(mvp, model, cube_shader_program, light_cube_shader_program, axis_shader_program);
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

        struct timespec sleep_time;

        sleep_time.tv_sec = 0;
        sleep_time.tv_nsec = ((1.0f / FPS_CAP) - time_elapsed) * 1000000000.0f;
        sleep_time.tv_nsec = glm_max(sleep_time.tv_nsec, 0);

        nanosleep(&sleep_time, NULL);

        time_elapsed += sleep_time.tv_nsec / 1000000000.0f;
        double fps = 1 / time_elapsed;

        fps_values[frames_elapsed] = fps;
        frames_elapsed += 1;
        
        if (frames_elapsed % FPS_CAP == 0) {
            double avg_fps = sum_array(fps_values, fps_values_length) / fps_values_length;
            printf("\rFPS: %.2f    ", avg_fps);
            fflush(stdout);

            frames_elapsed = 0;
        }
    }

    printf("\n");

    dynarray_free(vaos);
    camera_destroy(camera);

    glfwTerminate();

    return EXIT_SUCCESS;
}