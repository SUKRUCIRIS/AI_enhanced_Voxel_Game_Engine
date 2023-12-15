#pragma once
#include "../core/core.h"

typedef struct world_batch
{
	br_object_manager *obj_manager;
	br_texture_manager *tex_manager;
} world_batch;

world_batch *create_world_batch(int **hm, int startx, int startz, int widthx, int widthz, int dimensionx, int dimensionz);

void use_world_batch(world_batch *w, GLuint program);

void delete_world_batch(world_batch *w);