#include "chunk.h"
#include "core.h"

struct aiScene *gsu_model = 0;
int gsu_x = 64, gsu_z = 32, gsu_y = 50, gsu_h = 16;
unsigned char gsu_can_exist = 0;

void set_gsu_model(struct aiScene *model)
{
  gsu_model = model;
}

chunk_op *create_chunk_op(unsigned int chunk_size, unsigned int chunk_range, player *p, int **hm,
                          int dimensionx, int dimensionz, vec3 lightdir, float sealevel, unsigned char facemerged)
{
  if (dimensionx > 2 * gsu_x && dimensionz > 2 * gsu_z && gsu_model != 0)
  {
    gsu_can_exist = 1;
  }
  chunk_op *c = malloc(sizeof(chunk_op));
  c->batch = create_DA_HIGH_MEMORY(sizeof(world_batch *));
  c->allbatch = create_DA_HIGH_MEMORY(sizeof(world_batch *));
  c->chunkinfo = create_DA_HIGH_MEMORY(sizeof(chunk_info));
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
      if (i == c->chunknumberinrow / 2 && i2 == c->chunknumberincolumn / 2 && gsu_can_exist)
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
  trim_DA(c->chunkinfo);
  chunk_info *y = get_data_DA(c->chunkinfo);
  for (unsigned int i = 0; i < get_size_DA(c->chunkinfo); i++)
  {
    world_batch *batch = 0;
    if (facemerged)
    {
      if (i == 0)
      {
        batch = create_world_batch_facemerged(c->hm, y[i].startx, y[i].startz, c->chunk_size,
                                              c->chunk_size, c->dimensionx, c->dimensionz, sealevel, 1);
      }
      else
      {
        batch = create_world_batch_facemerged(c->hm, y[i].startx, y[i].startz, c->chunk_size,
                                              c->chunk_size, c->dimensionx, c->dimensionz, sealevel, 0);
      }
    }
    else
    {
      if (i == 0)
      {
        batch = create_world_batch(c->hm, y[i].startx, y[i].startz, c->chunk_size,
                                   c->chunk_size, c->dimensionx, c->dimensionz, lightdir, sealevel, 1);
      }
      else
      {
        batch = create_world_batch(c->hm, y[i].startx, y[i].startz, c->chunk_size,
                                   c->chunk_size, c->dimensionx, c->dimensionz, lightdir, sealevel, 0);
      }
    }
    batch->chunk_id = i;
    pushback_DA(c->allbatch, &batch);
    if (i == c->centerchunkid && gsu_can_exist)
    {
      br_scene gsu = load_object_br(batch->obj_manager, get_world_texture_manager(), gsu_model, 7, 0, 3, 10, 0.1f, 0.5f);
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
      free(gsu.textures);
      free(gsu.meshes);
    }
    delete_cpu_memory_br_object_manager(batch->obj_manager);
  }
  trim_DA(c->allbatch);
  c->previous_ids = 0;
  c->current_ids = malloc(sizeof(int) * c->renderedchunkcount);
  c->delete_ids = create_DA_HIGH_MEMORY(sizeof(int));
  return c;
}

void delete_chunk_op(chunk_op *c)
{
  world_batch **x = get_data_DA(c->batch);
  for (unsigned int i = 0; i < get_size_DA(c->batch); i++)
  {
    remove_animation_translate_br_manager(x[i]->obj_manager);
  }
  delete_DA(c->batch);
  world_batch **y = get_data_DA(c->allbatch);
  for (unsigned int i = 0; i < get_size_DA(c->allbatch); i++)
  {
    delete_world_batch(y[i]);
  }
  delete_DA(c->allbatch);
  delete_DA(c->chunkinfo);
  free(c->previous_ids);
  free(c->current_ids);
  delete_DA(c->delete_ids);
  free(c);
  delete_world_texture_manager();
  delete_water_texture_manager();
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

void update_chunk_op(chunk_op *c, unsigned char animation)
{
  // remove deleted chunks after remove animation
  world_batch **z = get_data_DA(c->allbatch);
remove:
  int *delids = get_data_DA(c->delete_ids);
  for (unsigned int i = 0; i < get_size_DA(c->delete_ids); i++)
  {
    if (has_animation_br_manager(z[delids[i]]->obj_manager) == 0)
    {
      remove_DA(c->batch, get_index_DA(c->batch, &(z[delids[i]])));
      remove_DA(c->delete_ids, i);
      goto remove;
    }
  }

  // dont calculate if it is in same chunk
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

  // figure out which ids will be rendered
  int index = 0;
  int index2 = 0;
  for (unsigned int i = 0; i <= 2 * c->chunk_range; i++)
  {
    for (unsigned int i2 = 0; i2 <= 2 * c->chunk_range; i2++)
    {
      index = current_id - (c->chunk_range - i) -
              ((c->chunk_range - i2) * c->chunknumberinrow);
      c->current_ids[index2] = index;
      index2++;
    }
  }

  // add the new ids
  for (int i = 0; i < c->renderedchunkcount; i++)
  {
    index = c->current_ids[i];
    if (index < 0 || index >= (int)get_size_DA(c->chunkinfo))
    {
      continue;
    }
    if (c->previous_chunkid != -1 && c->previous_ids != 0)
    {
      if (inarray(index, c->previous_ids, c->renderedchunkcount) == 0)
      {
        pushback_DA(c->batch, &(z[index]));
        if (animation)
        {
          glm_mat4_copy(GLM_MAT4_IDENTITY, z[index]->obj_manager->model);
          glm_mat4_copy(GLM_MAT4_IDENTITY, z[index]->obj_manager->normal);
          glm_mat4_copy(GLM_MAT4_IDENTITY, z[index]->obj_manager->translation);
          glm_mat4_copy(GLM_MAT4_IDENTITY, z[index]->obj_manager->rotation);
          glm_mat4_copy(GLM_MAT4_IDENTITY, z[index]->obj_manager->scale);
          translate_br_object_all(z[index]->obj_manager, (vec3){0.0f, -100, 0.0f});
          add_animation_translate_br_manager(z[index]->obj_manager, (vec3){0.0f, 100, 0.0f}, 1500);
          if (z[index]->w != 0)
          {
            glm_mat4_copy(GLM_MAT4_IDENTITY, z[index]->w->obj->model);
            glm_mat4_copy(GLM_MAT4_IDENTITY, z[index]->w->obj->normal);
            glm_mat4_copy(GLM_MAT4_IDENTITY, z[index]->w->obj->translation);
            glm_mat4_copy(GLM_MAT4_IDENTITY, z[index]->w->obj->rotation);
            glm_mat4_copy(GLM_MAT4_IDENTITY, z[index]->w->obj->scale);
            translate_br_object_all(z[index]->w->obj, (vec3){0.0f, -100, 0.0f});
            add_animation_translate_br_manager(z[index]->w->obj, (vec3){0.0f, 100, 0.0f}, 1500);
          }
        }
      }
    }
    else
    {
      pushback_DA(c->batch, &(z[index]));
    }
  }

  // add the old ids remove animation
  if (c->previous_ids != 0)
  {
    for (int i = 0; i < c->renderedchunkcount; i++)
    {
      if (inarray(c->previous_ids[i], c->current_ids, c->renderedchunkcount) == 0 &&
          c->previous_ids[i] >= 0 && c->previous_ids[i] < (int)get_size_DA(c->chunkinfo))
      {
        pushback_DA(c->delete_ids, &(c->previous_ids[i]));
        if (animation)
        {
          add_animation_translate_br_manager(z[c->previous_ids[i]]->obj_manager, (vec3){0.0f, -100, 0.0f}, 1500);
          if (z[c->previous_ids[i]]->w != 0)
          {
            add_animation_translate_br_manager(z[c->previous_ids[i]]->w->obj, (vec3){0.0f, -100, 0.0f}, 1500);
          }
        }
      }
    }
  }

  if (c->previous_ids == 0)
  {
    c->previous_ids = malloc(sizeof(int) * c->renderedchunkcount);
  }
  for (int i = 0; i < c->renderedchunkcount; i++)
  {
    c->previous_ids[i] = c->current_ids[i];
  }

  c->previous_chunkid = current_id;
}

void use_chunk_op(chunk_op *c, GLuint program, camera *cam, unsigned char land0_water1)
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
        glm_vec3_distance((vec3){cam->position[0], 0, cam->position[2]}, (vec3){center[0], 0, center[2]}) <= (float)c->chunk_size * 4.0f)
    {
      if (land0_water1 == 0)
      {
        use_world_batch_land(x[i], program);
      }
      else
      {
        use_world_batch_water(x[i], program);
      }
    }
  }
}