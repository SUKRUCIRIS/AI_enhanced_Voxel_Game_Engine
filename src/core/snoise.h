#pragma once
#include "dynamic.h"

float snoise2(float x, float y);

// simplex points should start from 0 and end at 1, must be a float dynamic array.
// corresponding heights should be an integer dynamic array and must have same size as simplex points
// precision heightmap should be created as same size before
int **create_heightmap(int dimensionx, int dimensionz, int seedx, int seedz, float precision, int border_add,
                       DA *simplex_points, DA *corresponding_heights);