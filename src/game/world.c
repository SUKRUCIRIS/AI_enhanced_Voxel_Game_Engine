#include "world.h"

GLfloat cube_vertices[] = {
	-1, -1, -1, 0, 0, 0, 0, 1, 0, // A 0
	-1, 1, -1, 0, 1, 0, 0, 1, 0,  // B 1
	1, 1, -1, 1, 1, 0, 0, 1, 0,	  // C 2
	1, -1, -1, 1, 0, 0, 0, 1, 0,  // D 3
	-1, -1, 1, 0, 0, 0, 0, -1, 0, // E 4
	1, -1, 1, 1, 0, 0, 0, -1, 0,  // F 5
	1, 1, 1, 1, 1, 0, 0, -1, 0,	  // G 6
	-1, 1, 1, 0, 1, 0, 0, -1, 0,  // H 7

	-1, 1, -1, 0, 0, -1, 0, 0, 0,  // D 8
	-1, -1, -1, 1, 0, -1, 0, 0, 0, // A 9
	-1, -1, 1, 1, 1, -1, 0, 0, 0,  // E 10
	-1, 1, 1, 0, 1, -1, 0, 0, 0,   // H 11
	1, -1, -1, 0, 0, 1, 0, 0, 0,   // B 12
	1, 1, -1, 1, 0, 1, 0, 0, 0,	   // C 13
	1, 1, 1, 1, 1, 1, 0, 0, 0,	   // G 14
	1, -1, 1, 0, 1, 1, 0, 0, 0,	   // F 15

	-1, -1, -1, 0, 0, 0, -1, 0, 0, // A 16
	1, -1, -1, 1, 0, 0, -1, 0, 0,  // B 17
	1, -1, 1, 1, 1, 0, -1, 0, 0,   // F 18
	-1, -1, 1, 0, 1, 0, -1, 0, 0,  // E 19
	1, 1, -1, 0, 0, 0, 1, 0, 0,	   // C 20
	-1, 1, -1, 1, 0, 0, 1, 0, 0,   // D 21
	-1, 1, 1, 1, 1, 0, 1, 0, 0,	   // H 22
	1, 1, 1, 0, 1, 0, 1, 0, 0,	   // G 23
};
// index data
GLuint cube_indices[] = {
	// front and back
	2, 1, 0,
	0, 3, 2,
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

br_object *create_top_surface(br_object_manager *x)
{
	return create_br_object(x, &(cube_vertices[180]), 4, cube_indices, 6, 0, 0, 3, 10, 0.1f, 0.5f);
}

br_object *create_bottom_surface(br_object_manager *x)
{
	return create_br_object(x, &(cube_vertices[144]), 4, cube_indices, 6, 0, 0, 3, 10, 0.1f, 0.5f);
}

br_object *create_right_surface(br_object_manager *x)
{
	return create_br_object(x, &(cube_vertices[108]), 4, cube_indices, 6, 0, 0, 3, 10, 0.1f, 0.5f);
}

br_object *create_left_surface(br_object_manager *x)
{
	return create_br_object(x, &(cube_vertices[72]), 4, cube_indices, 6, 0, 0, 3, 10, 0.1f, 0.5f);
}

br_object *create_back_surface(br_object_manager *x)
{
	return create_br_object(x, &(cube_vertices[36]), 4, cube_indices, 6, 0, 0, 3, 10, 0.1f, 0.5f);
}

br_object *create_front_surface(br_object_manager *x)
{
	return create_br_object(x, cube_vertices, 4, cube_indices, 6, 0, 0, 3, 10, 0.1f, 0.5f);
}

void create_surfaces(br_object_manager *x, int i, int i2, int i3, int dimensionx, int dimensionz, int **hm)
{
	br_object *tmp = 0;
	if (hm[i][i2] == i3)
	{
		tmp = create_top_surface(x);

		scale_br_object(tmp, (vec3){0.5f, 0.5f, 0.5f}, 1);
		translate_br_object(tmp, (vec3){(float)(i - (int)(dimensionx / 2)), (float)i3, (float)(i2 - (int)(dimensionz / 2))}, 1);
	}
	if (i2 > 0 && hm[i][i2 - 1] < i3)
	{
		tmp = create_front_surface(x);

		scale_br_object(tmp, (vec3){0.5f, 0.5f, 0.5f}, 1);
		translate_br_object(tmp, (vec3){(float)(i - (int)(dimensionx / 2)), (float)i3, (float)(i2 - (int)(dimensionz / 2))}, 1);
	}
	if (i2 < dimensionz - 1 && hm[i][i2 + 1] < i3)
	{
		tmp = create_back_surface(x);

		scale_br_object(tmp, (vec3){0.5f, 0.5f, 0.5f}, 1);
		translate_br_object(tmp, (vec3){(float)(i - (int)(dimensionx / 2)), (float)i3, (float)(i2 - (int)(dimensionz / 2))}, 1);
	}
	if (i > 0 && hm[i - 1][i2] < i3)
	{
		tmp = create_left_surface(x);

		scale_br_object(tmp, (vec3){0.5f, 0.5f, 0.5f}, 1);
		translate_br_object(tmp, (vec3){(float)(i - (int)(dimensionx / 2)), (float)i3, (float)(i2 - (int)(dimensionz / 2))}, 1);
	}
	if (i < dimensionx - 1 && hm[i + 1][i2] < i3)
	{
		tmp = create_right_surface(x);

		scale_br_object(tmp, (vec3){0.5f, 0.5f, 0.5f}, 1);
		translate_br_object(tmp, (vec3){(float)(i - (int)(dimensionx / 2)), (float)i3, (float)(i2 - (int)(dimensionz / 2))}, 1);
	}
}

world *create_world(int **hm, int dimensionx, int dimensionz)
{
	world *x = malloc(sizeof(world));
	x->obj_manager = create_br_object_manager();
	x->tex_manager = create_br_texture_manager();

	// textures
	create_br_texture(x->tex_manager, "./textures/grass.jpg", GL_TEXTURE_2D, GL_UNSIGNED_BYTE, GL_NEAREST, GL_NEAREST, 0);

	// objects
	for (int i = 0; i < dimensionx; i++)
	{
		for (int i2 = 0; i2 < dimensionz; i2++)
		{
			create_surfaces(x->obj_manager, i, i2, hm[i][i2], dimensionx, dimensionz, hm);
			if (hm[i][i2] != 0)
			{
				for (int i3 = hm[i][i2] - 1; i3 >= 0; i3--)
				{
					if ((!(i2 > 0) || hm[i][i2 - 1] >= i3) &&
						(!(i2 < dimensionz - 1) || hm[i][i2 + 1] >= i3) &&
						(!(i > 0) || hm[i - 1][i2] >= i3) &&
						(!(i < dimensionx - 1) || hm[i + 1][i2] >= i3))
					{
						break;
					}
					create_surfaces(x->obj_manager, i, i2, i3, dimensionx, dimensionz, hm);
				}
			}
		}
	}
	prepare_render_br_object_manager(x->obj_manager);
	return x;
}

void use_world(world *w, GLuint program)
{
	use_br_texture_manager(w->tex_manager, program);
	use_br_object_manager(w->obj_manager);
}

void delete_world(world *w)
{
	delete_br_object_manager(w->obj_manager);
	delete_br_texture_manager(w->tex_manager);
}