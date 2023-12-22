#include "chunk.h"
#include "world_batch.h"

chunk_op *create_chunk_op(unsigned int chunk_size, unsigned int chunk_range, player *p, int **hm, int dimensionx, int dimensionz)
{
  chunk_op *c = malloc(sizeof(chunk_op));
  c->batch = create_DA(sizeof(world_batch *));
  c->chunkinfo = create_DA(sizeof(chunk_info));
  c->chunk_size = chunk_size;
  c->chunk_range = chunk_range;
  c->p = p;
  c->hm = hm;
  c->dimensionx = dimensionx;
  c->dimensionz = dimensionz;
  c->previous_chunkid = -1;
  c->chunknumberinrow = (int)ceilf((float)c->dimensionx / c->chunk_size);
  c->renderedchunkcount = (c->chunk_range * 2 + 1) * (c->chunk_range * 2 + 1);
  for (int i = 0; i < (int)ceilf((float)c->dimensionx / c->chunk_size); i++)
  {
    for (int i2 = 0; i2 < (int)ceilf((float)c->dimensionz / c->chunk_size); i2++)
    {
      chunk_info x = {
          .startx = i * c->chunk_size,
          .startz = i2 * c->chunk_size,
          .minxy = {
              (float)(i * c->chunk_size) - (int)(dimensionx / 2),
              (float)(i2 * c->chunk_size) - (int)(dimensionz / 2)},
          .maxxy = {(float)((i + 1) * c->chunk_size) - (int)(dimensionx / 2), (float)((i2 + 1) * c->chunk_size) - (int)(dimensionz / 2)}};
      x.minxy[0] -= 1;
      x.minxy[1] -= 1;
      x.maxxy[0] += 1;
      x.maxxy[1] += 1;
      pushback_DA(c->chunkinfo, &x);
    }
  }
  return c;
}

void delete_chunk_op(chunk_op *c)
{
  world_batch **x = get_data_DA(c->batch);
  for (unsigned int i = 0; i < get_size_DA(c->batch); i++)
  {
    delete_world_batch(x[i]);
  }
  delete_DA(c->batch);
  delete_DA(c->chunkinfo);
  free(c);
  delete_world_texture_manager();
}

unsigned char inarray(int x, int *array, int size)
{
  for (int i = 0; i < size; i++)
  {
    if (array[i] == x)
    {
      return 1;
    }
  }
  return 0;
}

void update_chunk_op(chunk_op *c)
{
  float *pos = c->p->fp_camera->position;
  chunk_info *y = get_data_DA(c->chunkinfo);
  if (c->previous_chunkid != -1 && y[c->previous_chunkid].minxy[0] <= pos[0] && y[c->previous_chunkid].minxy[1] <= pos[1] &&
      y[c->previous_chunkid].maxxy[0] >= pos[0] && y[c->previous_chunkid].maxxy[1] >= pos[1])
  {
    return;
  }

  int current_id = c->previous_chunkid;
  for (unsigned int i = 0; i < get_size_DA(c->chunkinfo); i++)
  {
    if (y[i].minxy[0] <= pos[0] && y[i].minxy[1] <= pos[1] &&
        y[i].maxxy[0] >= pos[0] && y[i].maxxy[1] >= pos[1])
    {
      current_id = i;
      break;
    }
  }
  c->previous_chunkid = current_id;

  // find out which ids will be rendered
  int *wanted_ids = malloc(sizeof(int) * c->renderedchunkcount);
  for (unsigned int i = 0; i <= 2 * c->chunk_range; i++)
  {
    for (unsigned int i2 = 0; i2 <= 2 * c->chunk_range; i2++)
    {
      wanted_ids[i + i2 * i] = current_id - (c->chunk_range - i) -
                               ((c->chunk_range - i2) * c->chunknumberinrow);
    }
  }

  {
  delete_unwanted:
    world_batch **x = get_data_DA(c->batch);
    for (unsigned int i = 0; i < get_size_DA(c->batch); i++)
    {
      if (inarray(x[i]->chunk_id, wanted_ids, c->renderedchunkcount) == 0)
      {
        delete_world_batch(x[i]);
        remove_DA(c->batch, i);
        goto delete_unwanted;
      }
    }
  }

  {
  add_wanted:
    world_batch **x = get_data_DA(c->batch);
    char found = 0;
    for (int i = 0; i < c->renderedchunkcount; i++)
    {
      found = 0;
      for (unsigned int i2 = 0; i2 < get_size_DA(c->batch); i2++)
      {
        if (wanted_ids[i] == x[i2]->chunk_id)
        {
          found = 1;
          break;
        }
      }
      if (found == 0)
      {
        world_batch *batch = create_world_batch(c->hm, y[wanted_ids[i]].startx, y[wanted_ids[i]].startz,
                                                c->chunk_size, c->chunk_size, c->dimensionx, c->dimensionz);
        batch->chunk_id = wanted_ids[i];
        pushback_DA(c->batch, &batch);
        goto add_wanted;
      }
    }
  }

  free(wanted_ids);
}

void use_chunk_op(chunk_op *c, GLuint program)
{
  world_batch **x = get_data_DA(c->batch);
  for (unsigned int i = 0; i < get_size_DA(c->batch); i++)
  {
    use_world_batch(x[i], program);
  }
}