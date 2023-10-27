#include "world.h"
#include "snoise.h"

GLuint indices[] = {
	5, 1, 0, 6, 1, 5,
	6, 2, 1, 7, 2, 6,
	7, 3, 2, 8, 3, 7,
	8, 4, 3, 9, 4, 8,
	0, 11, 10, 1, 11, 0,
	12, 6, 5, 13, 6, 12};

GLfloat vertices[] = {
	-1, -1, -1, 0, 0,
	1, -1, -1, 1, 0,
	1, 1, -1, 2, 0,
	-1, 1, -1, 3, 0,
	-1, -1, -1, 4, 0,

	-1, -1, 1, 0, 1,
	1, -1, 1, 1, 1,
	1, 1, 1, 2, 1,
	-1, 1, 1, 3, 1,
	-1, -1, 1, 4, 1,

	-1, 1, -1, 0, -1,
	1, 1, -1, 1, -1,

	-1, 1, 1, 0, 2,
	1, 1, 1, 1, 2};

int **create_random_world(int dimensionx, int dimensionz, int maxy, int miny)
{
}

object **create_world_cubes(int **world)
{
}
