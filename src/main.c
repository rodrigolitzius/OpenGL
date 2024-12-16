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
#include "texture.h"
#include "camera.h"

#include <main.h>

bool draw_wireframe = false;

struct VaoData** vaos;
unsigned int vao_count = 0;
unsigned int current_vao = 0;

int window_width;
int window_height;

double mouse_last_x = WINDOW_WIDTH/2.0f;
double mouse_last_y = WINDOW_HEIGHT/2.0f;

struct camera* camera;
double fov = INITIAL_FOV;

// Should run when the size of the window is changed
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
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

    // Change VAO
    if (key == GLFW_KEY_V && action == GLFW_PRESS) {
        if (current_vao == vao_count-1) {
            current_vao = 0;
        } else {
            current_vao += 1;
        }

        printf("Switched to VAO \"%s\"\n", vaos[current_vao]->name);
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
        direction[FORWARD] = MOV_SPEED;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        direction[FORWARD] = -MOV_SPEED;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        direction[RIGHT] = -MOV_SPEED;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        direction[RIGHT] = MOV_SPEED;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        direction[UP] = -MOV_SPEED;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        direction[UP] = MOV_SPEED;
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

    // Intiializing GLAD
    return_value = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    if (!return_value) {
        exit_with_error_generic("Failed to load GLAD.", "");
    }

    // OpenGL stuff
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glEnable(GL_DEPTH_TEST);
}

// Links shaders to a shader program
GLuint link_shaders() {
    // Getting the source for each shader and compiling them
    const char* vertex_shader_src = file_read_all("./src/vertex_shader");
    GLuint vertex_shader = compile_shader(vertex_shader_src, GL_VERTEX_SHADER);

    const char* fragment_shader_src = file_read_all("./src/fragment_shader");
    GLuint fragment_shader = compile_shader(fragment_shader_src, GL_FRAGMENT_SHADER);

    // Linking the shaders
    GLuint shader_program;
    shader_program = glCreateProgram();

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);

    glLinkProgram(shader_program);

    // Deleting the shaders since we don't need them anymore
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    // Checking for errors in the linking process
    int linked_successfully = GL_TRUE;
    char linking_log[8192];
    glGetProgramiv(shader_program, GL_LINK_STATUS, &linked_successfully);

    if (linked_successfully == GL_FALSE) {
        glGetProgramInfoLog(shader_program, 8192, NULL, linking_log);
        exit_with_error_generic("Failed to link shaders.", linking_log);
    }

    // Freeing the shader sources
    free((void*)fragment_shader_src);
    free((void*)vertex_shader_src);

    return shader_program;
}

void add_vao(struct VaoData* vao) {
    // Just a dynamic array
    vaos = realloc(vaos, (vao_count+1)*sizeof(struct VaoData));
    vao_count += 1;

    vaos[vao_count-1] = vao;
}

void vertex_spec() {
    double vertices1[] = {
        -0.5f, -0.5f, -0.5f,  0, 1, 0,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0, 0, 1,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1, 0, 0,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,   1, 0, 0,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  1, 1, 0,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0, 1, 0,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0, 1, 0,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0, 0, 1,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1, 0, 0,1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,   1, 0, 0,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  1, 1, 0,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0, 1, 0,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  0, 1, 0,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0, 0, 1,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  1, 0, 0,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  1, 0, 0,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  1, 1, 0,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0, 1, 0,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  0, 1, 0,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0, 0, 1,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1, 0, 0,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,   1, 0, 0,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1, 1, 0,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0, 1, 0,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0, 1, 0,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0, 0, 1,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1, 0, 0,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,   1, 0, 0,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  1, 1, 0,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0, 1, 0,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0, 1, 0,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0, 0, 1,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1, 0, 0,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1, 0, 0,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1, 1, 0,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0, 1, 0,  0.0f, 1.0f
    };

    GLuint vertex_count1 = ((sizeof(vertices1) / sizeof(double)) / 8);

    // Setting VBO do draw the defined vertices
    // and VAO to remember what to draw
    GLuint vao1, vbo1;
    glGenVertexArrays(1, &vao1);
    glGenBuffers(1, &vbo1);

    glBindVertexArray(vao1);

    glBindBuffer(GL_ARRAY_BUFFER, vbo1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

    //glVertexAttribPointer(index, size, type, normalized, stride, offset_pointer);
    glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 8*sizeof(double), (void*)0);
    glVertexAttribPointer(1, 3, GL_DOUBLE, GL_FALSE, 8*sizeof(double), (void*)(3*sizeof(double)));
    glVertexAttribPointer(2, 2, GL_DOUBLE, GL_FALSE, 8*sizeof(double), (void*)(6*sizeof(double)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    struct VaoData temp1 = {
        vao1, vertex_count1, "Main", true
    };

    struct VaoData *vao_1 = malloc(sizeof(struct VaoData));
    memcpy(vao_1, &temp1, sizeof(struct VaoData));

    add_vao(vao_1);

    //////////////////////
    //////////////////////
    //////////////////////

    double vertices2[] = {
        // Vertices         // Colors
        -1.0f, 0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
        -0.0f, 0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,

        1.0f, 0.5f, 0.0f,   1.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f,   0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,
    };
    GLuint vertex_count2 = (sizeof(vertices1) / sizeof(double)) / 6;


    // Setting VBO do draw the defined vertices
    // and VAO to remember what to draw
    GLuint vao2, vbo2;
    glGenVertexArrays(1, &vao2);
    glGenBuffers(1, &vbo2);

    glBindVertexArray(vao2);

    glBindBuffer(GL_ARRAY_BUFFER, vbo2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

    //glVertexAttribPointer(index, size, type, normalized, stride, offset_pointer);
    glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 6*sizeof(double), (void*)0);
    glVertexAttribPointer(1, 3, GL_DOUBLE, GL_FALSE, 6*sizeof(double), (void*)(3*sizeof(double)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    struct VaoData temp2 = {
        vao2, vertex_count2, "Inverted triangles", false
    };

    struct VaoData *vao_2 = malloc(sizeof(struct VaoData));
    memcpy(vao_2, &temp2, sizeof(struct VaoData));

    add_vao(vao_2);

    //////////////////////
    //////////////////////
    //////////////////////

    double vertices3[] = {
        // Vertices          // Colors
        0.0f, 0.5f, 0.0f,    1.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 1.0f,
    };
    GLuint vertex_count3 = (sizeof(vertices3) / sizeof(double)) / 6;

    // Setting VBO do draw the defined vertices
    // and VAO to remember what to draw
    GLuint vao3, vbo3;
    glGenVertexArrays(1, &vao3);
    glGenBuffers(1, &vbo3);

    glBindVertexArray(vao3);

    glBindBuffer(GL_ARRAY_BUFFER, vbo3);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices3), vertices3, GL_STATIC_DRAW);

    //glVertexAttribPointer(index, size, type, normalized, stride, offset_pointer);
    glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 6*sizeof(double), (void*)0);
    glVertexAttribPointer(1, 3, GL_DOUBLE, GL_FALSE, 6*sizeof(double), (void*)(3*sizeof(double)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    struct VaoData temp3 = {
        vao3, vertex_count3, "CMY triangle", false
    };

    struct VaoData *vao_3 = malloc(sizeof(struct VaoData));
    memcpy(vao_3, &temp3, sizeof(struct VaoData));

    add_vao(vao_3);
}

int main() {
    // Initializing
    GLFWwindow* window;

    initialize(&window);

    glfwGetFramebufferSize(window, &window_width, &window_height);

    // test();

    // Build, compile and link the shaders
    GLuint shader_program = link_shaders();

    // Creating all the VAOs
    vertex_spec();

    struct TextureWrap texture1_wrap = {
        GL_REPEAT, GL_REPEAT,
        GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR
    };

    struct TextureWrap texture2_wrap = {
        GL_REPEAT, GL_MIRRORED_REPEAT,
        GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR
    };

    GLuint texture1 = load_texture("./textures/container.jpg", GL_RGB, texture1_wrap, false);
    GLuint texture2 = load_texture("./textures/awesomeface.png", GL_RGBA, texture2_wrap, true);

    // The VAO to use when drawing
    struct VaoData vao;

    glUseProgram(shader_program);
    glUniform1i(glGetUniformLocation(shader_program, "texture1"), 0);
    glUniform1i(glGetUniformLocation(shader_program, "texture2"), 1);

    GLuint blend_variable = glGetUniformLocation(shader_program, "blend");
    glUniform1f(blend_variable, 0.3);

    vec3 cubePositions[] = {
        {0.0f,  0.0f,  0.0f}, 
        {2.0f,  5.0f, -15.0f}, 
        {-1.5f, -2.2f, -2.5f},
        {-3.8f, -2.0f, -12.3f},
        {2.4f, -0.4f, -3.5f},
        {-1.7f,  3.0f, -7.5f},
        {1.3f, -2.0f, -2.5f},
        {1.5f,  2.0f, -2.5f},
        {1.5f,  0.2f, -1.5f},
        {-1.3f,  1.0f, -1.5f},
    };

    camera = camera_create();
    camera_move(camera, (vec3){0.0f, 0.0f, 3.0f});

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        mat4 model_mat, view_mat, projection_mat;
        glm_mat4_identity(model_mat);
        glm_mat4_identity(view_mat);
        glm_mat4_identity(projection_mat);

        glm_translate(view_mat, camera->pos);
        glm_perspective(glm_rad(fov), (double)window_width/(double)window_height, 0.1, 100, projection_mat);

        vec3 camera_right, camera_up;
        camera_get_up_and_right(camera, &camera_right, &camera_up);

        camera_update_direction(camera);

        vec3 look_at;
        glm_vec3_add(camera->pos, camera->direction, look_at);
        glm_lookat(camera->pos, look_at, camera_up, view_mat);

        glUniformMatrix4fv(glad_glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, *model_mat);
        glUniformMatrix4fv(glad_glGetUniformLocation(shader_program, "view"), 1, GL_FALSE, *view_mat);
        glUniformMatrix4fv(glad_glGetUniformLocation(shader_program, "projection"), 1, GL_FALSE, *projection_mat);

        // Selecting the right VAO to draw
        vao = *vaos[current_vao];

        // Telling the fragment shader if texture coordinate information is in the VBO 
        // so that it doesn't try to sample textures when there is none
        glUniform1i(glGetUniformLocation(shader_program, "use_texture"), vao.has_texture);

        // Input
        handle_input(window);

        // Clearing the screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Drawing with VAO
        glUseProgram(shader_program);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        glBindVertexArray(vao.vao);

        // Finally drawing the vertices
        for (int i=0; i<9; i++) {
            double rotation_speed = i + 0.5;
            if ((i % 2) == 0) {
                rotation_speed *= -1;
            }

            rotation_speed /= 2;

            glm_mat4_identity(model_mat);
            glm_translate(model_mat, cubePositions[i]);
            glm_rotate(model_mat, glfwGetTime()*rotation_speed, (vec3){1.0f, 0.5f, 0.0f});
            glUniformMatrix4fv(glad_glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, *model_mat);

            glDrawArrays(GL_TRIANGLES, 0, vao.vertex_count);
        }

        // Updating window
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    for (int i=0; i < vao_count; i++) {
        free(vaos[i]);
    }

    glfwTerminate();

    return EXIT_SUCCESS;
}