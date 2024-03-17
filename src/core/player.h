#pragma once
#include "camera.h"
#include "br_object.h"
#include "br_texture.h"
#include "jolt_physics.h"

typedef struct player
{
  camera *fp_camera;
  float speed;
  float jumpspeed;
  float airspeedfactor;
  float boostspeedfactor;
  float width;
  float height;
  unsigned char jumping;
  unsigned char onland;
  unsigned char horizontalcontrol;
  int **hm;
  int dimensionx;
  int dimensionz;
  double jumpstartms;
  double jumpdurationms;
  playerid *phy;
  br_object_manager *model;
  br_texture_manager *textures;
} player;

player *create_player(camera *fp_camera, float speed, float jumpspeed, float airspeedfactor,
                      float boostspeedfactor, float width, float height, int **hm, int dimensionx,
                      int dimensionz, const char *modelpath, float *start_pos, float maxslopeangle,
                      float maxstrength, float mass, unsigned char scaleall0_scaleonlyheight1);

void delete_player(player *p);

void run_input_player(player *p, GLFWwindow *window, double framems, unsigned char fp0_tp1);

void render_player(player *p, GLuint program);
