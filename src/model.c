#include <model.h>
#include <glad.h>

#include <functions.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <cglm/cglm.h>

void setup_mesh(struct Mesh* mesh) {
    glGenVertexArrays(1, &mesh->vao);
    glGenBuffers(1, &mesh->vbo);
    glGenBuffers(1, &mesh->ebo);

    glBindVertexArray(mesh->vao);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);

    glBufferData(GL_ARRAY_BUFFER, mesh->vertex_count*sizeof(struct Vertex), mesh->vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->index_count*sizeof(unsigned int), mesh->indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (void*)0);                 // Positions
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (void*)(3*sizeof(float))); // Normals
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (void*)(6*sizeof(float))); // Texture coords

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

struct Model load_model(const char* path) {
    // Importing model
    const struct aiScene* scene = aiImportFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs);

    if (!scene) {
        exit_with_error_generic("Failed to load model: ", aiGetErrorString());
    }

    struct Model model;

    model.mesh_count = scene->mNumMeshes;
    model.meshes = malloc(sizeof(struct Mesh) * model.mesh_count);

    // Retrieving data for rendering from every mesh
    for (int i=0; i<scene->mNumMeshes; i++) {
        struct aiMesh* ai_mesh = scene->mMeshes[i];

        unsigned int vertex_count = ai_mesh->mNumVertices;
        unsigned int index_count = ai_mesh->mNumFaces*3; // times 3 beacuse every face has 3 vertices

        struct Vertex* vertices = calloc(vertex_count*sizeof(struct Vertex), 1);
        unsigned int* indices = calloc(index_count*sizeof(unsigned int), 1);

        // Retrieving data
        for (int j=0; j<ai_mesh->mNumVertices; j++) {
            // Positions
            struct aiVector3D vertex = ai_mesh->mVertices[j];
            glm_vec3_copy((vec3){vertex.x, vertex.y, vertex.z}, vertices[j].position);

            // Normals
            struct aiVector3D normal = ai_mesh->mNormals[j];
            glm_vec3_copy((vec3){normal.x, normal.y, normal.z}, vertices[j].normal);

            // Texture coordinates
            if (ai_mesh->mTextureCoords[0]) {
                struct aiVector3D text_coord = ai_mesh->mTextureCoords[0][j];
                glm_vec2_copy((vec2){text_coord.x, text_coord.y}, vertices[j].text_coords);
            }
        }

        // Indices
        unsigned int index = 0;
        for (int j=0; j<ai_mesh->mNumFaces; j++) {
            struct aiFace face = ai_mesh->mFaces[j];

            for (int k=0; k<face.mNumIndices; k++) {
                indices[index] = face.mIndices[k];
                index++;
            }
        }

        // Generating a mesh from the retrieved data
        struct Mesh mesh;
        mesh.vertex_count = vertex_count;
        mesh.vertices = vertices;

        mesh.index_count = index_count;
        mesh.indices = indices;

        setup_mesh(&mesh);

        // Copying it to model
        memcpy(model.meshes+i, &mesh, sizeof(mesh));
    }

    return model;
}

void draw_mesh(struct Mesh mesh, int shader_program) {
    glUseProgram(shader_program);

    glBindVertexArray(mesh.vao);

    glDrawElements(GL_TRIANGLES, mesh.index_count, GL_UNSIGNED_INT, mesh.indices);
    glBindVertexArray(0);
}

void draw_model(struct Model model, int shader_program) {
    for (int i=0; i<model.mesh_count; i++) {
        struct Mesh mesh = model.meshes[i];

        draw_mesh(mesh, shader_program);
    }
}
