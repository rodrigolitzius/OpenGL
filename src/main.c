#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <glad.h>
#include <GLFW/glfw3.h>

#include <main.h>

void finish();

// Print an error with glfw and exit
void exit_with_error_glfw(const char* error_text) {
    const char* error_description = NULL;
    glfwGetError(&error_description);

    fprintf(stderr, "\n[E] ERROR: %s\n%s\n\n", error_text, (char*) error_description);
    finish();
    exit(EXIT_FAILURE);
}

// Print a generic error and exit
void exit_with_error_generic(const char* error_text, const char* error_description) {
    fprintf(stderr, "\n[E] ERROR: %s\n%s\n\n", error_text, error_description);
    finish();
    exit(EXIT_FAILURE);
}

void finish() {
    glfwTerminate();
}

// Should run when the size of the window is changed
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
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

    // Intiializing GLAD
    return_value = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    if (!return_value) {
        exit_with_error_generic("Failed to load GLAD.", "");
    }

    // OpenGL stuff
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}

// Handles the input, duh
void handle_input(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        printf("Input handling is working!\n");
    }
}

// Reads all contents of a file
void* file_read_all(const char* file_path) {
    FILE* file = fopen(file_path, "r");
    // Finding the size of the file in bytes
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocating enough memory to hold the data in the file
    void* contents = malloc(file_size);

    // Reading all the data
    fread(contents, 1, file_size, file);

    // Closing file
    fclose(file);

    return contents;
}

// Compile a GLSL shader
GLuint compile_shader(const GLchar* source, GLenum type) {
    // Creating the shader object
    GLuint shader;
    shader = glCreateShader(type);

    // COmpiling from the source code
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    // Checking for erros in the compilation
    GLint shader_compiled = GL_TRUE;
    char compiling_log[8192];

    glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_compiled);

    if (shader_compiled == GL_FALSE) {
        glGetShaderInfoLog(shader, 8192, NULL, compiling_log);
        exit_with_error_generic("Failed to compile shader", compiling_log);
    }

    return shader;
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

    return shader_program;
}

int main() {
    // Initializing
    GLFWwindow* window;

    initialize(&window);

    // Build, compile and link the shaders
    GLuint shader_program = link_shaders();

    // Defining each vertex
    double vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f
    };

    // Setting VBO do draw the defined vertices
    // and VAO to remember what to draw
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 3*sizeof(double), (void*)0);
    glEnableVertexAttribArray(0);  

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Input
        handle_input(window);

        // Clearing the screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Drawing with VAO
        glUseProgram(shader_program);
        glBindVertexArray(vao);

        // Finally drawing the vertices
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Updating window
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    finish();

    return EXIT_SUCCESS;
}