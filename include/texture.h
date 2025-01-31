#ifndef TDE_TEXTURE
#define TDE_TEXTURE

#include <stdbool.h>

#include <glad.h>

struct TextureWrap {
    GLenum s_wrap; GLenum t_wrap; GLenum mag_filter; GLenum min_mipmap_filter;
};

enum TEXTURE_TYPES {
    // Affected by diffuse lighting
    TEXTT_DIFFUSE = 0,
    // Affects specular lighting
    TEXTT_SPECULAR = 0,
};

// Represents a loaded texture
struct Texture {
    GLuint id; int type;
};

// Returns a texture object
GLuint load_texture(const char* file_path, GLenum format, struct TextureWrap wrap, bool flip_v);

#endif