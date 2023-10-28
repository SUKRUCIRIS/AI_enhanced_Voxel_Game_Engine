#include "world.h"
#include "snoise.h"

GLuint indices_no_normal_has_texture[] = {
	5, 1, 0, 6, 1, 5,
	6, 2, 1, 7, 2, 6,
	7, 3, 2, 8, 3, 7,
	8, 4, 3, 9, 4, 8,
	0, 11, 10, 1, 11, 0,
	12, 6, 5, 13, 6, 12};

GLfloat vertices_no_normal_has_texture[] = {
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

GLfloat vertices_has_normal_has_texture[] = {
	-1, -1, -1, 0, 0, 0, 0, 1, // A 0
	1, -1, -1, 1, 0, 0, 0, 1,  // B 1
	1, 1, -1, 1, 1, 0, 0, 1,   // C 2
	-1, 1, -1, 0, 1, 0, 0, 1,  // D 3
	-1, -1, 1, 0, 0, 0, 0, -1, // E 4
	1, -1, 1, 1, 0, 0, 0, -1,  // F 5
	1, 1, 1, 1, 1, 0, 0, -1,   // G 6
	-1, 1, 1, 0, 1, 0, 0, -1,  // H 7

	-1, 1, -1, 0, 0, -1, 0, 0,	// D 8
	-1, -1, -1, 1, 0, -1, 0, 0, // A 9
	-1, -1, 1, 1, 1, -1, 0, 0,	// E 10
	-1, 1, 1, 0, 1, -1, 0, 0,	// H 11
	1, -1, -1, 0, 0, 1, 0, 0,	// B 12
	1, 1, -1, 1, 0, 1, 0, 0,	// C 13
	1, 1, 1, 1, 1, 1, 0, 0,		// G 14
	1, -1, 1, 0, 1, 1, 0, 0,	// F 15

	-1, -1, -1, 0, 0, 0, -1, 0, // A 16
	1, -1, -1, 1, 0, 0, -1, 0,	// B 17
	1, -1, 1, 1, 1, 0, -1, 0,	// F 18
	-1, -1, 1, 0, 1, 0, -1, 0,	// E 19
	1, 1, -1, 0, 0, 0, 1, 0,	// C 20
	-1, 1, -1, 1, 0, 0, 1, 0,	// D 21
	-1, 1, 1, 1, 1, 0, 1, 0,	// H 22
	1, 1, 1, 0, 1, 0, 1, 0,		// G 23
};
// index data
GLuint indices_has_normal_has_texture[] = {
	// front and back
	2, 3, 0,
	0, 1, 2,
	6, 5, 4,
	4, 7, 6,
	// left and right
	9, 8, 11,
	11, 10, 9,
	14, 13, 12,
	12, 15, 14,
	// bottom and top
	18, 17, 16,
	16, 19, 18,
	22, 21, 20,
	20, 23, 22};

texture *text = 0;

int **create_heightmap(int dimensionx, int dimensionz, int maxy, int seedx, int seedz)
{
	int **hm = malloc(sizeof(int *) * dimensionx);
	for (int i = 0; i < dimensionx; i++)
	{
		hm[i] = malloc(sizeof(int) * dimensionz);
		for (int i2 = 0; i2 < dimensionz; i2++)
		{
			hm[i][i2] = (int)floorf(snoise2((float)(seedx + i), (float)(seedz + i2)) * maxy);
			if (hm[i][i2] < 0)
			{
				hm[i][i2] = 0;
			}
		}
	}

	return hm;
}

DA *create_world_cubes(int **hm, int dimensionx, int dimensionz)
{
	DA *world_cubes = create_DA(sizeof(object *));
	object *tmp = 0;
	for (int i = 0; i < dimensionx; i++)
	{
		for (int i2 = 0; i2 < dimensionz; i2++)
		{
			if (i == 0 && i2 == 0)
			{
				tmp = create_object(vertices_has_normal_has_texture, 24, GL_STATIC_DRAW,
									indices_has_normal_has_texture, 36, 1, 1, 1, 3, 10, 0.1f, 0.5f);
			}
			else
			{
				tmp = create_object_copy(((object **)get_data_DA(world_cubes))[0], 1);
			}
			translate_object(tmp, (vec3){(float)(i - (int)(dimensionx / 2)), (float)hm[i][i2], (float)(i2 - (int)(dimensionz / 2))}, 1);
			scale_object(tmp, (vec3){0.5f, 0.5f, 0.5f}, 1);
			pushback_DA(world_cubes, &tmp);
			if (hm[i][i2] != 0)
			{
				for (int i3 = 0; i3 < hm[i][i2]; i3++)
				{
					tmp = create_object_copy(((object **)get_data_DA(world_cubes))[0], 1);
					translate_object(tmp, (vec3){(float)(i - (int)(dimensionx / 2)), (float)i3, (float)(i2 - (int)(dimensionz / 2))}, 1);
					scale_object(tmp, (vec3){0.5f, 0.5f, 0.5f}, 1);
					pushback_DA(world_cubes, &tmp);
				}
			}
		}
	}
	return world_cubes;
}

void use_world(DA *world_cubes, GLuint program, camera *cam)
{
	use_texture(text);
	object **tmp = get_data_DA(world_cubes);
	for (unsigned int i = 0; i < get_size_DA(world_cubes); i++)
	{
		use_object(tmp[i], program, cam);
	}
}

void init_world_textures(void)
{
	if (text == 0)
	{
		text = load_texture("./textures/grass.jpg", GL_TEXTURE_2D, GL_UNSIGNED_BYTE, GL_NEAREST, GL_NEAREST);
	}
}

void delete_world_textures(void)
{
	delete_texture(text);
	text = 0;
}