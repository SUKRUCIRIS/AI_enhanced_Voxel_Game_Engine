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

br_texture_manager *water_texture = 0;

water *create_water(float sealevel, const char *texture_path, int startx,
                    int startz, int widthx, int widthz, unsigned char create_physic)
{
  water *w = malloc(sizeof(water));
  w->obj = create_br_object_manager();
  if (water_texture == 0)
  {
    water_texture = create_br_texture_manager();
    create_br_texture(water_texture, texture_path, GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST, 0);
  }
  vertices[1] = sealevel;
  vertices[10] = sealevel;
  vertices[19] = sealevel;
  vertices[28] = sealevel;
  for (int i = startx; i < startx + widthx; i++)
  {
    for (int i2 = startz; i2 < startz + widthz; i2++)
    {
      br_object *tmp = create_br_object(w->obj, vertices, 4, indices, 6, 0, 0, 0, 0, 0, 0);
      translate_br_object(tmp, (vec3){(float)i, 0, (float)i2}, 0);
    }
  }
  prepare_render_br_object_manager(w->obj);
  if (create_physic)
  {
    create_water_jolt(sealevel, 1.1f, 0.3f, 0.05f);
  }
  return w;
}

void delete_water(water *w)
{
  delete_br_object_manager(w->obj);
  free(w);
}

void use_water(water *w, GLuint program)
{
  use_br_texture_manager(water_texture, program);
  use_br_object_manager(w->obj, program);
}

void delete_water_texture_manager(void)
{
  delete_br_texture_manager(water_texture);
}