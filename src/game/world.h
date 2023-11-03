#pragma once
#include "./core/core.h"

typedef struct world
{
	br_object_manager *obj_manager;
	br_texture_manager *tex_manager;
} world;

int **create_heightmap(int dimensionx, int dimensionz, int maxy, int seedx, int seedy);

world *create_world(int **hm, int dimensionx, int dimensionz);

void use_world(world *w, GLuint program);

void delete_world(world *w);