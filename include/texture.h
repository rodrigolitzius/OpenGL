#ifndef TDE_TEXTURE
#define TDE_TEXTURE

#include <stdbool.h>

#include <glad.h>

struct TextureWrap {
    GLenum s_wrap; GLenum t_wrap; GLenum mag_filter; GLenum min_mipmap_filter;
};

// Returns a texture object
GLuint load_texture(const char* file_path, GLenum format, struct TextureWrap wrap, bool flip_v);

#endif