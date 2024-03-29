#pragma once
#include "camera.h"

typedef struct skybox
{
  GLuint VAO, VBO, EBO, cubemap;
  camera *cam;
  mat4 result;
  DA *programs; // i will save uniforms here. i wont find their locations everytime i render for performance
  DA *uniforms;
  float rotate_frame;
  float rotate_full;
  vec3 rotate_axis;
} skybox;

skybox *create_skybox(const char *right_texture, const char *left_texture, const char *top_texture,
                      const char *bottom_texture, const char *front_texture, const char *back_texture,
                      camera *cam, float rotate_frame_ms, vec3 rotate_axis);

void delete_skybox(skybox *s);

void use_skybox(skybox *s, GLuint program);