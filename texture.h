#pragma once
typedef struct texture texture;

texture* load_texture(const char* path, GLenum texType, GLenum pixelType, GLint min_filter, GLint mag_filter);

void use_texture(texture* tex);

void delete_texture(texture* tex);