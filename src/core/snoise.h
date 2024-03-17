#pragma once
#include "dynamic.h"

float snoise2(float x, float y);

// simplex points should start from 0 and end at 1, must be a float dynamic array.
// corresponding heights should be an integer dynamic array and must have same size as simplex points
// precision heightmap should be created as same size before
// lacunarity > 1, 0 < persistence < 1, octaves > 1, amplitude > 1
// if you supply points dynamic arrays, maxheight wont be used and vice versa
int **create_heightmap(int dimensionx, int dimensionz, int seedx, int seedz, float precision, int border_add,
                       DA *simplex_points, DA *corresponding_heights, int octaves, float amplitude,
                       float lacunarity, float persistence, int maxheightmult);

// float *create_points_heightmap(int **hm, int dimensionx, int dimensionz, int startx, int startz, int widthx, int widthz);

int **create_heightmap_texture(const char *path, int maxheightmult, int border_add, int result_dimensionx, int result_dimensionz);