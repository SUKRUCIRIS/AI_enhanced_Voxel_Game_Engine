#pragma once
#include "ins_object.h"
#include "br_texture.h"

typedef struct world_instanced
{
	ins_object_manager *obj_manager_left;
	ins_object_manager *obj_manager_right;
	ins_object_manager *obj_manager_front;
	ins_object_manager *obj_manager_back;
	ins_object_manager *obj_manager_top;
	br_texture_manager *tex_manager;
} world_instanced;

world_instanced *create_world_instanced(int **hm, int dimensionx, int dimensionz);

void use_world_instanced(world_instanced *w, GLuint program);

void delete_world_instanced(world_instanced *w);