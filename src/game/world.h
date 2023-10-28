#pragma once
#include "./core/object.h"
#include "./core/texture.h"
#include "./core/camera.h"

int **create_heightmap(int dimensionx, int dimensionz, int maxy, int seedx, int seedy);

DA *create_world_cubes(int **hm, int dimensionx, int dimensionz);

void use_world(DA *world_cubes, GLuint program);

void init_world_textures(void);

void delete_world_textures(void);