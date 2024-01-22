#include "chunk.h"
#include "world_batch.h"

struct aiScene *gsu_model = 0;
int gsu_x = 64, gsu_z = 32, gsu_y = 50, gsu_h = 16;

void set_gsu_model(struct aiScene *model)
{
  gsu_model = model;
}

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
  c->chunknumberincolumn = (int)ceilf((float)c->dimensionz / c->chunk_size);
  c->renderedchunkcount = (c->chunk_range * 2 + 1) * (c->chunk_range * 2 + 1);
  for (int i = 0; i < c->chunknumberinrow; i++)
  {
    for (int i2 = 0; i2 < c->chunknumberincolumn; i2++)
    {
      chunk_info x = {
          .startx = i * c->chunk_size,
          .startz = i2 * c->chunk_size,
          .minz = -200,
          .maxz = 200,
          .minxy = {
              (float)(i * c->chunk_size) - (int)(dimensionx / 2),
              (float)(i2 * c->chunk_size) - (int)(dimensionz / 2)},
          .maxxy = {(float)((i + 1) * c->chunk_size) - (int)(dimensionx / 2), (float)((i2 + 1) * c->chunk_size) - (int)(dimensionz / 2)}};
      x.minxy[0] -= 1;
      x.minxy[1] -= 1;
      x.maxxy[0] += 1;
      x.maxxy[1] += 1;
      pushback_DA(c->chunkinfo, &x);
      if (i == c->chunknumberinrow / 2 && i2 == c->chunknumberincolumn / 2)
      {
        c->centerchunkid = get_size_DA(c->chunkinfo) - 1;
        // setting gsu terrain
        for (int ix = -gsu_x / 2; ix < (int)(1.5f * gsu_x); ix++)
        {
          for (int iz = -gsu_z / 2; iz < (int)(1.5f * gsu_z); iz++)
          {
            hm[x.startx + ix][x.startz + iz] = gsu_y;
          }
        }
      }
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

void update_chunk_op(chunk_op *c, vec3 lightdir)
{
  float *pos = c->p->fp_camera->position;
  chunk_info *y = get_data_DA(c->chunkinfo);
  if (c->previous_chunkid != -1 && y[c->previous_chunkid].minxy[0] <= pos[0] && y[c->previous_chunkid].minxy[1] <= pos[2] &&
      y[c->previous_chunkid].maxxy[0] >= pos[0] && y[c->previous_chunkid].maxxy[1] >= pos[2])
  {
    return;
  }

  int current_id = c->previous_chunkid;
  char found = 0;
  int columnnumber = 0; // first find column then find chunk for performance
  for (int i = 0; i < c->chunknumberinrow; i++)
  {
    if (y[i * c->chunknumberincolumn].minxy[0] <= pos[0] && y[i * c->chunknumberincolumn].minxy[1] <= pos[2] &&
        y[(i + 1) * c->chunknumberincolumn - 1].maxxy[0] >= pos[0] && y[(i + 1) * c->chunknumberincolumn - 1].maxxy[1] >= pos[2])
    {
      columnnumber = i;
      found = 1;
      break;
    }
  }
  if (found == 0)
  {
    c->previous_chunkid = -1;
    return;
  }
  for (int i = 0; i < c->chunknumberincolumn; i++)
  {
    if (y[columnnumber * c->chunknumberincolumn + i].minxy[0] <= pos[0] && y[columnnumber * c->chunknumberincolumn + i].minxy[1] <= pos[2] &&
        y[columnnumber * c->chunknumberincolumn + i].maxxy[0] >= pos[0] && y[columnnumber * c->chunknumberincolumn + i].maxxy[1] >= pos[2])
    {
      current_id = columnnumber * c->chunknumberincolumn + i;
      break;
    }
  }

  // find out which ids will be rendered
  int *wanted_ids = malloc(sizeof(int) * c->renderedchunkcount);
  int index = 0;
  for (unsigned int i = 0; i <= 2 * c->chunk_range; i++)
  {
    for (unsigned int i2 = 0; i2 <= 2 * c->chunk_range; i2++)
    {
      wanted_ids[index] = current_id - (c->chunk_range - i) -
                          ((c->chunk_range - i2) * c->chunknumberinrow);
      index++;
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
    for (int i = 0; i < c->renderedchunkcount; i++)
    {
      if (wanted_ids[i] < 0 || wanted_ids[i] >= (int)get_size_DA(c->chunkinfo))
      {
        continue;
      }
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
                                                c->chunk_size, c->chunk_size, c->dimensionx, c->dimensionz, lightdir);
        batch->chunk_id = wanted_ids[i];
        pushback_DA(c->batch, &batch);
        if (wanted_ids[i] == c->centerchunkid)
        {
          br_scene gsu = load_object_br(batch->obj_manager, get_world_texture_manager(), gsu_model, 8, 0, 3, 10, 0.1f, 0.5f);
          float scalex = gsu_x / (gsu.box.mMax.x - gsu.box.mMin.x),
                scaley = gsu_h / (gsu.box.mMax.y - gsu.box.mMin.y),
                scalez = gsu_z / (gsu.box.mMax.z - gsu.box.mMin.z);
          gsu.box.mMin.y *= scaley;
          gsu.box.mMax.y *= scaley;
          gsu.box.mMin.x *= scalex;
          gsu.box.mMax.x *= scalex;
          gsu.box.mMin.z *= scalez;
          gsu.box.mMax.z *= scalez;
          for (unsigned int i2 = 0; i2 < gsu.mesh_count; i2++)
          {
            scale_br_object(gsu.meshes[i2], (vec3){scalex, scaley, scalez}, 0);
            translate_br_object(gsu.meshes[i2], (vec3){y[c->centerchunkid].minxy[0] - gsu.box.mMin.x, gsu_y + 0.5f - gsu.box.mMin.y, y[c->centerchunkid].minxy[1] - gsu.box.mMin.z}, 0);
          }
          prepare_render_br_object_manager(batch->obj_manager);
        }
        if (c->previous_chunkid != -1)
        {
          translate_br_object_all(batch->obj_manager, (vec3){0.0f, -100, 0.0f}, 0);
          add_animation_translate_br_manager(batch->obj_manager, (vec3){0.0f, 100, 0.0f}, 0, 1000);
        }
        goto add_wanted;
      }
    }
  }
  c->previous_chunkid = current_id;
  free(wanted_ids);
}

void use_chunk_op(chunk_op *c, GLuint program, camera *cam)
{
  calculate_camera(cam, cam->nearPlane, cam->farPlane);
  world_batch **x = get_data_DA(c->batch);
  chunk_info *y = get_data_DA(c->chunkinfo);
  vec3 box2[2];
  vec4 planes[6] = {0};
  glm_frustum_planes(cam->result, planes);
  vec3 center;
  for (unsigned int i = 0; i < get_size_DA(c->batch); i++)
  {
    box2[0][0] = y[x[i]->chunk_id].minxy[0];
    box2[0][1] = y[x[i]->chunk_id].minz;
    box2[0][2] = y[x[i]->chunk_id].minxy[1];
    box2[1][0] = y[x[i]->chunk_id].maxxy[0];
    box2[1][1] = y[x[i]->chunk_id].maxz;
    box2[1][2] = y[x[i]->chunk_id].maxxy[1];
    glm_aabb_center(box2, center);
    if (glm_aabb_frustum(box2, planes) ||
        glm_vec3_distance(cam->position, center) <= (float)c->chunk_size * 4.0f)
    {
      use_world_batch(x[i], program);
    }
  }
}