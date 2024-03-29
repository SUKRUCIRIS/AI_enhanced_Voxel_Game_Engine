#pragma once
#include "player.h"
#include "dynamic.h"
#include "camera.h"
#include "load_object.h"

typedef struct chunk_op
{
  DA *chunkinfo;
  DA *batch;
  DA *allbatch;
  unsigned int chunk_size;
  unsigned int chunk_range;
  player *p;
  int **hm;
  int dimensionx, dimensionz;
  int previous_chunkid;
  int chunknumberinrow;
  int chunknumberincolumn;
  int renderedchunkcount;
  unsigned int centerchunkid;
  int *previous_ids;
  int *current_ids;
  DA *delete_ids;
} chunk_op;

typedef struct chunk_info
{
  vec2 minxy, maxxy;
  float minz, maxz;
  int startx, startz;
} chunk_info;

chunk_op *create_chunk_op(unsigned int chunk_size, unsigned int chunk_range, player *p, int **hm,
                          int dimensionx, int dimensionz, vec3 lightdir, float sealevel, unsigned char facemerged);

void delete_chunk_op(chunk_op *c);

void update_chunk_op(chunk_op *c, unsigned char animation);

void use_chunk_op(chunk_op *c, GLuint program, camera *cam, unsigned char land0_water1);

void set_gsu_model(struct aiScene *model);

int get_world_triangle_count(void);

int get_rendered_triangle_count(void);