#pragma once
#include "br_object.h"

// chars start from 32
typedef struct font
{
  GLuint font_textures;
  br_object_manager *text_quads;
  int sizex[95];
  int sizey[95];
  int bearingx[95];
  int bearingy[95];
  unsigned int advance;
} font;

font *create_font(const char *font_file, int width, int height);

void delete_font(font *f);