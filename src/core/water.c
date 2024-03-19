#include "water.h"
#include "core.h"

GLfloat vertices[] = {
    0.5f, 5, -0.5f, 0, 0, 0, 1, 0, 0,  // 0
    -0.5f, 5, -0.5f, 1, 0, 0, 1, 0, 0, // 1
    -0.5f, 5, 0.5f, 1, 1, 0, 1, 0, 0,  // 2
    0.5f, 5, 0.5f, 0, 1, 0, 1, 0, 0,   // 3
};

GLuint indices[] = {
    2, 1, 0, //
    0, 3, 2, //
};

water *create_water(float sealevel, const char *texture_path, int startx, int startz, int widthx, int widthz)
{
  water *w = malloc(sizeof(water));
  w->obj = create_br_object_manager();
  w->texture = create_br_texture_manager();
  vertices[1] = sealevel;
  vertices[10] = sealevel;
  vertices[19] = sealevel;
  vertices[28] = sealevel;
  create_br_texture(w->texture, texture_path, GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST, 0);
  for (int i = startx; i < startx + widthx; i++)
  {
    for (int i2 = startz; i2 < startz + widthz; i2++)
    {
      br_object *tmp = create_br_object(w->obj, vertices, 4, indices, 6, 0, 0, 0, 0, 0, 0);
      translate_br_object(tmp, (vec3){(float)i, 0, (float)i2}, 0);
    }
  }
  prepare_render_br_object_manager(w->obj);
  create_water_jolt(sealevel, 1.1f, 0.3f, 0.05f);
  return w;
}

void delete_water(water *w)
{
  delete_br_object_manager(w->obj);
  delete_br_texture_manager(w->texture);
  free(w);
}

void use_water(water *w, GLuint program)
{
  use_br_texture_manager(w->texture, program);
  use_br_object_manager(w->obj, program);
}