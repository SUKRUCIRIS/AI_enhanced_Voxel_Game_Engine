#pragma once
#include "br_object.h"
#include "br_texture.h"

typedef struct water
{
  br_object_manager *obj;
} water;

water *create_water(float sealevel, int **hm, const char *texture_path, int startx,
                    int startz, int widthx, int widthz, int dimensionx, int dimensionz, unsigned char create_physic);

void delete_water(water *w);

void use_water(water *w, GLuint program);

void delete_water_texture_manager(void);