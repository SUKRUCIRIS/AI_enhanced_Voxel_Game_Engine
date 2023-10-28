#pragma once
#include "../../third_party/opengl/include/glad/glad.h"
#include "dynamic.h"

typedef struct texture
{
	GLuint id;
	GLenum type;
	float shininess;
	DA *programs; // i will save uniforms here. i wont find their locations everytime i render for performance
	DA *uniforms;
} texture;

texture *load_texture(const char *path, GLenum texType, GLenum pixelType, GLint min_filter, GLint mag_filter, float shininess);

void use_texture(texture *tex, GLuint program);

void delete_texture(texture *tex);