#include "world_batch.h"
#include "core.h"

GLfloat cube_vertices[] = {
		-0.5f, -0.5f, -0.5f, 0, 0, 0, 0, -1, 0, // A 0
		-0.5f, 0.5f, -0.5f, 0, 1, 0, 0, -1, 0,	// B 1
		0.5f, 0.5f, -0.5f, 1, 1, 0, 0, -1, 0,		// C 2
		0.5f, -0.5f, -0.5f, 1, 0, 0, 0, -1, 0,	// D 3
		-0.5f, -0.5f, 0.5f, 0, 0, 0, 0, 1, 0,		// E 4
		0.5f, -0.5f, 0.5f, 1, 0, 0, 0, 1, 0,		// F 5
		0.5f, 0.5f, 0.5f, 1, 1, 0, 0, 1, 0,			// G 6
		-0.5f, 0.5f, 0.5f, 0, 1, 0, 0, 1, 0,		// H 7

		-0.5f, 0.5f, -0.5f, 0, 1, -1, 0, 0, 0,	// D 8
		-0.5f, -0.5f, -0.5f, 0, 0, -1, 0, 0, 0, // A 9
		-0.5f, -0.5f, 0.5f, 1, 0, -1, 0, 0, 0,	// E 10
		-0.5f, 0.5f, 0.5f, 1, 1, -1, 0, 0, 0,		// H 11
		0.5f, -0.5f, -0.5f, 0, 0, 1, 0, 0, 0,		// B 12
		0.5f, 0.5f, -0.5f, 0, 1, 1, 0, 0, 0,		// C 13
		0.5f, 0.5f, 0.5f, 1, 1, 1, 0, 0, 0,			// G 14
		0.5f, -0.5f, 0.5f, 1, 0, 1, 0, 0, 0,		// F 15

		-0.5f, -0.5f, -0.5f, 0, 0, 0, -1, 0, 0, // A 16
		0.5f, -0.5f, -0.5f, 1, 0, 0, -1, 0, 0,	// B 17
		0.5f, -0.5f, 0.5f, 1, 1, 0, -1, 0, 0,		// F 18
		-0.5f, -0.5f, 0.5f, 0, 1, 0, -1, 0, 0,	// E 19
		0.5f, 0.5f, -0.5f, 0, 0, 0, 1, 0, 0,		// C 20
		-0.5f, 0.5f, -0.5f, 1, 0, 0, 1, 0, 0,		// D 21
		-0.5f, 0.5f, 0.5f, 1, 1, 0, 1, 0, 0,		// H 22
		0.5f, 0.5f, 0.5f, 0, 1, 0, 1, 0, 0,			// G 23
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

int grass_border = 30;
int snow_border = 80;

br_texture_manager *tex_manager = 0;

br_object *create_top_surface(br_object_manager *x, GLfloat texture_i)
{
	return create_br_object(x, &(cube_vertices[180]), 4, cube_indices, 6, texture_i, 0, 3, 10, 0.1f, 0.5f);
}

br_object *create_bottom_surface(br_object_manager *x, GLfloat texture_i)
{
	return create_br_object(x, &(cube_vertices[144]), 4, cube_indices, 6, texture_i, 0, 3, 10, 0.1f, 0.5f);
}

br_object *create_right_surface(br_object_manager *x, GLfloat texture_i)
{
	return create_br_object(x, &(cube_vertices[108]), 4, cube_indices, 6, texture_i, 0, 3, 10, 0.1f, 0.5f);
}

br_object *create_left_surface(br_object_manager *x, GLfloat texture_i)
{
	return create_br_object(x, &(cube_vertices[72]), 4, cube_indices, 6, texture_i, 0, 3, 10, 0.1f, 0.5f);
}

br_object *create_back_surface(br_object_manager *x, GLfloat texture_i)
{
	return create_br_object(x, &(cube_vertices[36]), 4, cube_indices, 6, texture_i, 0, 3, 10, 0.1f, 0.5f);
}

br_object *create_front_surface(br_object_manager *x, GLfloat texture_i)
{
	return create_br_object(x, cube_vertices, 4, cube_indices, 6, texture_i, 0, 3, 10, 0.1f, 0.5f);
}

void create_surfaces(br_object_manager *x, int i, int i2, int i3, int dimensionx,
										 int dimensionz, int **hm, vec3 lightdir)
{
	br_object *tmp = 0;
	vec3 translate;
	translate[0] = (float)(i - (int)(dimensionx / 2));
	translate[1] = (float)i3;
	translate[2] = (float)(i2 - (int)(dimensionz / 2));
	unsigned char top = 0, front = 0, back = 0, left = 0, right = 0;
	float texture_i = 0;
	if (hm[i][i2] == i3)
	{
		if (i3 < grass_border)
		{
			texture_i = 2;
		}
		else if (i3 < snow_border)
		{
			texture_i = 4;
		}
		else
		{
			texture_i = 6;
		}
		tmp = create_top_surface(x, texture_i);
		translate_br_object(tmp, translate, 1);
		top = 1;
	}
	if (i3 != hm[i][i2])
	{
		texture_i = 0;
	}
	else if (i3 < grass_border)
	{
		texture_i = 1;
	}
	else if (i3 < snow_border)
	{
		texture_i = 3;
	}
	else
	{
		texture_i = 5;
	}
	if (i2 > 0 && hm[i][i2 - 1] < i3)
	{
		tmp = create_front_surface(x, texture_i);
		translate_br_object(tmp, translate, 1);
		front = 1;
	}
	if (i2 < dimensionz - 1 && hm[i][i2 + 1] < i3)
	{
		tmp = create_back_surface(x, texture_i);
		translate_br_object(tmp, translate, 1);
		back = 1;
	}
	if (i > 0 && hm[i - 1][i2] < i3)
	{
		tmp = create_left_surface(x, texture_i);
		translate_br_object(tmp, translate, 1);
		left = 1;
	}
	if (i < dimensionx - 1 && hm[i + 1][i2] < i3)
	{
		tmp = create_right_surface(x, texture_i);
		translate_br_object(tmp, translate, 1);
		right = 1;
	}
	texture_i = 0;
	if (lightdir != 0)
	{
		if ((i2 > 0 && hm[i][i2 - 1] == i3 - 1) ||
				(i2 < dimensionz - 1 && hm[i][i2 + 1] == i3 - 1) ||
				(i > 0 && hm[i - 1][i2] == i3 - 1) ||
				(i < dimensionx - 1 && hm[i + 1][i2] == i3 - 1))
		{
			tmp = create_bottom_surface(x, texture_i);
			translate_br_object(tmp, translate, 1);
		}
		if (lightdir[0] > 0 && front == 0)
		{
			tmp = create_front_surface(x, texture_i);
			translate_br_object(tmp, translate, 1);
		}
		else if (lightdir[0] < 0 && back == 0)
		{
			tmp = create_back_surface(x, texture_i);
			translate_br_object(tmp, translate, 1);
		}
		if (lightdir[2] > 0 && left == 0)
		{
			tmp = create_left_surface(x, texture_i);
			translate_br_object(tmp, translate, 1);
		}
		else if (lightdir[2] < 0 && right == 0)
		{
			tmp = create_right_surface(x, texture_i);
			translate_br_object(tmp, translate, 1);
		}
	}
}

world_batch *create_world_batch(int **hm, int startx, int startz, int widthx, int widthz,
																int dimensionx, int dimensionz, vec3 lightdir, float sealevel,
																unsigned char create_water_physic)
{
	if (startx >= dimensionx || startz >= dimensionz)
	{
		return 0;
	}
	if (startx + widthx > dimensionx)
	{
		widthx = dimensionx - startx;
	}
	if (startz + widthz > dimensionz)
	{
		widthz = dimensionz - startz;
	}

	world_batch *x = malloc(sizeof(world_batch));
	x->obj_manager = create_br_object_manager();

	// textures
	if (tex_manager == 0)
	{
		tex_manager = create_br_texture_manager();
		create_br_texture(tex_manager, "./textures/side.png", GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST, 0, GL_REPEAT, GL_REPEAT);
		create_br_texture(tex_manager, "./textures/dirt_side.png", GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST, 1, GL_REPEAT, GL_CLAMP_TO_EDGE);
		create_br_texture(tex_manager, "./textures/dirt.png", GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST, 2, GL_REPEAT, GL_REPEAT);
		create_br_texture(tex_manager, "./textures/grass_side.png", GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST, 3, GL_REPEAT, GL_CLAMP_TO_EDGE);
		create_br_texture(tex_manager, "./textures/grass.png", GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST, 4, GL_REPEAT, GL_REPEAT);
		create_br_texture(tex_manager, "./textures/snow_side.png", GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST, 5, GL_REPEAT, GL_CLAMP_TO_EDGE);
		create_br_texture(tex_manager, "./textures/snow.png", GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST, 6, GL_REPEAT, GL_REPEAT);
	}

	if (sealevel > 0)
	{
		x->w = create_water(sealevel, hm, "./textures/water.png", startx - (int)(dimensionx / 2), startz - (int)(dimensionz / 2),
												widthx, widthz, dimensionx, dimensionz, create_water_physic);
	}
	else
	{
		x->w = 0;
	}

	// objects
	for (int i = startx; i < startx + widthx; i++)
	{
		for (int i2 = startz; i2 < startz + widthz; i2++)
		{
			create_surfaces(x->obj_manager, i, i2, hm[i][i2], dimensionx, dimensionz, hm, lightdir);
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
					create_surfaces(x->obj_manager, i, i2, i3, dimensionx, dimensionz, hm, lightdir);
				}
			}
		}
	}
	prepare_render_br_object_manager(x->obj_manager);
	return x;
}

void merge_top(br_object_manager *x, int **hm, int startx, int startz, int widthx, int widthz,
							 int dimensionx, int dimensionz, unsigned char **done)
{
	float texture_i = 0;
	for (int i = startx; i < startx + widthx; i++)
	{
		for (int i2 = startz; i2 < startz + widthz; i2++)
		{
			if (done[i - startx][i2 - startz] == 0)
			{
				if (hm[i][i2] < grass_border)
				{
					texture_i = 2;
				}
				else if (hm[i][i2] < snow_border)
				{
					texture_i = 4;
				}
				else
				{
					texture_i = 6;
				}
				int loop = 0;
				unsigned char type = 0;
				if (i < dimensionx - 1 && i < startx + widthx - 1 && hm[i + 1][i2] == hm[i][i2] && done[i + 1 - startx][i2 - startz] == 0)
				{
					type = 1;
				}
				else if (i2 < dimensionz - 1 && i2 < startz + widthz - 1 && hm[i][i2 + 1] == hm[i][i2] && done[i - startx][i2 + 1 - startz] == 0)
				{
					type = 0;
				}
				else
				{
					type = 2;
				}
				if (type == 2)
				{
					vec3 translate = {(float)(i - (int)(dimensionx / 2)), (float)hm[i][i2], (float)(i2 - (int)(dimensionz / 2))};
					br_object *tmp = create_br_object(x, &(cube_vertices[180]), 4, cube_indices, 6, texture_i, 0, 3, 10, 0.1f, 0.5f);
					translate_br_object(tmp, translate, 0);
					done[i - startx][i2 - startz] = 1;
				}
				else if (type == 0)
				{
					while (i2 + loop < dimensionz && i2 + loop < startz + widthz && hm[i][i2 + loop] == hm[i][i2] && done[i - startx][i2 + loop - startz] == 0)
					{
						done[i - startx][i2 + loop - startz] = 1;
						loop++;
					}
					vec3 translate = {(float)(i - (int)(dimensionx / 2)), (float)hm[i][i2], (float)(i2 + (loop - 1) / 2.0f - (int)(dimensionz / 2))};
					vec3 scale = {1, 1, (float)loop};
					cube_vertices[180 + 22] = (float)loop;
					cube_vertices[180 + 31] = (float)loop;
					br_object *tmp = create_br_object(x, &(cube_vertices[180]), 4, cube_indices, 6, texture_i, 0, 3, 10, 0.1f, 0.5f);
					scale_br_object(tmp, scale, 0);
					translate_br_object(tmp, translate, 0);
					cube_vertices[180 + 22] = 1;
					cube_vertices[180 + 31] = 1;
				}
				else if (type == 1)
				{
					while (i + loop < dimensionx && i + loop < startx + widthx && hm[i + loop][i2] == hm[i][i2] && done[i + loop - startx][i2 - startz] == 0)
					{
						done[i + loop - startx][i2 - startz] = 1;
						loop++;
					}
					vec3 translate = {(float)(i + (loop - 1) / 2.0f - (int)(dimensionx / 2)), (float)hm[i][i2], (float)(i2 - (int)(dimensionz / 2))};
					vec3 scale = {(float)loop, 1, 1};
					cube_vertices[180 + 12] = (float)loop;
					cube_vertices[180 + 21] = (float)loop;
					br_object *tmp = create_br_object(x, &(cube_vertices[180]), 4, cube_indices, 6, texture_i, 0, 3, 10, 0.1f, 0.5f);
					scale_br_object(tmp, scale, 0);
					translate_br_object(tmp, translate, 0);
					cube_vertices[180 + 12] = 1;
					cube_vertices[180 + 21] = 1;
				}
			}
		}
	}
}

void merge_front(br_object_manager *x, int **hm, int startx, int startz, int widthx, int widthz,
								 int dimensionx, int dimensionz, unsigned char **done)
{
	float texture_i = 0;
	for (int i = startx; i < startx + widthx; i++)
	{
		for (int i2 = startz; i2 < startz + widthz; i2++)
		{
			if (done[i - startx][i2 - startz] == 0)
			{
				if (hm[i][i2] < grass_border)
				{
					texture_i = 1;
				}
				else if (hm[i][i2] < snow_border)
				{
					texture_i = 3;
				}
				else
				{
					texture_i = 5;
				}
				int loop = 0;
				unsigned char type = 0;
				if (i < dimensionx - 1 && i < startx + widthx - 1 && hm[i + 1][i2] == hm[i][i2] && done[i + 1 - startx][i2 - startz] == 0 &&
						i2 > 0 && hm[i][i2 - 1] < hm[i][i2] && i2 > 0 && hm[i + 1][i2 - 1] < hm[i + 1][i2])
				{
					type = 1;
				}
				else if (i2 < dimensionz - 1 && i2 < startz + widthz - 1 && hm[i][i2 + 1] == hm[i][i2] && done[i - startx][i2 + 1 - startz] == 0 &&
								 i2 > 0 && hm[i][i2 - 1] < hm[i][i2] && i2 > 0 && hm[i][i2] < hm[i][i2 + 1])
				{
					type = 0;
				}
				else if (i2 > 0 && hm[i][i2 - 1] < hm[i][i2])
				{
					type = 2;
				}
				else
				{
					done[i - startx][i2 - startz] = 1;
					continue;
				}
				if (type == 2)
				{
					vec3 translate = {(float)(i - (int)(dimensionx / 2)), (float)hm[i][i2], (float)(i2 - (int)(dimensionz / 2))};
					br_object *tmp = create_br_object(x, cube_vertices, 4, cube_indices, 6, texture_i, 0, 3, 10, 0.1f, 0.5f);
					translate_br_object(tmp, translate, 0);
					done[i - startx][i2 - startz] = 1;
				}
				else if (type == 0)
				{
					while (i2 + loop < dimensionz && i2 + loop < startz + widthz && hm[i][i2 + loop] == hm[i][i2] && done[i - startx][i2 + loop - startz] == 0)
					{
						if (i2 > 0 && hm[i][i2 - 1 + loop] < hm[i][i2 + loop])
						{
							done[i - startx][i2 + loop - startz] = 1;
							loop++;
						}
						else
						{
							break;
						}
					}
					vec3 translate = {(float)(i - (int)(dimensionx / 2)), (float)hm[i][i2], (float)(i2 + (loop - 1) / 2.0f - (int)(dimensionz / 2))};
					vec3 scale = {1, 1, (float)loop};
					cube_vertices[0 + 13] = (float)loop;
					cube_vertices[0 + 22] = (float)loop;
					br_object *tmp = create_br_object(x, cube_vertices, 4, cube_indices, 6, texture_i, 0, 3, 10, 0.1f, 0.5f);
					cube_vertices[0 + 13] = 1;
					cube_vertices[0 + 22] = 1;
					scale_br_object(tmp, scale, 0);
					translate_br_object(tmp, translate, 0);
				}
				else if (type == 1)
				{
					while (i + loop < dimensionx && i + loop < startx + widthx && hm[i + loop][i2] == hm[i][i2] && done[i + loop - startx][i2 - startz] == 0)
					{
						if (i2 > 0 && hm[i + loop][i2 - 1] < hm[i + loop][i2])
						{
							done[i + loop - startx][i2 - startz] = 1;
							loop++;
						}
						else
						{
							break;
						}
					}
					vec3 translate = {(float)(i + (loop - 1) / 2.0f - (int)(dimensionx / 2)), (float)hm[i][i2], (float)(i2 - (int)(dimensionz / 2))};
					vec3 scale = {(float)loop, 1, 1};
					cube_vertices[0 + 21] = (float)loop;
					cube_vertices[0 + 30] = (float)loop;
					br_object *tmp = create_br_object(x, cube_vertices, 4, cube_indices, 6, texture_i, 0, 3, 10, 0.1f, 0.5f);
					cube_vertices[0 + 21] = 1;
					cube_vertices[0 + 30] = 1;
					scale_br_object(tmp, scale, 0);
					translate_br_object(tmp, translate, 0);
				}
			}
		}
	}
}

void merge_back(br_object_manager *x, int **hm, int startx, int startz, int widthx, int widthz,
								int dimensionx, int dimensionz, unsigned char **done)
{
	float texture_i = 0;
	for (int i = startx; i < startx + widthx; i++)
	{
		for (int i2 = startz; i2 < startz + widthz; i2++)
		{
			if (done[i - startx][i2 - startz] == 0)
			{
				if (hm[i][i2] < grass_border)
				{
					texture_i = 1;
				}
				else if (hm[i][i2] < snow_border)
				{
					texture_i = 3;
				}
				else
				{
					texture_i = 5;
				}
				int loop = 0;
				unsigned char type = 0;
				if (i < dimensionx - 1 && i < startx + widthx - 1 && hm[i + 1][i2] == hm[i][i2] && done[i + 1 - startx][i2 - startz] == 0 &&
						i2 < dimensionz - 1 && hm[i][i2 + 1] < hm[i][i2] && i2 < dimensionz - 1 && hm[i + 1][i2 + 1] < hm[i + 1][i2])
				{
					type = 1;
				}
				else if (i2 < dimensionz - 1 && i2 < startz + widthz - 1 && hm[i][i2 + 1] == hm[i][i2] && done[i - startx][i2 + 1 - startz] == 0 &&
								 i2 < dimensionz - 1 && hm[i][i2 + 1] < hm[i][i2] && i2 + 1 < dimensionz - 1 && hm[i][i2 + 2] < hm[i][i2 + 1])
				{
					type = 0;
				}
				else if (i2 < dimensionz - 1 && hm[i][i2 + 1] < hm[i][i2])
				{
					type = 2;
				}
				else
				{
					done[i - startx][i2 - startz] = 1;
					continue;
				}
				if (type == 2)
				{
					vec3 translate = {(float)(i - (int)(dimensionx / 2)), (float)hm[i][i2], (float)(i2 - (int)(dimensionz / 2))};
					br_object *tmp = create_br_object(x, &(cube_vertices[36]), 4, cube_indices, 6, texture_i, 0, 3, 10, 0.1f, 0.5f);
					translate_br_object(tmp, translate, 0);
					done[i - startx][i2 - startz] = 1;
				}
				else if (type == 0)
				{
					while (i2 + loop < dimensionz && i2 + loop < startz + widthz && hm[i][i2 + loop] == hm[i][i2] && done[i - startx][i2 + loop - startz] == 0)
					{
						if (i2 + loop < dimensionz - 1 && hm[i][i2 + 1 + loop] < hm[i][i2 + loop])
						{
							done[i - startx][i2 + loop - startz] = 1;
							loop++;
						}
						else
						{
							break;
						}
					}
					vec3 translate = {(float)(i - (int)(dimensionx / 2)), (float)hm[i][i2], (float)(i2 + (loop - 1) / 2.0f - (int)(dimensionz / 2))};
					vec3 scale = {1, 1, (float)loop};
					cube_vertices[36 + 22] = (float)loop;
					cube_vertices[36 + 31] = (float)loop;
					br_object *tmp = create_br_object(x, &(cube_vertices[36]), 4, cube_indices, 6, texture_i, 0, 3, 10, 0.1f, 0.5f);
					cube_vertices[36 + 22] = 1;
					cube_vertices[36 + 31] = 1;
					scale_br_object(tmp, scale, 0);
					translate_br_object(tmp, translate, 0);
				}
				else if (type == 1)
				{
					while (i + loop < dimensionx && i + loop < startx + widthx && hm[i + loop][i2] == hm[i][i2] && done[i + loop - startx][i2 - startz] == 0)
					{
						if (i2 < dimensionz - 1 && hm[i + loop][i2 + 1] < hm[i + loop][i2])
						{
							done[i + loop - startx][i2 - startz] = 1;
							loop++;
						}
						else
						{
							break;
						}
					}
					vec3 translate = {(float)(i + (loop - 1) / 2.0f - (int)(dimensionx / 2)), (float)hm[i][i2], (float)(i2 - (int)(dimensionz / 2))};
					vec3 scale = {(float)loop, 1, 1};
					cube_vertices[36 + 12] = (float)loop;
					cube_vertices[36 + 21] = (float)loop;
					br_object *tmp = create_br_object(x, &(cube_vertices[36]), 4, cube_indices, 6, texture_i, 0, 3, 10, 0.1f, 0.5f);
					cube_vertices[36 + 12] = 1;
					cube_vertices[36 + 21] = 1;
					scale_br_object(tmp, scale, 0);
					translate_br_object(tmp, translate, 0);
				}
			}
		}
	}
}

void merge_left(br_object_manager *x, int **hm, int startx, int startz, int widthx, int widthz,
								int dimensionx, int dimensionz, unsigned char **done)
{
	float texture_i = 0;
	for (int i = startx; i < startx + widthx; i++)
	{
		for (int i2 = startz; i2 < startz + widthz; i2++)
		{
			if (done[i - startx][i2 - startz] == 0)
			{
				if (hm[i][i2] < grass_border)
				{
					texture_i = 1;
				}
				else if (hm[i][i2] < snow_border)
				{
					texture_i = 3;
				}
				else
				{
					texture_i = 5;
				}
				int loop = 0;
				unsigned char type = 0;
				if (i < dimensionx - 1 && i < startx + widthx - 1 && hm[i + 1][i2] == hm[i][i2] && done[i + 1 - startx][i2 - startz] == 0 &&
						i > 0 && hm[i - 1][i2] < hm[i][i2] && i > 0 && hm[i][i2] < hm[i + 1][i2])
				{
					type = 1;
				}
				else if (i2 < dimensionz - 1 && i2 < startz + widthz - 1 && hm[i][i2 + 1] == hm[i][i2] && done[i - startx][i2 + 1 - startz] == 0 &&
								 i > 0 && hm[i - 1][i2] < hm[i][i2] && i > 0 && hm[i - 1][i2 + 1] < hm[i][i2 + 1])
				{
					type = 0;
				}
				else if (i > 0 && hm[i - 1][i2] < hm[i][i2])
				{
					type = 2;
				}
				else
				{
					done[i - startx][i2 - startz] = 1;
					continue;
				}
				if (type == 2)
				{
					vec3 translate = {(float)(i - (int)(dimensionx / 2)), (float)hm[i][i2], (float)(i2 - (int)(dimensionz / 2))};
					br_object *tmp = create_br_object(x, &(cube_vertices[72]), 4, cube_indices, 6, texture_i, 0, 3, 10, 0.1f, 0.5f);
					translate_br_object(tmp, translate, 0);
					done[i - startx][i2 - startz] = 1;
				}
				else if (type == 0)
				{
					while (i2 + loop < dimensionz && i2 + loop < startz + widthz && hm[i][i2 + loop] == hm[i][i2] && done[i - startx][i2 + loop - startz] == 0)
					{
						if (i > 0 && hm[i - 1][i2 + loop] < hm[i][i2 + loop])
						{
							done[i - startx][i2 + loop - startz] = 1;
							loop++;
						}
						else
						{
							break;
						}
					}
					vec3 translate = {(float)(i - (int)(dimensionx / 2)), (float)hm[i][i2], (float)(i2 + (loop - 1) / 2.0f - (int)(dimensionz / 2))};
					vec3 scale = {1, 1, (float)loop};
					cube_vertices[72 + 21] = (float)loop;
					cube_vertices[72 + 30] = (float)loop;
					br_object *tmp = create_br_object(x, &(cube_vertices[72]), 4, cube_indices, 6, texture_i, 0, 3, 10, 0.1f, 0.5f);
					cube_vertices[72 + 21] = 1;
					cube_vertices[72 + 30] = 1;
					scale_br_object(tmp, scale, 0);
					translate_br_object(tmp, translate, 0);
				}
				else if (type == 1)
				{
					while (i + loop < dimensionx && i + loop < startx + widthx && hm[i + loop][i2] == hm[i][i2] && done[i + loop - startx][i2 - startz] == 0)
					{
						if (i > 0 && hm[i + loop - 1][i2] < hm[i + loop][i2])
						{
							done[i + loop - startx][i2 - startz] = 1;
							loop++;
						}
						else
						{
							break;
						}
					}
					vec3 translate = {(float)(i + (loop - 1) / 2.0f - (int)(dimensionx / 2)), (float)hm[i][i2], (float)(i2 - (int)(dimensionz / 2))};
					vec3 scale = {(float)loop, 1, 1};
					cube_vertices[72 + 4] = (float)loop;
					cube_vertices[72 + 31] = (float)loop;
					br_object *tmp = create_br_object(x, &(cube_vertices[72]), 4, cube_indices, 6, texture_i, 0, 3, 10, 0.1f, 0.5f);
					cube_vertices[72 + 4] = 1;
					cube_vertices[72 + 31] = 1;
					scale_br_object(tmp, scale, 0);
					translate_br_object(tmp, translate, 0);
				}
			}
		}
	}
}

void merge_right(br_object_manager *x, int **hm, int startx, int startz, int widthx, int widthz,
								 int dimensionx, int dimensionz, unsigned char **done)
{
	float texture_i = 0;
	for (int i = startx; i < startx + widthx; i++)
	{
		for (int i2 = startz; i2 < startz + widthz; i2++)
		{
			if (done[i - startx][i2 - startz] == 0)
			{
				if (hm[i][i2] < grass_border)
				{
					texture_i = 1;
				}
				else if (hm[i][i2] < snow_border)
				{
					texture_i = 3;
				}
				else
				{
					texture_i = 5;
				}
				int loop = 0;
				unsigned char type = 0;
				if (i < dimensionx - 1 && i < startx + widthx - 1 && hm[i + 1][i2] == hm[i][i2] && done[i + 1 - startx][i2 - startz] == 0 &&
						i < dimensionx - 1 && hm[i + 1][i2] < hm[i][i2] && i < dimensionx - 1 && hm[i + 2][i2] < hm[i + 1][i2])
				{
					type = 1;
				}
				else if (i2 < dimensionz - 1 && i2 < startz + widthz - 1 && hm[i][i2 + 1] == hm[i][i2] && done[i - startx][i2 + 1 - startz] == 0 &&
								 i < dimensionx - 1 && hm[i + 1][i2] < hm[i][i2] && i < dimensionx - 1 && hm[i + 1][i2 + 1] < hm[i][i2 + 1])
				{
					type = 0;
				}
				else if (i < dimensionx - 1 && hm[i + 1][i2] < hm[i][i2])
				{
					type = 2;
				}
				else
				{
					done[i - startx][i2 - startz] = 1;
					continue;
				}
				if (type == 2)
				{
					vec3 translate = {(float)(i - (int)(dimensionx / 2)), (float)hm[i][i2], (float)(i2 - (int)(dimensionz / 2))};
					br_object *tmp = create_br_object(x, &(cube_vertices[108]), 4, cube_indices, 6, texture_i, 0, 3, 10, 0.1f, 0.5f);
					translate_br_object(tmp, translate, 0);
					done[i - startx][i2 - startz] = 1;
				}
				else if (type == 0)
				{
					while (i2 + loop < dimensionz && i2 + loop < startz + widthz && hm[i][i2 + loop] == hm[i][i2] && done[i - startx][i2 + loop - startz] == 0)
					{
						if (i < dimensionx - 1 && hm[i + 1][i2 + loop] < hm[i][i2 + loop])
						{
							done[i - startx][i2 + loop - startz] = 1;
							loop++;
						}
						else
						{
							break;
						}
					}
					vec3 translate = {(float)(i - (int)(dimensionx / 2)), (float)hm[i][i2], (float)(i2 + (loop - 1) / 2.0f - (int)(dimensionz / 2))};
					vec3 scale = {1, 1, (float)loop};
					cube_vertices[108 + 21] = (float)loop;
					cube_vertices[108 + 30] = (float)loop;
					br_object *tmp = create_br_object(x, &(cube_vertices[108]), 4, cube_indices, 6, texture_i, 0, 3, 10, 0.1f, 0.5f);
					cube_vertices[108 + 21] = 1;
					cube_vertices[108 + 30] = 1;
					scale_br_object(tmp, scale, 0);
					translate_br_object(tmp, translate, 0);
				}
				else if (type == 1)
				{
					while (i + loop < dimensionx && i + loop < startx + widthx && hm[i + loop][i2] == hm[i][i2] && done[i + loop - startx][i2 - startz] == 0)
					{
						if (i < dimensionx - 1 && hm[i + 1 + loop][i2] < hm[i + loop][i2])
						{
							done[i + loop - startx][i2 - startz] = 1;
							loop++;
						}
						else
						{
							break;
						}
					}
					vec3 translate = {(float)(i + (loop - 1) / 2.0f - (int)(dimensionx / 2)), (float)hm[i][i2], (float)(i2 - (int)(dimensionz / 2))};
					vec3 scale = {(float)loop, 1, 1};
					cube_vertices[108 + 13] = (float)loop;
					cube_vertices[108 + 22] = (float)loop;
					br_object *tmp = create_br_object(x, &(cube_vertices[108]), 4, cube_indices, 6, texture_i, 0, 3, 10, 0.1f, 0.5f);
					cube_vertices[108 + 13] = 1;
					cube_vertices[108 + 22] = 1;
					scale_br_object(tmp, scale, 0);
					translate_br_object(tmp, translate, 0);
				}
			}
		}
	}
}

void merge_vertical(br_object_manager *x, int **hm, int startx, int startz, int widthx, int widthz,
										int dimensionx, int dimensionz)
{
	for (int i = startx; i < startx + widthx; i++)
	{
		for (int i2 = startz; i2 < startz + widthz; i2++)
		{
			if (hm[i][i2] != 0)
			{
				int front = 0;
				int back = 0;
				int left = 0;
				int right = 0;
				for (int i3 = hm[i][i2] - 1; i3 >= 0; i3--)
				{
					if ((!(i2 > 0) || hm[i][i2 - 1] >= i3) &&
							(!(i2 < dimensionz - 1) || hm[i][i2 + 1] >= i3) &&
							(!(i > 0) || hm[i - 1][i2] >= i3) &&
							(!(i < dimensionx - 1) || hm[i + 1][i2] >= i3))
					{
						break;
					}
					if (i2 > 0 && hm[i][i2 - 1] < i3)
					{
						front++;
					}
					if (i2 < dimensionz - 1 && hm[i][i2 + 1] < i3)
					{
						back++;
					}
					if (i > 0 && hm[i - 1][i2] < i3)
					{
						left++;
					}
					if (i < dimensionx - 1 && hm[i + 1][i2] < i3)
					{
						right++;
					}
				}
				{
					if (front > 0)
					{
						vec3 translate = {(float)(i - (int)(dimensionx / 2)), (float)hm[i][i2] - 1 - (front - 1) / 2.0f, (float)(i2 - (int)(dimensionz / 2))};
						vec3 scale = {1, (float)front, 1};
						cube_vertices[0 + 13] = (float)front;
						cube_vertices[0 + 22] = (float)front;
						br_object *tmp = create_br_object(x, cube_vertices, 4, cube_indices, 6, 0, 0, 3, 10, 0.1f, 0.5f);
						cube_vertices[0 + 13] = 1;
						cube_vertices[0 + 22] = 1;
						scale_br_object(tmp, scale, 0);
						translate_br_object(tmp, translate, 0);
					}
					if (back > 0)
					{
						vec3 translate = {(float)(i - (int)(dimensionx / 2)), (float)hm[i][i2] - 1 - (back - 1) / 2.0f, (float)(i2 - (int)(dimensionz / 2))};
						vec3 scale = {1, (float)back, 1};
						cube_vertices[36 + 22] = (float)back;
						cube_vertices[36 + 31] = (float)back;
						br_object *tmp = create_br_object(x, &(cube_vertices[36]), 4, cube_indices, 6, 0, 0, 3, 10, 0.1f, 0.5f);
						cube_vertices[36 + 22] = 1;
						cube_vertices[36 + 31] = 1;
						scale_br_object(tmp, scale, 0);
						translate_br_object(tmp, translate, 0);
					}
					if (left > 0)
					{
						vec3 translate = {(float)(i - (int)(dimensionx / 2)), (float)hm[i][i2] - 1 - (left - 1) / 2.0f, (float)(i2 - (int)(dimensionz / 2))};
						vec3 scale = {1, (float)left, 1};
						cube_vertices[72 + 4] = (float)left;
						cube_vertices[72 + 31] = (float)left;
						br_object *tmp = create_br_object(x, &(cube_vertices[72]), 4, cube_indices, 6, 0, 0, 3, 10, 0.1f, 0.5f);
						cube_vertices[72 + 4] = 1;
						cube_vertices[72 + 31] = 1;
						scale_br_object(tmp, scale, 0);
						translate_br_object(tmp, translate, 0);
					}
					if (right > 0)
					{
						vec3 translate = {(float)(i - (int)(dimensionx / 2)), (float)hm[i][i2] - 1 - (right - 1) / 2.0f, (float)(i2 - (int)(dimensionz / 2))};
						vec3 scale = {1, (float)right, 1};
						cube_vertices[108 + 13] = (float)right;
						cube_vertices[108 + 22] = (float)right;
						br_object *tmp = create_br_object(x, &(cube_vertices[108]), 4, cube_indices, 6, 0, 0, 3, 10, 0.1f, 0.5f);
						cube_vertices[108 + 13] = 1;
						cube_vertices[108 + 22] = 1;
						scale_br_object(tmp, scale, 0);
						translate_br_object(tmp, translate, 0);
					}
				}
			}
		}
	}
}

world_batch *create_world_batch_facemerged(int **hm, int startx, int startz, int widthx, int widthz,
																					 int dimensionx, int dimensionz, float sealevel,
																					 unsigned char create_water_physic, unsigned char **done)
{
	if (startx >= dimensionx || startz >= dimensionz)
	{
		return 0;
	}
	if (startx + widthx > dimensionx)
	{
		widthx = dimensionx - startx;
	}
	if (startz + widthz > dimensionz)
	{
		widthz = dimensionz - startz;
	}

	world_batch *x = malloc(sizeof(world_batch));
	x->obj_manager = create_br_object_manager();

	// textures
	if (tex_manager == 0)
	{
		tex_manager = create_br_texture_manager();
		create_br_texture(tex_manager, "./textures/side.png", GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST, 0, GL_REPEAT, GL_REPEAT);
		create_br_texture(tex_manager, "./textures/dirt_side.png", GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST, 1, GL_REPEAT, GL_CLAMP_TO_EDGE);
		create_br_texture(tex_manager, "./textures/dirt.png", GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST, 2, GL_REPEAT, GL_REPEAT);
		create_br_texture(tex_manager, "./textures/grass_side.png", GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST, 3, GL_REPEAT, GL_CLAMP_TO_EDGE);
		create_br_texture(tex_manager, "./textures/grass.png", GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST, 4, GL_REPEAT, GL_REPEAT);
		create_br_texture(tex_manager, "./textures/snow_side.png", GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST, 5, GL_REPEAT, GL_CLAMP_TO_EDGE);
		create_br_texture(tex_manager, "./textures/snow.png", GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST, 6, GL_REPEAT, GL_REPEAT);
	}

	if (sealevel > 0)
	{
		x->w = create_water(sealevel, hm, "./textures/water.png", startx - (int)(dimensionx / 2), startz - (int)(dimensionz / 2),
												widthx, widthz, dimensionx, dimensionz, create_water_physic);
	}
	else
	{
		x->w = 0;
	}

	for (int i = 0; i < widthx; i++)
	{
		memset(done[i], 0, sizeof(unsigned char) * widthz);
	}
	merge_top(x->obj_manager, hm, startx, startz, widthx, widthz, dimensionx, dimensionz, done);
	for (int i = 0; i < widthx; i++)
	{
		memset(done[i], 0, sizeof(unsigned char) * widthz);
	}
	merge_front(x->obj_manager, hm, startx, startz, widthx, widthz, dimensionx, dimensionz, done);
	for (int i = 0; i < widthx; i++)
	{
		memset(done[i], 0, sizeof(unsigned char) * widthz);
	}
	merge_back(x->obj_manager, hm, startx, startz, widthx, widthz, dimensionx, dimensionz, done);
	for (int i = 0; i < widthx; i++)
	{
		memset(done[i], 0, sizeof(unsigned char) * widthz);
	}
	merge_left(x->obj_manager, hm, startx, startz, widthx, widthz, dimensionx, dimensionz, done);
	for (int i = 0; i < widthx; i++)
	{
		memset(done[i], 0, sizeof(unsigned char) * widthz);
	}
	merge_right(x->obj_manager, hm, startx, startz, widthx, widthz, dimensionx, dimensionz, done);
	merge_vertical(x->obj_manager, hm, startx, startz, widthx, widthz, dimensionx, dimensionz);

	prepare_render_br_object_manager(x->obj_manager);
	return x;
}

void use_world_batch_land(world_batch *w, GLuint land_program)
{
	use_br_texture_manager(tex_manager, land_program);
	use_br_object_manager(w->obj_manager, land_program);
}

void use_world_batch_water(world_batch *w, GLuint water_program)
{
	if (w->w != 0)
	{
		use_water(w->w, water_program);
	}
}

void delete_world_batch(world_batch *w)
{
	if (w != 0)
	{
		delete_br_object_manager(w->obj_manager);
		if (w->w != 0)
		{
			delete_water(w->w);
		}
		free(w);
	}
}

br_texture_manager *get_world_texture_manager(void)
{
	return tex_manager;
}

void delete_world_texture_manager(void)
{
	if (tex_manager != 0)
	{
		delete_br_texture_manager(tex_manager);
		tex_manager = 0;
	}
}