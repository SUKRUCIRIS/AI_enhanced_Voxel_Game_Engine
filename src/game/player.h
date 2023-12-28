#pragma once
#include "../core/core.h"

typedef struct player
{
  camera *fp_camera;
  float speed;
  float width;
  float height;
  unsigned char jumping;
  unsigned char onland;
  int **hm;
  int dimensionx;
  int dimensionz;
  double jumpstartms;
  double jumpdurationms;
} player;

void run_input_player(player *p, GLFWwindow *window, double framems);
