#pragma once
#include "../../third_party/opengl/include/glad/glad.h"
#include "dynamic.h"

typedef struct br_texture_manager
{
	DA *textures;
	int indices[32];
	DA *programs; // i will save uniforms here. i wont find their locations everytime i render for performance
	DA *uniforms;
} br_texture_manager;

typedef struct br_texture
{
	GLuint id;
	GLenum type;
	br_texture_manager *manager;
} br_texture;

br_texture_manager *create_br_texture_manager(void);

// texture slot 31, 30, 29, 28, 27 are reserved for shadowmap and gbuffer, use [0-26]
br_texture *create_br_texture(br_texture_manager *manager, const char *path, GLenum texType,
															GLint min_filter, GLint mag_filter, int index, int wraps, int wrapt);

// texture slot 31, 30, 29, 28, 27 are reserved for shadowmap and gbuffer, use [0-26]
br_texture *create_br_texture_memory(br_texture_manager *manager, unsigned char *data, int width, int height, GLenum texType,
																		 GLint min_filter, GLint mag_filter, int index, int wraps, int wrapt);

void delete_br_texture(br_texture *texture);

void delete_br_texture_manager(br_texture_manager *manager);

void use_br_texture_manager(br_texture_manager *manager, GLuint program);