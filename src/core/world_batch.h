#pragma once
#include "br_object.h"
#include "br_texture.h"
#include "water.h"

typedef struct world_batch
{
	br_object_manager *obj_manager;
	water *w;
	int chunk_id;
} world_batch;

world_batch *create_world_batch(int **hm, int startx, int startz, int widthx, int widthz,
																int dimensionx, int dimensionz, vec3 lightdir, float sealevel,
																unsigned char create_water_physic);

world_batch *create_world_batch_facemerged(int **hm, int startx, int startz, int widthx, int widthz,
																					 int dimensionx, int dimensionz, float sealevel,
																					 unsigned char create_water_physic, unsigned char **done);

void use_world_batch_land(world_batch *w, GLuint land_program);

void use_world_batch_water(world_batch *w, GLuint water_program);

void delete_world_batch(world_batch *w);

br_texture_manager *get_world_texture_manager(void);

void delete_world_texture_manager(void);
