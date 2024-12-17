#include <stdlib.h>
#include <string.h>

#include <glad.h>
#include "main.h"
#include "dynamic_array.h"

void vertex_spec(struct DynamicArray* vaos) {
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

    dynarray_add(vaos, vao_1);

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

    dynarray_add(vaos, vao_2);

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

    dynarray_add(vaos, vao_3);
}