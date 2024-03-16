#pragma once
#include "../core/core.h"

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
  int **hm;
  int dimensionx;
  int dimensionz;
  double jumpstartms;
  double jumpdurationms;
  playerid *phy;
  br_object_manager *model;
  br_texture_manager *textures;
} player;

void run_input_fp_player(player *p, GLFWwindow *window, double framems);
