#pragma once
#include "../core/core.h"

typedef struct world_instanced
{
	ins_object_manager *obj_manager;
	br_texture_manager *tex_manager;
} world_instanced;

world_instanced *create_world_instanced(int **hm, int dimensionx, int dimensionz);

void use_world_instanced(world_instanced *w, GLuint program);

void delete_world_instanced(world_instanced *w);