#pragma once
#include "br_object.h"
#include "br_texture.h"

typedef struct water
{
  br_object_manager *obj;
  br_texture_manager *texture;
} water;

water *create_water(float sealevel, const char *texture_path, int startx, int startz, int widthx, int widthz);

void delete_water(water *w);

void use_water(water *w, GLuint program);