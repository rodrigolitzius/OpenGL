#ifndef TDE_MODEL
#define TDE_MODEL

#include <cglm/cglm.h>
#include <texture.h>

struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 text_coords;
};

struct Mesh {
    GLuint vao, vbo, ebo;
    
    struct Vertex* vertices;
    unsigned int vertex_count;

    unsigned int* indices;
    unsigned int index_count;

    struct Texture* textures;
    unsigned int texture_count;
};

struct Model {
    struct Mesh* meshes;
    unsigned int mesh_count;
};

struct Model load_model(const char* path);
void draw_model(struct Model model, int shader_program);

#endif