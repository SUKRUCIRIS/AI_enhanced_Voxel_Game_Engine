#pragma once
#include "./core/object.h"
#include "./core/texture.h"

int **create_random_world(int dimensionx, int dimensionz, int maxy, int miny);

object **create_world_cubes(int **world);