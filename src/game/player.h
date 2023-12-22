#pragma once
#include "../core/core.h"

typedef struct player
{
  camera *fp_camera;
  float speed;
  vec2 minxy;
  vec2 maxxy;
} player;

void run_input_player(player *p, GLFWwindow *window, long framems);
