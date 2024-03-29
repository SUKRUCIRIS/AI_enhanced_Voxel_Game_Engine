#pragma once
#include "../../third_party/opengl/include/glad/glad.h"
#include "dynamic.h"
#include "../../third_party/cglm/include/cglm/cglm.h"

// chars start from 32
typedef struct text_manager
{
  GLuint font_textures;
  GLuint VAO, VBO, EBO;
  DA *vertices; // 3 vertex coord, 2 texture coord, 3 rgb
  DA *indices;
  int sizex[95];
  int sizey[95];
  int bearingx[95];
  int bearingy[95];
  int advancex[95];
  float xoffset[95];
  mat4 projection;
  DA *programs; // i will save uniforms here. i wont find their locations everytime i render for performance
  DA *uniforms;
  unsigned char newdata;
  int screenwidth, screenheight;
  int realsw, realsh;
  unsigned int twidth, theight;
} text_manager;

text_manager *create_text_manager(const char *font_file, int height, int screenwidth, int screenheight,
                                  int realsw, int realsh, GLint min_filter, GLint mag_filter);

void delete_text_manager(text_manager *f);

void add_text(text_manager *f, float startx, float starty, float z, int scale, vec4 rgba, const char *text);

void add_text_variadic(text_manager *f, float startx, float starty, float z, int scale, vec4 rgba, const char *text, ...);

void get_text_size(text_manager *f, int scale, const char *text, float *width, float *height);

void get_text_size_variadic(text_manager *f, int scale, float *width, float *height, const char *text, ...);

void use_text_manager(text_manager *f, GLuint program);

void clear_text_manager(text_manager *f);