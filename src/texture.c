#include <stdbool.h>

#include <include_glad.h>

#include "stb_image.h"

#include "texture.h"
#include "functions.h"

GLuint load_texture(const char* file_path, GLenum format, struct TextureWrap wrap, bool flip_v) {
    int texture_width, texture_height, nr_channels;

    stbi_set_flip_vertically_on_load(flip_v);

    unsigned char *texture_data = stbi_load(file_path, &texture_width, &texture_height, &nr_channels, 0);

    if (!texture_data) {
        exit_with_error_generic("Could not load image", "");
    }

    GLuint texture;
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    // First: Sampling
    // Second: Mipmap
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap.s_wrap);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap.t_wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, wrap.mag_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, wrap.min_mipmap_filter);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, format, GL_UNSIGNED_BYTE, texture_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(texture_data);

    return texture;
}