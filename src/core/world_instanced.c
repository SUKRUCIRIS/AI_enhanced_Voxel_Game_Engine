#include "world_instanced.h"
#include "core.h"

GLfloat cube_vertices_ins[] = {
		-1, -1, -1, 0, 0, 0, 0, 1, // A 0
		-1, 1, -1, 0, 1, 0, 0, 1,	 // B 1
		1, 1, -1, 1, 1, 0, 0, 1,	 // C 2
		1, -1, -1, 1, 0, 0, 0, 1,	 // D 3
		-1, -1, 1, 0, 0, 0, 0, -1, // E 4
		1, -1, 1, 1, 0, 0, 0, -1,	 // F 5
		1, 1, 1, 1, 1, 0, 0, -1,	 // G 6
		-1, 1, 1, 0, 1, 0, 0, -1,	 // H 7

		-1, 1, -1, 0, 0, -1, 0, 0,	// D 8
		-1, -1, -1, 1, 0, -1, 0, 0, // A 9
		-1, -1, 1, 1, 1, -1, 0, 0,	// E 10
		-1, 1, 1, 0, 1, -1, 0, 0,		// H 11
		1, -1, -1, 0, 0, 1, 0, 0,		// B 12
		1, 1, -1, 1, 0, 1, 0, 0,		// C 13
		1, 1, 1, 1, 1, 1, 0, 0,			// G 14
		1, -1, 1, 0, 1, 1, 0, 0,		// F 15

		-1, -1, -1, 0, 0, 0, -1, 0, // A 16
		1, -1, -1, 1, 0, 0, -1, 0,	// B 17
		1, -1, 1, 1, 1, 0, -1, 0,		// F 18
		-1, -1, 1, 0, 1, 0, -1, 0,	// E 19
		1, 1, -1, 0, 0, 0, 1, 0,		// C 20
		-1, 1, -1, 1, 0, 0, 1, 0,		// D 21
		-1, 1, 1, 1, 1, 0, 1, 0,		// H 22
		1, 1, 1, 0, 1, 0, 1, 0,			// G 23
};

// index data
GLuint cube_indices_ins[] = {
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

void create_world_cube(world_instanced *x, int i, int i2, int i3, int dimensionx, int dimensionz, int **hm)
{
	ins_object *tmp = 0;
	if (hm[i][i2] == i3)
	{
		tmp = create_ins_object(x->obj_manager_top, 0, 0, 3, 10, 0.1f, 0.5f);
		translate_ins_object(tmp, (vec3){(float)(i - (int)(dimensionx / 2)), (float)i3, (float)(i2 - (int)(dimensionz / 2))}, 1);
		scale_ins_object(tmp, (vec3){0.5f, 0.5f, 0.5f}, 1);
	}
	if (i2 > 0 && hm[i][i2 - 1] < i3)
	{
		tmp = create_ins_object(x->obj_manager_front, 0, 0, 3, 10, 0.1f, 0.5f);
		translate_ins_object(tmp, (vec3){(float)(i - (int)(dimensionx / 2)), (float)i3, (float)(i2 - (int)(dimensionz / 2))}, 1);
		scale_ins_object(tmp, (vec3){0.5f, 0.5f, 0.5f}, 1);
	}
	if (i2 < dimensionz - 1 && hm[i][i2 + 1] < i3)
	{
		tmp = create_ins_object(x->obj_manager_back, 0, 0, 3, 10, 0.1f, 0.5f);
		translate_ins_object(tmp, (vec3){(float)(i - (int)(dimensionx / 2)), (float)i3, (float)(i2 - (int)(dimensionz / 2))}, 1);
		scale_ins_object(tmp, (vec3){0.5f, 0.5f, 0.5f}, 1);
	}
	if (i > 0 && hm[i - 1][i2] < i3)
	{
		tmp = create_ins_object(x->obj_manager_left, 0, 0, 3, 10, 0.1f, 0.5f);
		translate_ins_object(tmp, (vec3){(float)(i - (int)(dimensionx / 2)), (float)i3, (float)(i2 - (int)(dimensionz / 2))}, 1);
		scale_ins_object(tmp, (vec3){0.5f, 0.5f, 0.5f}, 1);
	}
	if (i < dimensionx - 1 && hm[i + 1][i2] < i3)
	{
		tmp = create_ins_object(x->obj_manager_right, 0, 0, 3, 10, 0.1f, 0.5f);
		translate_ins_object(tmp, (vec3){(float)(i - (int)(dimensionx / 2)), (float)i3, (float)(i2 - (int)(dimensionz / 2))}, 1);
		scale_ins_object(tmp, (vec3){0.5f, 0.5f, 0.5f}, 1);
	}
}

world_instanced *create_world_instanced(int **hm, int dimensionx, int dimensionz)
{
	world_instanced *x = malloc(sizeof(world_instanced));
	x->obj_manager_left = create_ins_object_manager(&(cube_vertices_ins[64]), 4, cube_indices_ins, 6);
	x->obj_manager_right = create_ins_object_manager(&(cube_vertices_ins[96]), 4, cube_indices_ins, 6);
	x->obj_manager_front = create_ins_object_manager(cube_vertices_ins, 4, cube_indices_ins, 6);
	x->obj_manager_back = create_ins_object_manager(&(cube_vertices_ins[32]), 4, cube_indices_ins, 6);
	x->obj_manager_top = create_ins_object_manager(&(cube_vertices_ins[160]), 4, cube_indices_ins, 6);
	x->tex_manager = create_br_texture_manager();

	// textures
	create_br_texture(x->tex_manager, "./textures/grass.jpg", GL_TEXTURE_2D, GL_NEAREST, GL_NEAREST, 0, GL_REPEAT, GL_REPEAT);
	// objects
	for (int i = 0; i < dimensionx; i++)
	{
		for (int i2 = 0; i2 < dimensionz; i2++)
		{
			create_world_cube(x, i, i2, hm[i][i2], dimensionx, dimensionz, hm);
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
					create_world_cube(x, i, i2, i3, dimensionx, dimensionz, hm);
				}
			}
		}
	}
	prepare_render_ins_object_manager(x->obj_manager_left);
	prepare_render_ins_object_manager(x->obj_manager_right);
	prepare_render_ins_object_manager(x->obj_manager_front);
	prepare_render_ins_object_manager(x->obj_manager_back);
	prepare_render_ins_object_manager(x->obj_manager_top);
	return x;
}

void use_world_instanced(world_instanced *w, GLuint program)
{
	use_br_texture_manager(w->tex_manager, program);
	use_ins_object_manager(w->obj_manager_left);
	use_ins_object_manager(w->obj_manager_right);
	use_ins_object_manager(w->obj_manager_front);
	use_ins_object_manager(w->obj_manager_back);
	use_ins_object_manager(w->obj_manager_top);
}

void delete_world_instanced(world_instanced *w)
{
	delete_ins_object_manager(w->obj_manager_left);
	delete_ins_object_manager(w->obj_manager_right);
	delete_ins_object_manager(w->obj_manager_front);
	delete_ins_object_manager(w->obj_manager_back);
	delete_ins_object_manager(w->obj_manager_top);
	delete_br_texture_manager(w->tex_manager);
	free(w);
}